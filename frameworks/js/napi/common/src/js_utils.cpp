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

#include "js_utils.h"

#include "js_logger.h"

namespace OHOS {
namespace AppDataMgrJsKit {
static const std::map<std::string, std::string> NAMESPACES = {
    { "ohos.cloudData", "ZGF0YS5jbG91ZERhdGE=" },
    { "ohos.distributedKVStore", "ZGF0YS5kaXN0cmlidXRlZGt2c3RvcmU=" },
};

napi_value JSUtils::GetNamedProperty(napi_env env, napi_value object, const char *name)
{
    napi_value jsItem = nullptr;
    napi_get_named_property(env, object, name, &jsItem);
    return jsItem;
}

std::string JSUtils::Convert2String(napi_env env, napi_value jsStr, bool useDefaultBufSize)
{
    size_t str_buffer_size = 0;
    napi_get_value_string_utf8(env, jsStr, nullptr, 0, &str_buffer_size);
    str_buffer_size = (useDefaultBufSize && (str_buffer_size > DEFAULT_BUF_SIZE))
                          ? (DEFAULT_BUF_SIZE + BUF_CACHE_MARGIN)
                          : (str_buffer_size + BUF_CACHE_MARGIN);
    auto buf = std::make_unique<char[]>(str_buffer_size);
    if (buf == nullptr) {
        LOG_ERROR("Convert2String new failed, buf is nullptr");
        return "";
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, jsStr, buf.get(), str_buffer_size, &len);
    buf[len] = 0;
    std::string value(buf.get());
    return value;
}

std::vector<std::string> JSUtils::Convert2StrVector(napi_env env, napi_value value)
{
    uint32_t arrLen = 0;
    napi_get_array_length(env, value, &arrLen);
    if (arrLen == 0) {
        return {};
    }
    std::vector<std::string> result;
    for (size_t i = 0; i < arrLen; ++i) {
        napi_value element;
        napi_get_element(env, value, i, &element);
        result.push_back(ConvertAny2String(env, element));
    }
    return result;
}

std::vector<uint8_t> JSUtils::Convert2U8Vector(napi_env env, napi_value input_array)
{
    bool isTypedArray = false;
    napi_is_typedarray(env, input_array, &isTypedArray);
    if (!isTypedArray) {
        return {};
    }

    napi_typedarray_type type;
    napi_value input_buffer = nullptr;
    size_t byte_offset = 0;
    size_t length = 0;
    void *data = nullptr;
    napi_get_typedarray_info(env, input_array, &type, &length, &data, &input_buffer, &byte_offset);
    if (type != napi_uint8_array || data == nullptr) {
        return {};
    }
    return std::vector<uint8_t>((uint8_t *)data, ((uint8_t *)data) + length);
}

std::string JSUtils::ConvertAny2String(napi_env env, napi_value jsValue)
{
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, jsValue, &valueType), "napi_typeof failed");
    if (valueType == napi_string) {
        return JSUtils::Convert2String(env, jsValue, false);
    } else if (valueType == napi_number) {
        double valueNumber;
        napi_get_value_double(env, jsValue, &valueNumber);
        return std::to_string(valueNumber);
    } else if (valueType == napi_boolean) {
        bool valueBool = false;
        napi_get_value_bool(env, jsValue, &valueBool);
        return std::to_string(valueBool);
    } else if (valueType == napi_null) {
        return "null";
    } else if (valueType == napi_object) {
        std::vector<uint8_t> bytes = JSUtils::Convert2U8Vector(env, jsValue);
        std::string ret(bytes.begin(), bytes.end());
        return ret;
    }

    return "invalid type";
}

int32_t JSUtils::Convert2ValueExt(napi_env env, napi_value jsValue, uint32_t &output)
{
    napi_valuetype type;
    napi_status status = napi_typeof(env, jsValue, &type);
    if (status != napi_ok || type != napi_number) {
        LOG_DEBUG("napi_typeof failed status = %{public}d type = %{public}d", status, type);
        return napi_invalid_arg;
    }

    status = napi_get_value_uint32(env, jsValue, &output);
    if (status != napi_ok) {
        LOG_DEBUG("Convert2Value napi_get_value_uint32 failed, status = %{public}d", status);
        return status;
    }
    return status;
}

int32_t JSUtils::Convert2ValueExt(napi_env env, napi_value jsValue, int32_t &output)
{
    napi_valuetype type;
    napi_status status = napi_typeof(env, jsValue, &type);
    if (status != napi_ok || type != napi_number) {
        LOG_DEBUG("napi_typeof failed status = %{public}d type = %{public}d", status, type);
        return napi_invalid_arg;
    }

    status = napi_get_value_int32(env, jsValue, &output);
    if (status != napi_ok) {
        LOG_DEBUG("Convert2Value napi_get_value_int32 failed, status = %{public}d", status);
        return status;
    }
    return status;
}

