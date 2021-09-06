// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include <vector>
#include <variant>

#include "src/utils/ApeHashMap.h"
#include "src/utils/DecimalConvertor.h"

namespace ape {

class DumpUtils {
 public:
  static void dumpGroupByKeyToJavaBuffer(const std::vector<Key>& keys,
                                         uint8_t* bufferAddr, uint8_t* nullAddr,
                                         const int index, const parquet::Type::type pType,
                                         int32_t offset, int32_t len) {
    *(nullAddr + index) = 1;
    switch (pType) {
      case parquet::Type::INT32: {
        for (int i = 0; i < len; i++) {
          *((int32_t*)(bufferAddr) + i) = std::get<0>(keys[offset + i][index]);
        }
        break;
      }
      case parquet::Type::INT64: {
        for (int i = 0; i < len; i++) {
          *((int64_t*)(bufferAddr) + i) = std::get<1>(keys[offset + i][index]);
        }
        break;
      }
      case parquet::Type::FLOAT: {
        for (int i = 0; i < len; i++) {
          *((float*)(bufferAddr) + i) = std::get<2>(keys[offset + i][index]);
        }
        break;
      }
      case parquet::Type::DOUBLE: {
        for (int i = 0; i < len; i++) {
          *((double*)(bufferAddr) + i) = std::get<3>(keys[offset + i][index]);
        }
        break;
      }
      case parquet::Type::BYTE_ARRAY: {
        for (int i = 0; i < len; i++) {
          *((parquet::ByteArray*)(bufferAddr) + i) = std::get<4>(keys[offset + i][index]);
        }
        break;
      }
      default: {
        ARROW_LOG(WARNING) << "Do not support yet";
        break;
      }
    }
  }

  static void dumpToJavaBuffer(uint8_t* bufferAddr, uint8_t* nullAddr,
                               const DecimalVector& result, int32_t offset, int32_t len) {
    for (int i = 0; i < len; i++) {
      *(nullAddr + i) = result.nullVector->at(i);
      switch (result.type) {
        case ResultType::IntType: {
          *((int32_t*)bufferAddr + i) =
              static_cast<int32_t>(result.data[offset + i].low_bits());
          break;
        }
        case ResultType::LongType: {
          *((int64_t*)bufferAddr + i) =
              static_cast<int64_t>(result.data[offset + i].low_bits());
          break;
        }
        case ResultType::FloatType: {
          // TODO: convert
          break;
        }
        case ResultType::DoubleType: {
          // TODO: this is just for UnscaledValue case, if the data type is Double, this
          // will not work
          arrow::Decimal128 tmp(result.data[offset + i]);
          *((double*)bufferAddr + i) = tmp.ToDouble(0);
          break;
        }
        case ResultType::Decimal64Type: {
          *((int64_t*)bufferAddr + i) =
              static_cast<int64_t>(result.data[offset + i].low_bits());
          break;
        }
        case ResultType::Decimal128Type: {
          decimalToBytes(result.data[offset + i], result.precision,
                         (uint8_t*)(bufferAddr + i * 16));
          break;
        }
        default: {
          ARROW_LOG(WARNING) << "Type not support!";
          break;
        }
      }
    }
  }
};
}  // namespace ape