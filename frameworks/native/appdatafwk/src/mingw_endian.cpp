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

#include "multi_platform_endian.h"
#include <arpa/inet.h>

namespace OHOS::AppDataFwk {
uint16_t Endian::Le16toh(uint16_t little_endian_16bits)
{
    return ntohl(little_endian_16bits);
}
uint16_t Endian::Htole16(uint16_t host_16bits)
{
    return htonl(host_16bits);
}
}