int32_t JSUtils::Convert2Value(napi_env env, napi_value jsValue, bool &output)
{
    napi_valuetype type;
    napi_status status = napi_typeof(env, jsValue, &type);
    if (status != napi_ok || type != napi_boolean) {
        LOG_DEBUG("napi_typeof failed status = %{public}d type = %{public}d", status, type);
        return napi_invalid_arg;
    }

    bool bValue = false;
    status = napi_get_value_bool(env, jsValue, &bValue);
    if (status != napi_ok) {
        LOG_ERROR("Convert2Value napi_get_value_bool failed, status = %{public}d", status);
        return status;
    }
    output = bValue;
    return status;
}

int32_t JSUtils::Convert2ValueExt(napi_env env, napi_value jsValue, int64_t &output)
{
    napi_valuetype type;
    napi_status status = napi_typeof(env, jsValue, &type);
    if (status != napi_ok || type != napi_number) {
        LOG_DEBUG("napi_typeof failed status = %{public}d type = %{public}d", status, type);
        return napi_invalid_arg;
    }

    status = napi_get_value_int64(env, jsValue, &output);
    if (status != napi_ok) {
        LOG_DEBUG("Convert2Value napi_get_value_int32 failed, status = %{public}d", status);
        return status;
    }
    return status;
}

int32_t JSUtils::Convert2Value(napi_env env, napi_value jsValue, double &output)
{
    napi_valuetype type;
    napi_status status = napi_typeof(env, jsValue, &type);
    if (status != napi_ok || type != napi_number) {
        LOG_DEBUG("napi_typeof failed status = %{public}d type = %{public}d", status, type);
        return napi_invalid_arg;
    }

    double number = 0.0;
    status = napi_get_value_double(env, jsValue, &number);
    if (status != napi_ok) {
        LOG_DEBUG("Convert2Value napi_get_value_double failed, status = %{public}d", status);
        return status;
    }
    output = number;
    return status;
}

int32_t JSUtils::Convert2Value(napi_env env, napi_value jsValue, int64_t &output)
{
    return napi_invalid_arg;
}

int32_t JSUtils::Convert2Value(napi_env env, napi_value jsValue, std::string &output)
{
    napi_valuetype type;
    napi_status status = napi_typeof(env, jsValue, &type);
    if (status != napi_ok || type != napi_string) {
        LOG_DEBUG("napi_typeof failed status = %{public}d type = %{public}d", status, type);
        return napi_invalid_arg;
    }

    size_t length = MAX_VALUE_LENGTH;
    napi_get_value_string_utf8(env, jsValue, nullptr, 0, &length);
    length = length < MAX_VALUE_LENGTH ? (MAX_VALUE_LENGTH - 1) : length;
    std::unique_ptr<char[]> str = std::make_unique<char[]>(length + 1);
    if (str == nullptr) {
        LOG_ERROR("Convert2Value new failed, str is nullptr");
        return napi_generic_failure;
    }

    size_t valueSize = 0;
    status = napi_get_value_string_utf8(env, jsValue, str.get(), length, &valueSize);
    if (status != napi_ok) {
        LOG_ERROR("Convert2Value napi_get_value_string_utf8 failed, status = %{public}d", status);
        return status;
    }
    str[valueSize] = 0;
    output = std::string(str.get());
    return status;
}

int32_t JSUtils::Convert2Value(napi_env env, napi_value jsValue, std::vector<uint8_t> &output)
{
    bool isTypedArray = false;
    napi_is_typedarray(env, jsValue, &isTypedArray);
    if (!isTypedArray) {
        return napi_invalid_arg;
    }

    napi_typedarray_type type;
    napi_value input_buffer = nullptr;
    size_t byte_offset = 0;
    size_t length = 0;
    void *tmp = nullptr;
    auto status = napi_get_typedarray_info(env, jsValue, &type, &length, &tmp, &input_buffer, &byte_offset);
    if (status != napi_ok || type != napi_uint8_array ) {
        return napi_invalid_arg;
    }
    output = (tmp != nullptr ? std::vector<uint8_t>((uint8_t*)tmp, ((uint8_t*)tmp) + length) : std::vector<uint8_t>());
    return status;
}

