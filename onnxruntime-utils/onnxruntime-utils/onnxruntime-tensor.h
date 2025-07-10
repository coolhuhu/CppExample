#ifndef ONNXRUNTIME_TENSOR_H_
#define ONNXRUNTIME_TENSOR_H_

#include "onnxruntime_cxx_api.h"
#include <vector>
#include <array>
#include <exception>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <functional>
#include <numeric>
#include <utility>


// TODO:
// 1. TopK
// 2. View
// 3. Unsqueeze
// 4. Squeeze






std::vector<int64_t> ComputeStrides(const std::vector<int64_t> &shape);



template <typename T>
std::pair<std::vector<T>, std::vector<int64_t>>
Repeat(const std::vector<T> &input,
       const std::vector<int64_t> &input_shape,
       const std::vector<int64_t> &repeat_factors)
{
    int64_t ndim = input_shape.size();
    if (repeat_factors.size() != ndim)
    {
        throw std::invalid_argument("repeat_factors must match input_shape dimensions.");
    }

    // 1. Compute output shape
    std::vector<int64_t> output_shape(ndim);
    for (int64_t i = 0; i < ndim; ++i)
    {
        output_shape[i] = input_shape[i] * repeat_factors[i];
    }

    // 2. Compute total sizes
    int64_t input_size = std::accumulate(input_shape.begin(), input_shape.end(), 1LL, std::multiplies<>());
    int64_t output_size = std::accumulate(output_shape.begin(), output_shape.end(), 1LL, std::multiplies<>());

    std::vector<T> output(output_size);

    // 3. Compute strides
    std::vector<int64_t> input_strides = ComputeStrides(input_shape);
    std::vector<int64_t> output_strides = ComputeStrides(output_shape);

    // 4. Repeat data
    for (int64_t i = 0; i < output_size; ++i)
    {
        // Compute multi-dimensional index in output tensor
        int64_t rem = i;
        std::vector<int64_t> out_idx(ndim);
        for (int64_t d = 0; d < ndim; ++d)
        {
            out_idx[d] = rem / output_strides[d];
            rem %= output_strides[d];
        }

        // Map to input index using modulo
        std::vector<int64_t> in_idx(ndim);
        for (int64_t d = 0; d < ndim; ++d)
        {
            in_idx[d] = out_idx[d] % input_shape[d];
        }

        // Flatten input index
        int64_t input_offset = 0;
        for (int64_t d = 0; d < ndim; ++d)
        {
            input_offset += in_idx[d] * input_strides[d];
        }

        output[i] = input[input_offset];
    }

    return std::make_pair(output, output_shape);
}

// Ort::Value CreateTensor(ONNXTensorElementDataType data_type,
//                         const int64_t *shape, size_t shape_len)
// {
//     Ort::AllocatorWithDefaultOptions allocator;

//     switch (data_type)
//     {
//     case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
//         return Ort::Value::CreateTensor<int32_t>(allocator, shape, shape_len);
//     case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
//         return Ort::Value::CreateTensor<int64_t>(allocator, shape, shape_len);
//     case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
//         return Ort::Value::CreateTensor<float>(allocator, shape, shape_len);
//     case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:
//         return Ort::Value::CreateTensor<bool>(allocator, shape, shape_len);
//     default:
//         throw std::runtime_error("unsupported data type");
//         // unreachable code
//         return Ort::Value{nullptr};
//     }
// }

// size_t GetTensorElementByteSize(const ONNXTensorElementDataType &data_type)
// {
//     switch (data_type)
//     {
//     case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
//         return sizeof(int32_t);
//     case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
//         return sizeof(int64_t);
//     case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
//         return sizeof(float);
//     case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:
//         return sizeof(bool);
//     default:
//         throw std::runtime_error("unsupported data type");
//         // unreachable code
//         return 0;
//     }
// }

// Ort::Value Repeat(Ort::Value tensor,
//                   const std::vector<int64_t> &repeat_shape)
// {
//     auto tensor_type_and_shape = tensor.GetTensorTypeAndShapeInfo();
//     auto tensor_shape = tensor_type_and_shape.GetShape();
//     auto tensor_type = tensor_type_and_shape.GetElementType();

//     if (tensor_shape.size() != repeat_shape.size())
//     {
//         throw std::runtime_error("tensor shape size != repeat shape size");
//     }

//     for (auto rs : repeat_shape)
//     {
//         if (rs <= 0)
//         {
//             std::runtime_error("repeat shape must be positive");
//         }
//     }

//     std::vector<int64_t> new_shape(repeat_shape.size());
//     for (int i = 0; i < new_shape.size(); i++)
//     {
//         new_shape[i] = tensor_shape[i] * repeat_shape[i];
//     }

//     int64_t input_total_elements = tensor_type_and_shape.GetElementCount();
//     int64_t output_total_elements = std::accumulate(new_shape.begin(), new_shape.end(), 1, std::multiplies<>());

//     std::cout << "input_total_elements: " << input_total_elements << std::endl;
//     std::cout << "output_total_elements: " << output_total_elements << std::endl;

//     Ort::Value repeat_tensor = CreateTensor(tensor_type, new_shape.data(), new_shape.size());
//     void *repeat_tensor_data = repeat_tensor.GetTensorMutableRawData();
//     void *tensor_data = tensor.GetTensorMutableRawData();

//     size_t element_byte_size = GetTensorElementByteSize(tensor_type);
//     for (int64_t i = 0; i < output_total_elements; ++i)
//     {
//         int64_t idx = 0;
//         int64_t stride = output_total_elements;
//         int64_t tmp = i;
//         for (int64_t j = 0; j < tensor_shape.size(); ++j)
//         {
//             stride /= new_shape[j];
//             int64_t out_idx = tmp / stride;
//             int64_t in_idx = out_idx % tensor_shape[j];
//             idx = idx * tensor_shape[j] + in_idx;
//             tmp %= stride;
//         }
//         std::memcpy(static_cast<char *>(repeat_tensor_data) + i * element_byte_size, static_cast<char *>(tensor_data) + idx * element_byte_size, element_byte_size);
//     }

//     return repeat_tensor;
// }

#endif //  ONNXRUNTIME_TENSOR_H_