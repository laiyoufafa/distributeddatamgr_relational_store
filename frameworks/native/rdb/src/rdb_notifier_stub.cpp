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

#include "rdb_notifier_stub.h"

#include <ipc_skeleton.h>

#include "itypes_util.h"
#include "logger.h"

namespace OHOS::DistributedRdb {
using namespace OHOS::Rdb;

RdbNotifierStub::RdbNotifierStub(const SyncCompleteHandler &completeNotifier, const DataChangeHandler &changeNotifier)
    : IRemoteStub<RdbNotifierStubBroker>(), completeNotifier_(completeNotifier), changeNotifier_(changeNotifier)
{
    LOG_INFO("construct");
}

RdbNotifierStub::~RdbNotifierStub() noexcept
{
    LOG_INFO("destroy");
}

bool RdbNotifierStub::CheckInterfaceToken(MessageParcel& data)
{
    auto localDescriptor = GetDescriptor();
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (remoteDescriptor != localDescriptor) {
        LOG_ERROR("interface token is not equal");
        return false;
    }
    return true;
}

int RdbNotifierStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                     MessageOption &option)
{
    LOG_DEBUG("code:%{public}u, callingPid:%{public}d", code, IPCSkeleton::GetCallingPid());
    if (!CheckInterfaceToken(data)) {
        return RDB_ERROR;
    }

    if (code >= 0 && code < RDB_NOTIFIER_CMD_MAX) {
        return (this->*HANDLES[code])(data, reply);
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t RdbNotifierStub::OnCompleteInner(MessageParcel &data, MessageParcel &reply)
{
    uint32_t seqNum = 0;
    Details result;
    if (!ITypesUtil::Unmarshal(data, seqNum, result)) {
        LOG_ERROR("read sync result failed");
        return RDB_ERROR;
    }
    return OnComplete(seqNum, std::move(result));
}

int32_t RdbNotifierStub::OnComplete(uint32_t seqNum, Details &&result)
{
    if (completeNotifier_) {
        completeNotifier_(seqNum, std::move(result));
    }
    return RDB_OK;
}


int32_t RdbNotifierStub::OnChange(const Origin &origin, const PrimaryFields &primaries, ChangeInfo &&changeInfo)
{
    if (changeNotifier_ != nullptr) {
        changeNotifier_(origin, primaries, std::move(changeInfo));
    }
    return RDB_OK;
}

int32_t RdbNotifierStub::OnChangeInner(MessageParcel &data, MessageParcel &reply)
{
    Origin origin;
    PrimaryFields primaries;
    ChangeInfo changeInfo;
    if (!ITypesUtil::Unmarshal(data, origin, primaries, changeInfo)) {
        LOG_ERROR("read sync result failed");
        return RDB_ERROR;
    }
    return OnChange(origin, primaries, std::move(changeInfo));
}
} // namespace OHOS::DistributedRdb