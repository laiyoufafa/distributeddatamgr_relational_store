/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "relational_predicates.h"

#include "relational_predicates_impl.h"
#include "relational_error_code.h"

OHOS::NativeRdb::RdbPredicates OHOS::NativeRdb::PredicateImpl::GetPredicates()
{
    return predicates_;
}

RDB_Predicates *RDB_CreatePredicates(char const *table)
{
    return new OHOS::NativeRdb::PredicateImpl(table);
}

int RDB_DestroyPredicates(RDB_Predicates *predicate)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    delete tempPredicates;
    return E_OK;
}

int PREDICATES_EqualTo(RDB_Predicates *predicate, char const *field, char const *value)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().EqualTo(field, value);
    return E_OK;
}

int PREDICATES_NotEqualTo(RDB_Predicates *predicate, char const *field, char const *value)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().NotEqualTo(field, value);
    return E_OK;
}

int PREDICATES_BeginWrap(RDB_Predicates *predicate)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().BeginWrap();
    return E_OK;
}

int PREDICATES_EndWrap(RDB_Predicates *predicate)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().EndWrap();
    return E_OK;
}

int PREDICATES_Or(RDB_Predicates *predicate)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().Or();
    return E_OK;
}

int PREDICATES_And(RDB_Predicates *predicate)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().And();
    return E_OK;
}

int PREDICATES_IsNull(RDB_Predicates *predicate, const char *field)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().IsNull(field);
    return E_OK;
}

int PREDICATES_IsNotNull(RDB_Predicates *predicate, const char *field)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().IsNotNull(field);
    return E_OK;
}

int PREDICATES_Like(RDB_Predicates *predicate, const char *field, const char *value)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().Like(field, value);
    return E_OK;
}

int PREDICATES_Between(RDB_Predicates *predicate, const char *field, const char *betweenValue, const char *andValue)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().Between(field, betweenValue, andValue);
    return E_OK;
}

int PREDICATES_NotBetween(RDB_Predicates *predicate, const char *field, const char *betweenValue, const char *andValue)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().NotBetween(field, betweenValue, andValue);
    return E_OK;
}

int PREDICATES_GreaterThan(RDB_Predicates *predicate, const char *field, const char *value)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().GreaterThan(field, value);
    return E_OK;
}

int PREDICATES_LessThan(RDB_Predicates *predicate, const char *field, const char *value)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().LessThan(field, value);
    return E_OK;
}

int PREDICATES_GreaterThanOrEqualTo(RDB_Predicates *predicate, const char *field, const char *value)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().GreaterThanOrEqualTo(field, value);
    return E_OK;
}
int PREDICATES_LessThanOrEqualTo(RDB_Predicates *predicate, const char *field, const char *value)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().LessThanOrEqualTo(field, value);
    return E_OK;
}

int PREDICATES_OrderBy(RDB_Predicates *predicate, const char *field, OrderByType type)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    if (type == OrderByType::DESC) {
        tempPredicates->GetPredicates().OrderByDesc(field);
        return E_OK;
    }
    tempPredicates->GetPredicates().OrderByAsc(field);
    return E_OK;
}

int PREDICATES_Distinct(RDB_Predicates *predicate)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().Distinct();
    return E_OK;
}

int PREDICATES_Limit(RDB_Predicates *predicate, unsigned int value)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().Limit(value);
    return E_OK;
}

int PREDICATES_Offset(RDB_Predicates *predicate, unsigned int rowOffset)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    tempPredicates->GetPredicates().Offset(rowOffset);
    return E_OK;
}

int PREDICATES_GroupBy(RDB_Predicates *predicate, char const *const *field, const int length)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    std::vector<std::string> vec;
    if (field != nullptr) {
        for (int i = 0; i < length; i++) {
            std::string str;
            str.assign(*(field + i), length);
            vec.push_back(std::move(str));
        }
    }

    tempPredicates->GetPredicates().GroupBy(vec);
    return E_OK;
}

int PREDICATES_In(RDB_Predicates *predicate, char const *filed, char const *const *values, const int length)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    std::vector<std::string> vec;
    if (values != nullptr) {
        for (int i = 0; i < length; i++) {
            std::string str;
            str.assign(*(values + i), length);
            vec.push_back(std::move(str));
        }
    }

    tempPredicates->GetPredicates().In(filed, vec);
    return E_OK;
}

int PREDICATES_NotIn(RDB_Predicates *predicate, char const *filed, char const *const *values, const int length)
{
    if (predicate == nullptr || predicate->id != OHOS::NativeRdb::RDB_PREDICATES_CID) {
        return E_INVALID_ARG;
    }
    OHOS::NativeRdb::PredicateImpl *tempPredicates = static_cast<OHOS::NativeRdb::PredicateImpl *>(predicate);
    std::vector<std::string> vec;
    if (values != nullptr) {
        for (int i = 0; i < length; i++) {
            std::string str;
            str.assign(*(values + i), length);
            vec.push_back(std::move(str));
        }
    }

    tempPredicates->GetPredicates().NotIn(filed, vec);
    return E_OK;
}