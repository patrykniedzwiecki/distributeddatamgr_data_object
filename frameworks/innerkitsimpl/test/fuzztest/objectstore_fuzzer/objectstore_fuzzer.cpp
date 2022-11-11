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

#include "objectstore_fuzzer.h"

#include <string>
#include <vector>
#include "distributed_object.h"
#include "distributed_objectstore.h"
#include "objectstore_errors.h"

using namespace OHOS::ObjectStore;
namespace OHOS {
static DistributedObject *object_ = nullptr;
static DistributedObjectStore *objectStore_ = nullptr;
constexpr const char *SESSIONID = "123456";
constexpr const char *SESSIONIDv9 = "789456";
uint32_t SetUpTestCase()
{
    std::string bundleName = "com.example.myapplication";
    DistributedObjectStore *objectStore = nullptr;
    DistributedObject *object = nullptr;
    objectStore = DistributedObjectStore::GetInstance(bundleName);
    if (objectStore != nullptr) {
        objectStore_ = objectStore;
        object = objectStore_->CreateObject(SESSIONID);
        if (object != nullptr) {
            object_ = object;
            return SUCCESS;
        } else {
            return ERR_EXIST;
        }
    } else {
        return ERR_EXIST;
    }
}

bool PutDoubleFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    double sval = static_cast<double>(size);
    std::string skey(data, data + size);
    uint32_t ret = object_->PutDouble(skey, sval);
    if (!ret) {
        result = true;
    }
    objectStore_->DeleteObject(SESSIONID);
    return result;
}

bool PutBooleanFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey(data, data + size);
    uint32_t ret = object_->PutBoolean(skey, true);
    if (!ret) {
        result = true;
    }
    ret = object_->PutBoolean(skey, false);
    if (ret != SUCCESS) {
        result = false;
    }
    objectStore_->DeleteObject(SESSIONID);
    return result;
}

bool PutStringFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey(data, data + size);
    std::string sval(data, data + size);
    uint32_t ret = object_->PutString(skey, sval);
    if (!ret) {
        result = true;
    }
    objectStore_->DeleteObject(SESSIONID);
    return result;
}

bool PutComplexFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    size_t sum = 10;
    std::string skey(data, data + size);
    std::vector<uint8_t> value;
    for (size_t i = 0; i < sum; i++) {
        value.push_back(*data + i);
    }
    uint32_t ret = object_->PutComplex(skey, value);
    if (!ret) {
        result = true;
    }
    objectStore_->DeleteObject(SESSIONID);
    return result;
}

bool GetDoubleFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    double sval = static_cast<double>(size);
    double val;
    std::string skey(data, data + size);
    if (SUCCESS == object_->PutDouble(skey, sval)) {
        uint32_t ret = object_->GetDouble(skey, val);
        if (!ret) {
            result = true;
        }
    }
    objectStore_->DeleteObject(SESSIONID);
    return result;
}

bool GetBooleanFuzz(const uint8_t *data, size_t size)
{
    bool val, result = false;
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey(data, data + size);
    if (SUCCESS == object_->PutBoolean(skey, true)) {
        uint32_t ret = object_->GetBoolean(skey, val);
        if (!ret) {
            result = true;
        }
    }
    objectStore_->DeleteObject(SESSIONID);
    return result;
}

bool GetStringFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey(data, data + size);
    std::string sval(data, data + size);
    std::string val;
    if (SUCCESS == object_->PutString(skey, sval)) {
        uint32_t ret = object_->GetString(skey, val);
        if (!ret) {
            result = true;
        }
    }
    objectStore_->DeleteObject(SESSIONID);
    return result;
}

bool GetComplexFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    size_t sum = 10;
    std::string skey(data, data + size);
    std::vector<uint8_t> svalue;
    std::vector<uint8_t> val;
    for (size_t i = 0; i < sum; i++) {
        svalue.push_back(*data + i);
    }
    if (SUCCESS == object_->PutComplex(skey, svalue)) {
        uint32_t ret = object_->GetComplex(skey, val);
        if (!ret) {
            result = true;
        }
    }
    objectStore_->DeleteObject(SESSIONID);
    return result;
}


bool GetTypeFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey(data, data + size);
    Type val;
    uint32_t ret = object_->GetType(skey, val);
    if (!ret) {
        result = true;
    }
    objectStore_->DeleteObject(SESSIONID);
    return result;
}

bool SaveFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey(data, data + size);
    uint32_t ret = object_->Save(skey);
    if (!ret) {
        result = true;
    }
    objectStore_->DeleteObject(SESSIONID);
    return result;
}

bool SaveAndRevokeSaveFuzz(const uint8_t *data, size_t size)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey(data, data + size);
    if (SUCCESS == object_->PutDouble(skey, static_cast<double>(size))) {
        uint32_t ret = object_->Save("local");
        if (!ret) {
            return false;
        }

        if (object_->RevokeSave()) {
            return false;
        }
        objectStore_->DeleteObject(SESSIONID);
    }
    return true;
}

bool CreateObjectV9Fuzz(const uint8_t *data, size_t size)
{
    std::string bundleName = "com.example.myapplication";
    DistributedObjectStore *objectStore = nullptr;
    DistributedObject *object = nullptr;
    objectStore = DistributedObjectStore::GetInstance(bundleName);
    uint32_t status = 0;
    std::string skey(data, data + size);
    if (objectStore != nullptr) {
        object = objectStore->CreateObject(skey, status);
        if (object == nullptr || status != SUCCESS) {
            return false;
        }
        double val = static_cast<double>(size);
        double result;
        if (SUCCESS == object_->PutDouble(skey, val)) {
            if (object_->GetDouble(skey, result)) {
                return false;
            }
        }
        objectStore_->DeleteObject(skey);
        return true;
    }
    return false;
}

bool GetFuzz(const uint8_t *data, size_t size)
{
    std::string bundleName = "default1";
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(bundleName);
    if (objectStore == nullptr) {
        return false;
    }

    DistributedObject *object = objectStore->CreateObject(SESSIONIDv9);
    if (object == nullptr) {
        return false;
    }

    DistributedObject *object2 = nullptr;
    std::string skey(data, data + size);
    if (!objectStore->Get(skey, &object2)) {
        return false;
    }

    if (object != object2) {
        return false;
    }
    objectStore->DeleteObject(SESSIONIDv9);
    return true;
}

}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::PutDoubleFuzz(data, size);
    OHOS::PutBooleanFuzz(data, size);
    OHOS::PutStringFuzz(data, size);
    OHOS::PutComplexFuzz(data, size);
    OHOS::GetDoubleFuzz(data, size);
    OHOS::GetBooleanFuzz(data, size);
    OHOS::GetStringFuzz(data, size);
    OHOS::GetComplexFuzz(data, size);
    OHOS::GetTypeFuzz(data, size);
    OHOS::SaveFuzz(data, size);
    OHOS::SaveAndRevokeSaveFuzz(data, size);
    OHOS::CreateObjectV9Fuzz(data, size);
    OHOS::GetFuzz(data, size);
    /* Run your code on data */
    return 0;
}