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

#include "distributed_objectstore.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include <js_object_wrapper.h>
#include <js_distributedobject.h>
#include <js_distributedobjectstore.h>
#include <logger.h>
#include <js_common.h>
#include <objectstore_errors.h>


namespace OHOS::ObjectStore {
constexpr size_t TYPE_SIZE = 10;
napi_value JSDistributedObjectStore::NewDistributedObject(napi_env env, DistributedObjectStore *objectStore, DistributedObject *object)
{
    napi_value result;
    napi_status status = napi_new_instance(env, JSDistributedObject::GetCons(env), 0, nullptr, &result);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    JSObjectWrapper *objectWrapper = new JSObjectWrapper(objectStore, object);
    status = napi_wrap( env, result, objectWrapper,
                        [](napi_env env, void* data, void* hint) {
                            auto objectWrapper = (JSObjectWrapper*)data;
                            if (objectWrapper != nullptr) {
                                delete objectWrapper;
                            }
                        },
                        nullptr, nullptr);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    return result;
}

//function createObjectSync(sessionId: string): DistributedObject;
napi_value JSDistributedObjectStore::JSCreateObjectSync(napi_env env, napi_callback_info info)
{
    LOG_INFO("start JSCreateObjectSync");
    size_t requireArgc = 1;
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    void* data = nullptr;
    char sessionId[SESSION_ID_SIZE] = { 0 };
    size_t sessionIdLen = 0;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    ASSERT_MATCH_ELSE_RETURN_NULL(argc >= requireArgc);
    for (size_t i = 0; i < argc; i++) {
        napi_valuetype valueType = napi_undefined;
        status = napi_typeof(env, argv[i], &valueType);
        CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);

        if (i == 0 && valueType == napi_string) {
            status = napi_get_value_string_utf8(env, argv[i], sessionId, SESSION_ID_SIZE, &sessionIdLen);
            CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
        } else {
            LOG_ERROR("type dismatch");
            return nullptr;
        }
    }
    DistributedObjectStore* objectInfo = DistributedObjectStore::GetInstance();
    ASSERT_MATCH_ELSE_RETURN_NULL(objectInfo != nullptr);
    DistributedObject *object = objectInfo->CreateObject(sessionId);
    ASSERT_MATCH_ELSE_RETURN_NULL(object != nullptr);
    return NewDistributedObject(env, objectInfo, object);
}

//function destroyObjectSync(sessionId: string): number;
napi_value JSDistributedObjectStore::JSDestroyObjectSync(napi_env env, napi_callback_info info)
{
    LOG_INFO("start");
    size_t requireArgc = 1;
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void* data = nullptr;
    char sessionId[SESSION_ID_SIZE] = { 0 };
    size_t sessionIdLen = 0;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    ASSERT_MATCH_ELSE_RETURN_NULL(argc >= requireArgc);
    for (size_t i = 0; i < argc; i++) {
        napi_valuetype valueType = napi_undefined;
        status = napi_typeof(env, argv[i], &valueType);
        CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);

        if (i == 0 && valueType == napi_string) {
            status = napi_get_value_string_utf8(env, argv[i], sessionId, SESSION_ID_SIZE, &sessionIdLen);
            CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
        } else {
            NAPI_ASSERT(env, false, "type mismatch");
        }
    }

    DistributedObjectStore* objectInfo = DistributedObjectStore::GetInstance();
    ASSERT_MATCH_ELSE_RETURN_NULL(objectInfo != nullptr);
    uint32_t ret = objectInfo->DeleteObject(sessionId);
    napi_value result = nullptr;
    napi_create_int32(env, ret, &result);
    return result;
}

