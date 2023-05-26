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

#ifndef RELATIONAL_STORE_IMPL_H
#define RELATIONAL_STORE_IMPL_H

#include <memory>

#include "rdb_store.h"
#include "relational_predicates.h"
#include "relational_store.h"

namespace OHOS {
namespace RdbNdk {
constexpr int RDB_STORE_CID = 1234560; // The class id used to uniquely identify the OH_Rdb_Store class.
class StoreImpl : public OH_Rdb_Store {
public:
    explicit StoreImpl(std::shared_ptr<OHOS::NativeRdb::RdbStore> store);
    std::shared_ptr<OHOS::NativeRdb::RdbStore> GetStore();

private:
    std::shared_ptr<OHOS::NativeRdb::RdbStore> store_;
};
} // namespace RdbNdk
} // namespace OHOS
#endif // RELATIONAL_STORE_IMPL_H
