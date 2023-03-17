/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "StepResultSet"

#include "step_result_set.h"

#include <unistd.h>

#include "logger.h"
#include "rdb_errno.h"
#include "rdb_trace.h"
#include "sqlite3sym.h"
#include "sqlite_database_utils.h"
#include "sqlite_errno.h"
#include "sqlite_utils.h"

namespace OHOS {
namespace NativeRdb {
StepResultSet::StepResultSet(std::shared_ptr<RdbStoreImpl> rdb, const std::string &sql,
    const std::vector<std::string> &selectionArgs)
    : rdb(rdb), sql(sql), selectionArgs(selectionArgs), isAfterLast(false), rowCount(INIT_POS),
      sqliteStatement(nullptr), connectionPool_(nullptr), connection_(nullptr)
{
}
StepResultSet::StepResultSet(SqliteConnectionPool *pool, const std::string &sql,
                             const std::vector<std::string> &selectionArgs)
    : sql(sql), selectionArgs(selectionArgs), isAfterLast(false), rowCount(INIT_POS),
      sqliteStatement(nullptr)
{
    connectionPool_ = pool;
    connection_ = connectionPool_->AcquireConnection(true);
}

StepResultSet::~StepResultSet()
{
    Close();
    connectionPool_->ReleaseConnection(connection_);
    connection_ = nullptr;
}

int StepResultSet::GetAllColumnNames(std::vector<std::string> &columnNames)
{
    if (isClosed) {
        return E_STEP_RESULT_CLOSED;
    }

    if (!columnNames_.empty()) {
        columnNames = columnNames_;
        return E_OK;
    }

    int errCode = PrepareStep();
    if (errCode) {
        return errCode;
    }

    int columnCount = 0;
    errCode = sqliteStatement->GetColumnCount(columnCount);
    if (errCode) {
        return errCode;
    }

    columnNames.clear();
    for (int i = 0; i < columnCount; i++) {
        std::string columnName;
        errCode = sqliteStatement->GetColumnName(i, columnName);
        if (errCode) {
            columnNames.clear();
            return errCode;
        }
        columnNames.push_back(columnName);
    }

    return E_OK;
}

int StepResultSet::GetColumnType(int columnIndex, ColumnType &columnType)
{
    if (isClosed) {
        return E_STEP_RESULT_CLOSED;
    }

    if (rowPos_ == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }
    int sqliteType;
    int errCode = sqliteStatement->GetColumnType(columnIndex, sqliteType);
    if (errCode) {
        return errCode;
    }

    switch (sqliteType) {
        case SQLITE_INTEGER:
            columnType = ColumnType::TYPE_INTEGER;
            break;
        case SQLITE_FLOAT:
            columnType = ColumnType::TYPE_FLOAT;
            break;
        case SQLITE_BLOB:
            columnType = ColumnType::TYPE_BLOB;
            break;
        case SQLITE_NULL:
            columnType = ColumnType::TYPE_NULL;
            break;
        default:
            columnType = ColumnType::TYPE_STRING;
    }

    return E_OK;
}

int StepResultSet::GetRowCount(int &count)
{
    if (rowCount != INIT_POS) {
        count = rowCount;
        return E_OK;
    }
    int oldPosition = 0;
    // Get the start position of the query result
    GetRowIndex(oldPosition);

    while (GoToNextRow() == E_OK) {
    }
    count = rowCount;
    // Reset the start position of the query result
    GoToRow(oldPosition);

    return E_OK;
}

/**
 * Moves the result set to a specified position
 */
int StepResultSet::GoToRow(int position)
{
    if (!connection_) {
        return E_ERROR;
    }
    // If the moved position is less than zero, reset the result and return an error
    if (position < 0) {
        Reset();
        return E_ERROR;
    }
    if (position == rowPos_) {
        return E_OK;
    }
    if (position < rowPos_) {
        Reset();
        return GoToRow(position);
    }
    while (position != rowPos_) {
        int errCode = GoToNextRow();
        if (errCode) {
            return errCode;
        }
    }

    return E_OK;
}

/**
 * Move the result set to the next row
 */
int StepResultSet::GoToNextRow()
{
    int errCode = PrepareStep();
    if (errCode) {
        return errCode;
    }

    int retryCount = 0;
    errCode = sqliteStatement->Step();

    while (errCode == SQLITE_LOCKED || errCode == SQLITE_BUSY) {
        // The table is locked, retry
        if (retryCount > STEP_QUERY_RETRY_MAX_TIMES) {
            LOG_ERROR("StepResultSet::GoToNextRow retrycount exceeded");
            return E_STEP_RESULT_QUERY_EXCEEDED;
        } else {
            // Sleep to give the thread holding the lock a chance to finish
            usleep(STEP_QUERY_RETRY_INTERVAL);
            errCode = sqliteStatement->Step();
            retryCount++;
        }
    }

    if (errCode == SQLITE_ROW) {
        rowPos_++;
        return E_OK;
    } else if (errCode == SQLITE_DONE) {
        isAfterLast = true;
        rowCount = rowPos_ + 1;
        FinishStep();
        return E_STEP_RESULT_IS_AFTER_LAST;
    } else {
        LOG_ERROR("StepResultSet::GoToNextRow step err = %{public}d", errCode);
        FinishStep();
        return SQLiteError::ErrNo(errCode);
    }
}

int StepResultSet::Close()
{
    if (isClosed) {
        return E_OK;
    }
    isClosed = true;
    int errCode = FinishStep();
    rdb = nullptr;
    return errCode;
}

/**
 * Obtain session and prepare precompile statement for step query
 */
int StepResultSet::PrepareStep()
{
    if (isClosed) {
        return E_STEP_RESULT_CLOSED;
    }

    if (sqliteStatement != nullptr) {
        return E_OK;
    }

    if (!SqliteDatabaseUtils::IsReadOnlySql(sql)) {
        LOG_ERROR("StoreSession BeginStepQuery fail : not select sql !");
        return E_EXECUTE_IN_STEP_QUERY;
    }

    int errCode;
    sqliteStatement = connection_->BeginStepQuery(errCode, sql, selectionArgs);
    if (sqliteStatement == nullptr) {
        connection_->EndStepQuery();
        return errCode;
    }

    return E_OK;
}

/**
 * Release resource of step result set, this method can be called more than once
 */
int StepResultSet::FinishStep()
{
    if (sqliteStatement == nullptr) {
        return E_OK;
    }

    sqliteStatement = nullptr;
    rowPos_ = INIT_POS;
    if (connection_ == nullptr) {
        return E_OK;
    }

    int errCode = connection_->EndStepQuery();
    if (errCode != E_OK) {
        LOG_ERROR("StepResultSet::FinishStep err = %d", errCode);
    }
    return errCode;
}

/**
 * Reset the statement
 */
void StepResultSet::Reset()
{
    if (sqliteStatement != nullptr) {
        sqlite3_reset(sqliteStatement->GetSql3Stmt());
    }
    rowPos_ = INIT_POS;
    isAfterLast = false;
}


/**
 * Checks whether the result set is positioned after the last row
 */
int StepResultSet::IsEnded(bool &result)
{
    result = isAfterLast;
    return E_OK;
}

/**
 * Checks whether the result set is moved
 */
int StepResultSet::IsStarted(bool &result) const
{
    result = (rowPos_ != INIT_POS);
    return E_OK;
}

/**
 * Check whether the result set is in the first row
 */
int StepResultSet::IsAtFirstRow(bool &result) const
{
    result = (rowPos_ == 0);
    return E_OK;
}

int StepResultSet::GetBlob(int columnIndex, std::vector<uint8_t> &blob)
{
    if (rowPos_ == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }

    return sqliteStatement->GetColumnBlob(columnIndex, blob);
}

int StepResultSet::GetString(int columnIndex, std::string &value)
{
    if (rowPos_ == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }

    int errCode = sqliteStatement->GetColumnString(columnIndex, value);
    if (errCode != E_OK) {
        LOG_ERROR("StepResultSet::GetString is err=%{public}d", errCode);
        return errCode;
    }
    return E_OK;
}

int StepResultSet::GetInt(int columnIndex, int &value)
{
    if (rowPos_ == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }

    int64_t columnValue;
    int errCode = sqliteStatement->GetColumnLong(columnIndex, columnValue);
    if (errCode != E_OK) {
        return errCode;
    }
    value = static_cast<int>(columnValue);
    return E_OK;
}

int StepResultSet::GetLong(int columnIndex, int64_t &value)
{
    if (rowPos_ == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }
    int errCode = sqliteStatement->GetColumnLong(columnIndex, value);
    if (errCode != E_OK) {
        return errCode;
    }
    return E_OK;
}

int StepResultSet::GetDouble(int columnIndex, double &value)
{
    if (rowPos_ == INIT_POS) {
        return E_STEP_RESULT_QUERY_NOT_EXECUTED;
    }
    int errCode = sqliteStatement->GetColumnDouble(columnIndex, value);
    if (errCode != E_OK) {
        return errCode;
    }
    return E_OK;
}

int StepResultSet::IsColumnNull(int columnIndex, bool &isNull)
{
    ColumnType columnType;
    int errCode = GetColumnType(columnIndex, columnType);
    if (errCode != E_OK) {
        return errCode;
    }
    isNull = (columnType == ColumnType::TYPE_NULL);
    return E_OK;
}

/**
 * Check whether the result set is over
 */
bool StepResultSet::IsClosed() const
{
    return isClosed;
}
} // namespace NativeRdb
} // namespace OHOS
