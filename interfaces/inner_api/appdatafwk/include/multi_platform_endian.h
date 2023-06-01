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

#ifndef MULTI_PLATFORM_EDIAN
#define MULTI_PLATFORM_EDIAN

#include <cstdint>
#include "rdb_visibility.h"
namespace OHOS {
namespace AppDataFwk {
class API_EXPORT Endian final {
public:
    static uint16_t Le16toh(uint16_t little_endian_16bits);
    static uint16_t Htole16(uint16_t host_16bits);
};
} // namespace AppDataFwk
} // namespace OHOS

#endif // MULTI_PLATFORM_EDIAN