//function sync(object_: DistributedObject): number;
napi_value JSDistributedObjectStore::JSSync(napi_env env, napi_callback_info info)
{
    LOG_INFO("start");
    size_t requireArgc = 1;
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void* data = nullptr;
    JSObjectWrapper *objectWrapper = nullptr;;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    ASSERT_MATCH_ELSE_RETURN_NULL(argc >= requireArgc);
    status = napi_unwrap(env, argv[0], (void**)&objectWrapper);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    ASSERT_MATCH_ELSE_RETURN_NULL(objectWrapper != nullptr);
    DistributedObjectStore *objectInfo = DistributedObjectStore::GetInstance();
    ASSERT_MATCH_ELSE_RETURN_NULL(objectInfo != nullptr);
    uint32_t ret = objectInfo->Sync(objectWrapper->GetObject());
    napi_value result = nullptr;
    napi_create_int32(env, ret, &result);
    return result;
}

// function on(type: 'change', object: DistributedObject, callback: Callback<ChangedDataObserver>): void;
// function on(type: 'status', object: DistributedObject, callback: Callback<ObjectStatusObserver>): void;
napi_value JSDistributedObjectStore::JSOn(napi_env env, napi_callback_info info)
{
    LOG_INFO("start");
    size_t requireArgc = 3;
    size_t argc = 3;
    napi_value argv[3] = { 0 };
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    ASSERT_MATCH_ELSE_RETURN_NULL(argc >= requireArgc);

    char type[TYPE_SIZE] = { 0 };
    size_t eventTypeLen = 0;
    napi_valuetype eventValueType = napi_undefined;
    status = napi_typeof(env, argv[0], &eventValueType);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    ASSERT_MATCH_ELSE_RETURN_NULL(eventValueType == napi_string);
    status = napi_get_value_string_utf8(env, argv[0], type, TYPE_SIZE, &eventTypeLen);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);

    napi_valuetype objectType = napi_undefined;
    status = napi_typeof(env, argv[1], &objectType);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    ASSERT_MATCH_ELSE_RETURN_NULL(objectType == napi_object);

    napi_valuetype callbackType = napi_undefined;
    status = napi_typeof(env, argv[2], &callbackType);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    ASSERT_MATCH_ELSE_RETURN_NULL(callbackType == napi_function);

    JSObjectWrapper *wrapper = nullptr;
    status = napi_unwrap(env, argv[1], (void**)&wrapper);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    ASSERT_MATCH_ELSE_RETURN_NULL(wrapper != nullptr);
    wrapper->AddWatch(env, type, argv[2]);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}
// function off(type: 'change', object: DistributedObject, callback?: Callback<ChangedDataObserver>): void;
// function off(type: 'status', object: DistributedObject, callback?: Callback<ObjectStatusObserver>): void;
napi_value JSDistributedObjectStore::JSOff(napi_env env, napi_callback_info info)
{
    LOG_INFO("start");
    size_t requireArgc = 2;
    size_t argc = 3;
    napi_value argv[3] = { 0 };
    napi_value thisVar = nullptr;
    void* data = nullptr;
    char type[TYPE_SIZE] = { 0 };
    size_t typeLen = 0;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    ASSERT_MATCH_ELSE_RETURN_NULL(argc >= requireArgc);
    for (size_t i = 0; i < argc; i++) {
        napi_valuetype valueType = napi_undefined;
        status = napi_typeof(env, argv[i], &valueType);
        CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);

        if (i == 0 && valueType == napi_string) {
            status = napi_get_value_string_utf8(env, argv[i], type, TYPE_SIZE, &typeLen);
            CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
        } else if (i == 1 && valueType == napi_object) {
            continue;
        } else if (i == 2 && (valueType == napi_function || valueType == napi_undefined)) {
            continue;
        } else {
            NAPI_ASSERT(env, false, "type mismatch");
        }
    }
    JSObjectWrapper *wrapper = nullptr;
    status = napi_unwrap(env, argv[1], (void**)&wrapper);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    NAPI_ASSERT(env, wrapper != nullptr, "object wrapper is null");
    if (argc == requireArgc) {
        wrapper->DeleteWatch(env, type);
    } else {
        wrapper->DeleteWatch(env, type, argv[2]);
    }
    napi_value result = nullptr;
    status = napi_get_undefined(env, &result);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    return result;
}
}