int32_t JSUtils::Convert2Value(napi_env env, napi_value jsValue, std::monostate &value)
{
    napi_value tempValue;
    napi_get_null(env, &tempValue);
    bool equal = false;
    napi_strict_equals(env, jsValue, tempValue, &equal);
    if (equal) {
        value = std::monostate();
        return napi_ok;
    }
    napi_get_undefined(env, &tempValue);
    napi_strict_equals(env, jsValue, tempValue, &equal);
    if (equal) {
        value = std::monostate();
        return napi_ok;
    }
    LOG_DEBUG("Convert2Value jsValue is not null");
    return napi_invalid_arg;
}

int32_t JSUtils::Convert2Value(napi_env env, napi_value jsValue, std::map<std::string, int32_t> &output)
{
    LOG_DEBUG("napi_value -> std::map<std::string, int32_t> ");
    output.clear();
    napi_value jsMapList = nullptr;
    uint32_t jsCount = 0;
    napi_status status = napi_get_property_names(env, jsValue, &jsMapList);
    status = napi_get_array_length(env, jsMapList, &jsCount);
    LOG_DEBUG("jsCOUNT: %{public}d", jsCount);
    NAPI_ASSERT_BASE(env, (status == napi_ok) && (jsCount > 0), "get_map failed!", ERR);
    napi_value jsKey = nullptr;
    napi_value jsVal = nullptr;
    for (uint32_t index = 0; index < jsCount; index++) {
        status = napi_get_element(env, jsMapList, index, &jsKey);
        NAPI_ASSERT_BASE(env, (jsKey != nullptr) && (status == napi_ok), "no element", ERR);
        std::string key;
        Convert2Value(env, jsKey, key);
        status = napi_get_property(env, jsValue, jsKey, &jsVal);
        NAPI_ASSERT_BASE(env, (jsVal != nullptr) && (status == napi_ok), "no element", ERR);
        int32_t val;
        Convert2ValueExt(env, jsVal, val);
        output.insert(std::pair<std::string, int32_t>(key, val));
    }
    return napi_ok;
}

int32_t JSUtils::Convert2Value(napi_env env, napi_value jsValue, std::map<std::string, bool> &output)
{
    LOG_DEBUG("napi_value -> std::map<std::string, bool> ");
    output.clear();
    napi_value jsMapList = nullptr;
    uint32_t jsCount = 0;
    napi_status status = napi_get_property_names(env, jsValue, &jsMapList);
    status = napi_get_array_length(env, jsMapList, &jsCount);
    LOG_DEBUG("jsCount: %{public}d", jsCount);
    NAPI_ASSERT_BASE(env, (status == napi_ok) && (jsCount > 0), "get_map failed!", ERR);
    napi_value jsKey = nullptr;
    napi_value jsVal = nullptr;
    for (uint32_t index = 0; index < jsCount; index++) {
        status = napi_get_element(env, jsMapList, index, &jsKey);
        NAPI_ASSERT_BASE(env, (jsKey != nullptr) && (status == napi_ok), "no element", ERR);
        std::string key;
        Convert2Value(env, jsKey, key);
        status = napi_get_property(env, jsValue, jsKey, &jsVal);
        NAPI_ASSERT_BASE(env, (jsVal != nullptr) && (status == napi_ok), "no element", ERR);
        bool val;
        Convert2Value(env, jsVal, val);
        output.insert(std::pair<std::string, bool>(key, val));
    }
    return napi_ok;
}

