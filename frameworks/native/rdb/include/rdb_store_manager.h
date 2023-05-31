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

#ifndef NATIVE_RDB_STORE_MANAGER_H
#define NATIVE_RDB_STORE_MANAGER_H

#include <memory>
#include <mutex>
#include <string>

#include "rdb_open_callback.h"
#include "rdb_store_config.h"
#include "rdb_store_impl.h"
#include "task_executor.h"

namespace OHOS {
namespace NativeRdb {
struct RdbStoreNode {
    RdbStoreNode(const std::shared_ptr<RdbStoreImpl> &rdbStore);
    RdbStoreNode &operator=(const std::shared_ptr<RdbStoreImpl> &store);

    std::shared_ptr<RdbStoreImpl> rdbStore_;
    TaskExecutor::TaskId taskId_;
};

class RdbStoreManager {
public:
    static RdbStoreManager &GetInstance();
    RdbStoreManager();
    virtual ~RdbStoreManager();
    std::shared_ptr<RdbStore> GetRdbStore(const RdbStoreConfig &config,
        int &errCode, int version, RdbOpenCallback &openCallback);
    void Remove(const std::string &path);
    void Clear();
    int SetSecurityLabel(const RdbStoreConfig &config);
    void SetReleaseTime(int ms);

private:
    int ProcessOpenCallback(RdbStore &rdbStore,
        const RdbStoreConfig &config, int version, RdbOpenCallback &openCallback);
    void RestartTimer(const std::string &path, RdbStoreNode &node);
    void AutoClose(const std::string &path);
    std::mutex mutex_;
    std::map<std::string, std::shared_ptr<RdbStoreNode>> storeCache_;

    std::shared_ptr<ExecutorPool> pool_;
    // ms_ : [10*1000 ~ 10*60*1000]
    int ms_;
};
} // namespace NativeRdb
} // namespace OHOS
#endif
