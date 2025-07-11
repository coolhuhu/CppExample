#ifndef ONNXRUNTIME_TENSOR_H_
#define ONNXRUNTIME_TENSOR_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <functional>
#include <iostream>
#include <numeric>
#include <queue>
#include <utility>
#include <vector>

#include "onnxruntime_cxx_api.h"

std::vector<int64_t> Unsqueeze(const std::vector<int64_t> &shape, int axis);

void Unsqueeze(std::vector<int64_t> *shape, int axis);

Ort::Value View(Ort::Value *value, const std::vector<int64_t> &shape);

Ort::Value View(Ort::Value *value, const int64_t *shape, size_t shape_len);

std::vector<int64_t> ComputeStrides(const std::vector<int64_t> &shape);

std::vector<int64_t> ComputeStrides(const int64_t *shape, int shape_size);

// NOTE: output and output_shape should have enough memory allocated by the
// caller.
template <typename T>
void Repeat(const T *input, size_t input_size, const int64_t *input_shape,
            size_t input_shape_size, const int64_t *repeat_factors,
            size_t repeat_factors_size, T *output, size_t output_size,
            int64_t *output_shape, size_t output_shape_size) {
  if (input_shape_size != repeat_factors_size) {
    throw std::invalid_argument(
        "repeat_factors must match input_shape dimensions.");
  }

  if (std::accumulate(input_shape, input_shape + input_shape_size, 1,
                      std::multiplies<>()) != input_size) {
    throw std::runtime_error(
        "The number of elements represented by input_shape should be equal to "
        "input_size.");
  }

  if (std::accumulate(output_shape, output_shape + output_shape_size, 1,
                      std::multiplies<>()) != output_size) {
    throw std::runtime_error(
        "The number of elements represented by output_shape should be equal to "
        "output_size.");
  }

  // 3. Compute strides
  std::vector<int64_t> input_strides =
      ComputeStrides(input_shape, input_shape_size);
  std::vector<int64_t> output_strides =
      ComputeStrides(output_shape, output_shape_size);

  // 4. Repeat data
  for (int64_t i = 0; i < output_size; ++i) {
    // Compute multi-dimensional index in output tensor
    int64_t rem = i;
    std::vector<int64_t> out_idx(input_shape_size);
    for (int64_t d = 0; d < input_shape_size; ++d) {
      out_idx[d] = rem / output_strides[d];
      rem %= output_strides[d];
    }

    // Map to input index using modulo
    std::vector<int64_t> in_idx(input_shape_size);
    for (int64_t d = 0; d < input_shape_size; ++d) {
      in_idx[d] = out_idx[d] % input_shape[d];
    }

    // Flatten input index
    int64_t input_offset = 0;
    for (int64_t d = 0; d < input_shape_size; ++d) {
      input_offset += in_idx[d] * input_strides[d];
    }

    output[i] = input[input_offset];
  }
}

template <typename T>
std::pair<std::vector<T>, std::vector<int64_t>> Repeat(
    const std::vector<T> &input, const std::vector<int64_t> &input_shape,
    const std::vector<int64_t> &repeat_factors) {
  int64_t ndim = input_shape.size();
  if (repeat_factors.size() != ndim) {
    throw std::invalid_argument(
        "repeat_factors must match input_shape dimensions.");
  }

  // 1. Compute output shape
  std::vector<int64_t> output_shape(ndim);
  for (int64_t i = 0; i < ndim; ++i) {
    output_shape[i] = input_shape[i] * repeat_factors[i];
  }

  // 2. Compute total sizes
  int64_t input_size = std::accumulate(input_shape.begin(), input_shape.end(),
                                       1LL, std::multiplies<>());
  int64_t output_size = std::accumulate(
      output_shape.begin(), output_shape.end(), 1LL, std::multiplies<>());

  std::vector<T> output(output_size);

  Repeat(input.data(), static_cast<size_t>(input.size()), input_shape.data(),
         static_cast<size_t>(input_shape.size()), repeat_factors.data(),
         static_cast<size_t>(repeat_factors.size()), output.data(),
         static_cast<size_t>(output_size), output_shape.data(),
         static_cast<size_t>(output_shape.size()));

  return std::make_pair(output, output_shape);
}

template <typename T>
struct ValueComp {
  bool operator()(const std::pair<T, int32_t> &lhs,
                  const std::pair<T, int32_t> &rhs) const {
    return lhs.first > rhs.first ||
           (lhs.first == rhs.first && lhs.second < rhs.second);
  }
};

// We refer the pytorch topk implementation
// https://github.com/pytorch/pytorch/blob/master/caffe2/operators/top_k.cc
template <typename T>
std::pair<std::vector<T>, std::vector<int>> TopK(const std::vector<T> &data,
                                                 int32_t k) {
  std::vector<std::pair<T, int32_t>> heap_data;
  int n = data.size();
  for (int32_t i = 0; i < k && i < n; ++i) {
    heap_data.emplace_back(data[i], i);
  }
  std::priority_queue<std::pair<T, int32_t>, std::vector<std::pair<T, int32_t>>,
                      ValueComp<T>>
      pq(ValueComp<T>(), std::move(heap_data));
  for (int32_t i = k; i < n; ++i) {
    if (pq.top().first < data[i]) {
      pq.pop();
      pq.emplace(data[i], i);
    }
  }

  std::vector<T> values(std::min(k, n));
  std::vector<int> indices(std::min(k, n));

  int32_t cur = values->size() - 1;
  while (!pq.empty()) {
    const auto &item = pq.top();
    values[cur] = item.first;
    indices[cur] = item.second;
    pq.pop();
    cur -= 1;
  }

  return std::make_pair(values, indices);
}

size_t GetTensorElementByteSize(const ONNXTensorElementDataType &data_type);

size_t GetTensorElementByteSize(Ort::Value *v);

Ort::Value Repeat(Ort::Value *input,
                  const std::vector<int64_t> &repeat_factors);

#endif  //  ONNXRUNTIME_TENSOR_H_