napi_value JSUtils::Convert2JSValue(napi_env env, const std::vector<std::string> &value)
{
    napi_value jsValue;
    napi_status status = napi_create_array_with_length(env, value.size(), &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }

    for (size_t i = 0; i < value.size(); ++i) {
        napi_set_element(env, jsValue, i, Convert2JSValue(env, value[i]));
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, const std::string &value)
{
    napi_value jsValue;
    if (napi_create_string_utf8(env, value.c_str(), value.size(), &jsValue) != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, const std::vector<uint8_t> &value)
{
    napi_value jsValue;
    void *native = nullptr;
    napi_value buffer = nullptr;
    napi_status status = napi_create_arraybuffer(env, value.size(), &native, &buffer);
    if (status != napi_ok) {
        return nullptr;
    }
    for (size_t i = 0; i < value.size(); i++) {
        *(static_cast<uint8_t *>(native) + i) = value[i];
    }
    status = napi_create_typedarray(env, napi_uint8_array, value.size(), buffer, 0, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, int32_t value)
{
    napi_value jsValue;
    napi_status status = napi_create_int32(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, uint32_t value)
{
    napi_value jsValue;
    napi_status status = napi_create_uint32(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, int64_t value)
{
    napi_value jsValue;
    napi_status status = napi_create_int64(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, double value)
{
    napi_value jsValue;
    napi_status status = napi_create_double(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, bool value)
{
    napi_value jsValue;
    napi_status status = napi_get_boolean(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, const std::map<std::string, int> &value)
{
    napi_value jsValue;
    napi_status status = napi_create_array_with_length(env, value.size(), &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }

    int index = 0;
    for (const auto &[device, result] : value) {
        napi_value jsElement;
        status = napi_create_array_with_length(env, SYNC_RESULT_ELEMNT_NUM, &jsElement);
        if (status != napi_ok) {
            return nullptr;
        }
        napi_set_element(env, jsElement, 0, Convert2JSValue(env, device));
        napi_set_element(env, jsElement, 1, Convert2JSValue(env, result));
        napi_set_element(env, jsValue, index++, jsElement);
    }

    return jsValue;
}

int32_t JSUtils::Convert2JSValue(napi_env env, std::string value, napi_value &output)
{
    std::string tempStr = std::string(value);
    if (napi_create_string_utf8(env, tempStr.c_str(), tempStr.size(), &output) != napi_ok) {
        LOG_ERROR("Convert2JSValue create JS string failed");
        return ERR;
    }
    return OK;
}

int32_t JSUtils::Convert2JSValue(napi_env env, bool value, napi_value &output)
{
    if (napi_get_boolean(env, value, &output) != napi_ok) {
        LOG_ERROR("Convert2JSValue create JS bool failed");
        return ERR;
    }
    return OK;
}

int32_t JSUtils::Convert2JSValue(napi_env env, double value, napi_value &output)
{
    if (napi_create_double(env, value, &output) != napi_ok) {
        LOG_ERROR("Convert2JSValue create JS double failed");
        return ERR;
    }
    return OK;
}

napi_value JSUtils::Convert2JSValue(napi_env env, const std::monostate &value)
{
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

napi_value JSUtils::DefineClass(napi_env env, const std::string &name, const Descriptor &descriptor, napi_callback ctor)
{
    // base64("data.cloudData") as rootPropName, i.e. global.<root>
    const std::string rootPropName = "ZGF0YS5jbG91ZERhdGE=";
    napi_value root = nullptr;
    bool hasRoot = false;
    napi_value global = nullptr;
    napi_get_global(env, &global);
    napi_has_named_property(env, global, rootPropName.c_str(), &hasRoot);
    if (hasRoot) {
        napi_get_named_property(env, global, rootPropName.c_str(), &root);
    } else {
        napi_create_object(env, &root);
        napi_set_named_property(env, global, rootPropName.c_str(), root);
    }

    std::string propName = "constructor_of_" + name;
    napi_value constructor = nullptr;
    bool hasProp = false;
    napi_has_named_property(env, root, propName.c_str(), &hasProp);
    if (hasProp) {
        napi_get_named_property(env, root, propName.c_str(), &constructor);
        if (constructor != nullptr) {
            LOG_DEBUG("got data.cloudData.%{public}s as constructor", propName.c_str());
            return constructor;
        }
        hasProp = false; // no constructor.
    }
    auto properties = descriptor();
    NAPI_CALL(env, napi_define_class(env, name.c_str(), name.size(), ctor, nullptr, properties.size(),
                       properties.data(), &constructor));
    NAPI_ASSERT(env, constructor != nullptr, "napi_define_class failed!");

    if (!hasProp) {
        napi_set_named_property(env, root, propName.c_str(), constructor);
        LOG_DEBUG("save constructor to data.cloudData.%{public}s", propName.c_str());
    }
    return constructor;
}

napi_value JSUtils::GetClass(napi_env env, const std::string &spaceName, const std::string &className)
{
    auto it = NAMESPACES.find(spaceName);
    if (it == NAMESPACES.end()) {
        return nullptr;
    }

    napi_value root = nullptr;
    napi_value global = nullptr;
    napi_get_global(env, &global);
    napi_get_named_property(env, global, it->second.c_str(), &root);
    std::string propName = "constructor_of_" + className;
    napi_value constructor = nullptr;
    bool hasProp = false;
    napi_has_named_property(env, root, propName.c_str(), &hasProp);
    if (!hasProp) {
        return nullptr;
    }
    napi_get_named_property(env, root, propName.c_str(), &constructor);
    if (constructor != nullptr) {
        LOG_DEBUG("got data.cloudData.%{public}s as constructor", propName.c_str());
        return constructor;
    }
    hasProp = false; // no constructor.
    return constructor;
}
} // namespace AppDataMgrJsKit
} // namespace OHOS
