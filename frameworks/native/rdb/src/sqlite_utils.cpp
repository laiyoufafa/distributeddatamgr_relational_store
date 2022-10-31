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

#include "sqlite_utils.h"

#include <algorithm>
#include <cstdio>

#include "logger.h"
#include "rdb_errno.h"

#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
#include "rdb_store_config.h"
#include "security_label.h"
#include <unistd.h>
#endif

namespace OHOS {
namespace NativeRdb {
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
using SecurityLabel = OHOS::DistributedFS::ModuleSecurityLabel::SecurityLabel;
const std::string SqliteUtils::DATA_LEVEL[] = { "", "s0", "s1", "s2", "s3", "s4" };
#endif
const int SqliteUtils::STATEMENT_SELECT = 1;
const int SqliteUtils::STATEMENT_UPDATE = 2;
const int SqliteUtils::STATEMENT_ATTACH = 3;
const int SqliteUtils::STATEMENT_DETACH = 4;
const int SqliteUtils::STATEMENT_BEGIN = 5;
const int SqliteUtils::STATEMENT_COMMIT = 6;
const int SqliteUtils::STATEMENT_ROLLBACK = 7;
const int SqliteUtils::STATEMENT_PRAGMA = 8;
const int SqliteUtils::STATEMENT_DDL = 9;
const int SqliteUtils::STATEMENT_OTHER = 99;

const std::map<std::string, int> SqliteUtils::SQL_TYPE_MAP = {
    { "SEL", SqliteUtils::STATEMENT_SELECT },
    { "INS", SqliteUtils::STATEMENT_UPDATE },
    { "UPD", SqliteUtils::STATEMENT_UPDATE },
    { "REP", SqliteUtils::STATEMENT_UPDATE },
    { "DEL", SqliteUtils::STATEMENT_UPDATE },
    { "ATT", SqliteUtils::STATEMENT_ATTACH },
    { "DET", SqliteUtils::STATEMENT_DETACH },
    { "COM", SqliteUtils::STATEMENT_COMMIT },
    { "END", SqliteUtils::STATEMENT_COMMIT },
    { "ROL", SqliteUtils::STATEMENT_ROLLBACK },
    { "BEG", SqliteUtils::STATEMENT_BEGIN },
    { "PRA", SqliteUtils::STATEMENT_PRAGMA },
    { "CRE", SqliteUtils::STATEMENT_DDL },
    { "DRO", SqliteUtils::STATEMENT_DDL },
    { "ALT", SqliteUtils::STATEMENT_DDL },
};

int SqliteUtils::GetSqlStatementType(const std::string &sql)
{
    std::string sqlStr = sql;
    /* the sql string length less than 3 can not be any type sql */
    if (sqlStr.length() < 3) {
        return STATEMENT_OTHER;
    }
    /* analyze the sql type through first 3 character */
    std::string prefixSql = StrToUpper(sqlStr.substr(0, 3));
    auto iter = SQL_TYPE_MAP.find(prefixSql);
    if (iter != SQL_TYPE_MAP.end()) {
        return iter->second;
    }

    return STATEMENT_OTHER;
}

std::string SqliteUtils::StrToUpper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
    return s;
}

bool SqliteUtils::IsSqlReadOnly(int sqlType)
{
    return (sqlType == STATEMENT_SELECT) ? true : false;
}

bool SqliteUtils::IsSpecial(int sqlType)
{
    if (sqlType == STATEMENT_BEGIN || sqlType == STATEMENT_COMMIT || sqlType == STATEMENT_ROLLBACK) {
        return true;
    }
    return false;
}

const std::string SqliteUtils::ON_CONFLICT_CLAUSE[] = { "", " OR ROLLBACK", " OR ABORT", " OR FAIL", " OR IGNORE",
    " OR REPLACE" };
int SqliteUtils::GetConflictClause(int conflictResolution, std::string &conflictClause)
{
    if (conflictResolution < 0 || conflictResolution >= CONFLICT_CLAUSE_COUNT) {
        return E_INVALID_CONFLICT_FLAG;
    }
    conflictClause = ON_CONFLICT_CLAUSE[conflictResolution];
    return E_OK;
}

bool SqliteUtils::DeleteFile(const std::string filePath)
{
    return remove(filePath.c_str()) == 0;
}

int SqliteUtils::RenameFile(const std::string srcFile, const std::string destFile)
{
    return rename(srcFile.c_str(), destFile.c_str());
}

#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
int SqliteUtils::SetFileSecurityLevel(const std::string &filePath, const int32_t &securityLevel)
{
    if (access(filePath.c_str(), F_OK) != 0) {
        LOG_ERROR("The database file does not exist.");
        return E_ERROR;
    }

    bool result = SecurityLabel::SetSecurityLabel(filePath, DATA_LEVEL[securityLevel]);
    if (result) {
        LOG_INFO("Set database securityLabel %{public}s successful.", DATA_LEVEL[securityLevel].c_str());
        return E_OK;
    }
    return E_ERROR;
}

std::string SqliteUtils::GetFileSecurityLevel(const std::string &filePath)
{
    return SecurityLabel::GetSecurityLabel(filePath);
}
#endif
} // namespace NativeRdb
} // namespace OHOS
