//===----------------------------------------------------------------------===//
//
// Copyright (c) 2020-2030 by Sophgo Technologies Inc. All rights reserved.
//
// Licensed under the Apache License v2.0.
// See http://www.apache.org/licenses/LICENSE-2.0 for license information.
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//

#pragma once

#include "mlir/Support/LLVM.h"
#include "tpu_mlir/Dialect/Tpu/IR/TpuOps.h"
#include "tpu_mlir/Support/Module.h"
#include <list>
#include <map>
#include <set>

#include "tpu_mlir/Dialect/Tpu/Transforms/LayerGroup/BasicTimeStep.h"
#include "tpu_mlir/Dialect/Tpu/Transforms/LayerGroup/LayerGroupDefs.h"

namespace tpu_mlir {
namespace tpu {

shape_secs_t get_group_max_secs(const LgInfo &lg_info);
shape_secs_t init_group_data_secs(const LgInfo &lg_info);

void update_tensor_infos(const LgInfo &lg_info, TensorInfo &tensor_infos);
bool update_data_split(BasicTimeStepPtr time_step, const LgInfo &lg_info,
                       shape_secs_t &shape_secs);

bool strip_back_judge(Value v, const LgInfo &lg_info,
                      const std::multiset<Operation *> &op_set,
                      const std::set<Value, value_compare> &out_tensor_set);
bool is_same_slice_info(const slice_info_t &si0, const slice_info_t &si1);
slice_info_t get_out_slice_info(const shape_secs_t &shape_secs,
                                int64_t max_nslice, int64_t max_hslice,
                                int64_t n, int64_t h);
bool get_backward_slice_info(slice_info_t &in_si, const slice_info_t &out_si, Operation *op);
bool stripe_mine_max_slice(const LgInfo &lg_info,
                           const shape_secs_t &shape_secs,
                           TensorInfo &tensor_infos);

void get_max_slice_nh(const slice_info_t &slice_info, int64_t &max_nslice,
                      int64_t &max_hslice);

int64_t get_buffer_size(const GdmaElt &tensor);

bool stripe_mine_idx_slice(const LgInfo &lg_info,
                           const shape_secs_t &shape_secs,
                           TensorInfo &tensor_infos);

void set_fake_local_layer_param(Operation *op, int64_t nidx, int64_t nslice,
                           int64_t hidx, int64_t hslice);
void delete_fake_local_layer_param(Operation *op);
void generate_fake_global_addr(Operation *op);

void delete_fake_global_addr(Operation *op);

bool isWeightValue(Value v);

bool is_eu_align(Value opd);

bool need_bcast(Value opd);

int64_t use_3ic(Value opd);

std::vector<Value> get_input_values(Operation *op);
std::vector<Value> get_output_values(Operation *op);

} // namespace tpu
} // namespace tpu_mlir
