//===----------------------------------------------------------------------===//
//
// Copyright (C) 2022 Sophgo Technologies Inc.  All rights reserved.
//
// TPU-MLIR is licensed under the 2-Clause BSD License except for the
// third-party components.
//
//===----------------------------------------------------------------------===//

#include "tpu_mlir/Backend/BM168x/BM1684X.h"
#include "tpu_mlir/Backend/BM168x/BM168x.h"
#include "tpu_mlir/Dialect/Tpu/IR/TpuOps.h"
#include "tpu_mlir/Support/Module.h"

#include <cassert>



using namespace tpu_mlir::backend;

// =========================================
// GlobalGenInterface
// =========================================

void tpu::ReciprocalOp::codegen_global_bm1684x() {
  assert(!module::isUniformQuantized(getInput()));

  auto op = getOperation();
  auto input_spec = BM168x::get_input_spec(op);
  auto output_spec = BM168x::get_output_spec(op);
  constbinary_global_spec_t param = {0};
  param.common.binary_type = BINARY_DIV;
  param.common.if_relu = getDoRelu();
  param.common.relu_upper_limit = getReluLimit().convertToDouble();

  param.common.inversed = 0;
  param.common.scale_A = 1;
  param.common.rshift_A = 0;
  param.common.B_const_val = getConstVal().convertToDouble();
  param.common.B_dtype = input_spec->at(0).dtype;
  param.common.inversed = true;

  BM168x::call_global_func("backend_api_constbinary_global", &param,
                           sizeof(param), input_spec->data(),
                           output_spec->data());
}

// =========================================
// LocalGenInterface
// =========================================

int64_t tpu::ReciprocalOp::getBufferSize_bm1684x(
    int64_t in_lmem_bytes, int64_t out_lmem_bytes, int64_t in_nslice,
    int64_t in_hslice, int64_t out_nslice, int64_t out_hslice) {
  int64_t buffer_size = 0;
  auto dtype_A = BM168x::getDataType(getInput());
  if (dtype_A == DTYPE_INT8 || dtype_A == DTYPE_UINT8) {
    buffer_size = in_lmem_bytes * 2;
  }
  return buffer_size;
}

void tpu::ReciprocalOp::assign_sec_info(int64_t n_step, int64_t h_step,
                                        void *sec_info_) {
  local_sec_info_t *sec_info = (local_sec_info_t *)sec_info_;
  memset(sec_info, 0, sizeof(local_sec_info_t));

  int64_t n, c, h, w;
  module::getNCHW(getOutput(), n, c, h, w);
  auto gi = getGroupInfo(n_step, h_step);
  auto in_gi = LocalGenInterface::getGroupInfo(getInput(), n_step, h_step);
  sec_info->n_slice = in_gi.n_slice;
  sec_info->d_slice = 1;
  sec_info->h_slice = in_gi.h_slice;
  sec_info->h_idx = in_gi.h_idx;
  sec_info->is_h_split = !(in_gi.h_idx == 0 && in_gi.h_slice == h);
  sec_info->w_slice = w;
  sec_info->out_n_slice = gi.n_slice;
  sec_info->out_h_idx = gi.h_idx;
  sec_info->out_h_slice = gi.h_slice;
  sec_info->out_w_slice = w;
}

void tpu::ReciprocalOp::codegen_local_bm1684x(int64_t n_step, int64_t h_step,
                                              void *sec_info_) {
  assert(!module::isUniformQuantized(getInput()));
  auto op = getOperation();
  auto input_spec = BM168x::get_input_spec(op);
  auto output_spec = BM168x::get_output_spec(op);

  constbinary_local_spec_t param = {0};
  param.common.binary_type = BINARY_DIV;
  param.common.if_relu = getDoRelu();
  param.common.relu_upper_limit = getReluLimit().convertToDouble();
  param.common.inversed = true;
  param.common.scale_A = 1;
  param.common.rshift_A = 0;
  param.common.B_const_val = getConstVal().convertToDouble();
  param.common.B_dtype = input_spec->at(0).dtype;

  BM168x::call_local_func("backend_api_constbinary_local", &param,
                          sizeof(param), sec_info_, input_spec->data(),
                          output_spec->data());
}


//dynamic codegen
int64_t tpu::ReciprocalOp::dyn_codegen_local_bm1684x(void *buffer) {
return 0;
}

// ======================================
// Dynamic GlobalGenInterface
// ======================================
int64_t tpu::ReciprocalOp::dyn_codegen_global_bm1684x(void *buffer) {
  return 0;
}
