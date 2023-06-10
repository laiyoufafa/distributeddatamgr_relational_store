/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTED_RDB_RDB_TYPES_H
#define DISTRIBUTED_RDB_RDB_TYPES_H

#include <securec.h>

#include <cinttypes>
#include <functional>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace OHOS::DistributedRdb {
enum RdbStatus {
    RDB_OK,
    RDB_ERROR,
};

enum RdbDistributedType {
    RDB_DEVICE_COLLABORATION = 10,
    RDB_DISTRIBUTED_TYPE_MAX
};

struct RdbSyncerParam {
    std::string bundleName_;
    std::string hapName_;
    std::string storeName_;
    int32_t area_ = 0;
    int32_t level_ = 0;
    int32_t type_ = RDB_DEVICE_COLLABORATION;
    bool isAutoSync_ = false;
    bool isEncrypt_ = false;
    std::vector<uint8_t> password_;
    ~RdbSyncerParam()
    {
        password_.assign(password_.size(), 0);
    };
};

enum SyncMode {
    PUSH,
    PULL,
    PULL_PUSH,
    TIME_FIRST = 4,
    NATIVE_FIRST,
    CLOUD_FIRST,
};

struct SyncOption {
    SyncMode mode;
    bool isBlock;
};

enum DistributedTableType {
    DISTRIBUTED_DEVICE = 0,
    DISTRIBUTED_CLOUD
};

enum Progress {
    SYNC_BEGIN,
    SYNC_IN_PROGRESS,
    SYNC_FINISH,
};

struct Statistic {
    int32_t total;
    int32_t success;
    int32_t failed;
    int32_t untreated;
};

struct TableDetail {
    Statistic upload;
    Statistic download;
};

using TableDetails = std::map<std::string, TableDetail>;

struct ProgressDetail {
    int32_t progress;
    int32_t code;
    TableDetails details;
};

using Briefs = std::map<std::string, int>;
using Details = std::map<std::string, ProgressDetail>;
using AsyncBrief = std::function<void(const Briefs&)>;
using AsyncDetail = std::function<void(Details &&)>;

using SyncResult = Briefs;
using SyncCallback = AsyncBrief;

enum RdbPredicateOperator {
    EQUAL_TO,
    NOT_EQUAL_TO,
    AND,
    OR,
    ORDER_BY,
    LIMIT,
    OPERATOR_MAX
};

struct RdbPredicateOperation {
    RdbPredicateOperator operator_;
    std::string field_;
    std::vector<std::string> values_;
};

struct PredicatesMemo {
    inline void AddOperation(const RdbPredicateOperator op, const std::string& field,
                             const std::string& value)
    {
        operations_.push_back({ op, field, { value } });
    }
    inline void AddOperation(const RdbPredicateOperator op, const std::string& field,
                             const std::vector<std::string>& values)
    {
        operations_.push_back({ op, field, values });
    }

    std::vector<std::string> tables_;
    std::vector<std::string> devices_;
    std::vector<RdbPredicateOperation> operations_;
};

enum SubscribeMode {
    REMOTE,
    CLOUD,
    CLOUD_DETAIL,
    SUBSCRIBE_MODE_MAX
};

struct SubscribeOption {
    SubscribeMode mode;
};

struct Origin {
    enum OriginType : int32_t {
        ORIGIN_LOCAL,
        ORIGIN_NEARBY,
        ORIGIN_CLOUD,
        ORIGIN_ALL,
        ORIGIN_BUTT,
    };
    enum DataType : int32_t {
        BASIC_DATA,
        ASSET_DATA,
        TYPE_BUTT,
    };
    int32_t origin = ORIGIN_ALL;
    int32_t dataType = BASIC_DATA;
    // origin is ORIGIN_LOCAL, the id is empty
    // origin is ORIGIN_NEARBY, the id is networkId;
    // origin is ORIGIN_CLOUD, the id is the cloud account id
    std::vector<std::string> id;
    std::string store;
};

class RdbStoreObserver {
public:
    enum ChangeType : int32_t {
        CHG_TYPE_INSERT = 0,
        CHG_TYPE_UPDATE,
        CHG_TYPE_DELETE,
        CHG_TYPE_BUTT
    };
    using PrimaryKey = std::variant<std::monostate, std::string, int64_t>;
    using ChangeInfo = std::map<std::string, std::vector<PrimaryKey>[CHG_TYPE_BUTT]>;
    using PrimaryFields = std::map<std::string, std::string>;
    virtual void OnChange(const std::vector<std::string> &devices) = 0; // networkid
    virtual void OnChange(const Origin &origin, const PrimaryFields &fields, ChangeInfo &&changeInfo)
    {
        OnChange(origin.id);
    };
};

struct DropOption {
};
} // namespace OHOS::DistributedRdb
#endif
