/**
 * Copyright (c) 2016-present, Facebook, Inc.
 *
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

#include "pybind_state_dlpack.h"

namespace caffe2 {
namespace python {

namespace py = pybind11;

const DLDeviceType* CaffeToDLDeviceType(int device_type) {
  static std::map<int, DLDeviceType> dl_device_type_map{
      {CPU, kCPU},
      {CUDA, kGPU},
  };
  const auto it = dl_device_type_map.find(device_type);
  return it == dl_device_type_map.end() ? nullptr : &it->second;
}

const DLDataType* CaffeToDLType(const TypeMeta& meta) {
  static std::map<CaffeTypeId, DLDataType> dl_type_map{
      {TypeMeta::Id<int8_t>(), DLDataType{0, 8, 1}},
      {TypeMeta::Id<int16_t>(), DLDataType{0, 16, 1}},
      {TypeMeta::Id<int32_t>(), DLDataType{0, 32, 1}},
      {TypeMeta::Id<int64_t>(), DLDataType{0, 64, 1}},
      {TypeMeta::Id<uint8_t>(), DLDataType{1, 8, 1}},
      {TypeMeta::Id<uint16_t>(), DLDataType{1, 16, 1}},
      {TypeMeta::Id<float16>(), DLDataType{2, 16, 1}},
      {TypeMeta::Id<float>(), DLDataType{2, 32, 1}},
      {TypeMeta::Id<double>(), DLDataType{2, 64, 1}},
  };
  const auto it = dl_type_map.find(meta.id());
  return it == dl_type_map.end() ? nullptr : &it->second;
}

const TypeMeta& DLTypeToCaffe(const DLDataType& dl_type) {
  try {
    if (dl_type.lanes != 1) {
      throw std::invalid_argument("invalid type");
    }
    static std::map<int, std::map<int, TypeMeta>> dl_caffe_type_map{
        {0,
         std::map<int, TypeMeta>{
             {8, TypeMeta::Make<int8_t>()},
             {16, TypeMeta::Make<int16_t>()},
             {32, TypeMeta::Make<int32_t>()},
             {64, TypeMeta::Make<int64_t>()},
         }},
        {1,
         std::map<int, TypeMeta>{
             {8, TypeMeta::Make<uint8_t>()},
             {16, TypeMeta::Make<uint16_t>()},
         }},
        {2,
         std::map<int, TypeMeta>{
             {16, TypeMeta::Make<float16>()},
             {32, TypeMeta::Make<float>()},
             {64, TypeMeta::Make<double>()},
         }},
    };
    if (!dl_caffe_type_map.count(dl_type.code)) {
      throw std::invalid_argument("invalid type");
    }
    const auto& bits_map = dl_caffe_type_map.at(dl_type.code);
    if (!bits_map.count(dl_type.bits)) {
      throw std::invalid_argument("invalid type");
    }
    return bits_map.at(dl_type.bits);
  } catch (const std::invalid_argument& e) {
    CAFFE_THROW(
        "Unsupported DLDataType: ", dl_type.code, dl_type.bits, dl_type.lanes);
  }
}

} // namespace python
} // namespace caffe2
