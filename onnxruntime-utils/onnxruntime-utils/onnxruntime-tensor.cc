#include "onnxruntime-tensor.h"

#include <sstream>

std::vector<int64_t> Unsqueeze(const std::vector<int64_t> &shape, int axis) {
  if (axis < 0 || axis > shape.size()) {
    std::ostringstream ss;
    ss << "axis out of range[0, " << shape.size() << "]";
    throw std::invalid_argument(ss.str());
  }

  std::vector<int64_t> new_shape(shape.size() + 1, 1);
  for (int n_i = 0, i = 0; n_i < new_shape.size(); ++n_i) {
    if (n_i == axis) {
      continue;
    }
    new_shape[n_i] = shape[i];
    ++i;
  }
  return new_shape;
}

// in-place
void Unsqueeze(std::vector<int64_t> *shape, int axis) {
  auto new_shape = Unsqueeze(*shape, axis);
  shape->swap(new_shape);
}

std::vector<int64_t> ComputeStrides(const int64_t *shape, int shape_size) {
  std::vector<int64_t> strides(shape_size);
  strides[shape_size - 1] = 1;
  for (int64_t i = shape_size - 2; i >= 0; --i) {
    strides[i] = strides[i + 1] * shape[i + 1];
  }
  return strides;
}

std::vector<int64_t> ComputeStrides(const std::vector<int64_t> &shape) {
  return ComputeStrides(shape.data(), shape.size());
}

Ort::Value View(Ort::Value *value, const std::vector<int64_t> &shape) {
  return View(value, shape.data(), shape.size());
}

Ort::Value View(Ort::Value *value, const int64_t *shape, size_t shape_len) {
  size_t element_byte_size = GetTensorElementByteSize(value);
  size_t element_count = value->GetTensorTypeAndShapeInfo().GetElementCount();

  for (int i = 0; i < shape_len; ++i) {
    if (shape[i] <= 0) {
      throw std::invalid_argument("shape must be positive");
    }
  }

  if (std::accumulate(shape, shape + shape_len, 1, std::multiplies<>()) !=
      element_count) {
    throw std::runtime_error("Invalid shape");
  }

  Ort::MemoryInfo memory_info =
      Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeDefault);
  return Ort::Value::CreateTensor(
      memory_info, value->GetTensorMutableRawData(),
      value->GetTensorTypeAndShapeInfo().GetElementCount() * element_byte_size,
      shape, shape_len, value->GetTensorTypeAndShapeInfo().GetElementType());
}

size_t GetTensorElementByteSize(const ONNXTensorElementDataType &data_type) {
  switch (data_type) {
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
      return sizeof(int32_t);
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
      return sizeof(int64_t);
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
      return sizeof(float);
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:
      return sizeof(bool);
    default:
      throw std::runtime_error("unsupported data type");
      // unreachable code
      return 0;
  }
}

size_t GetTensorElementByteSize(Ort::Value *v) {
  auto tensor_type_and_shape = v->GetTensorTypeAndShapeInfo();
  auto tensor_type = tensor_type_and_shape.GetElementType();
  return GetTensorElementByteSize(tensor_type);
}

Ort::Value Repeat(Ort::Value *input,
                  const std::vector<int64_t> &repeat_factors) {
  void *input_data = input->GetTensorMutableRawData();
  size_t input_data_size = input->GetTensorTypeAndShapeInfo().GetElementCount();
  std::vector<int64_t> input_shape =
      input->GetTensorTypeAndShapeInfo().GetShape();
  assert(input_shape.size() ==
         repeat_factors.size());  // FIXME: remove this assert.
  auto input_data_type = input->GetTensorTypeAndShapeInfo().GetElementType();
  size_t element_byte_size = GetTensorElementByteSize(input_data_type);

  std::vector<int64_t> output_shape(input_shape.size());
  for (int i = 0; i < input_shape.size(); ++i) {
    output_shape[i] = input_shape[i] * repeat_factors[i];
  }

  Ort::AllocatorWithDefaultOptions allocator;
  Ort::Value output_tensor = Ort::Value::CreateTensor(
      allocator, output_shape.data(), output_shape.size(), input_data_type);
  void *output_data = output_tensor.GetTensorMutableRawData();
  size_t output_size =
      output_tensor.GetTensorTypeAndShapeInfo().GetElementCount();

  switch (input_data_type) {
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
      Repeat(static_cast<int32_t *>(input_data),
             static_cast<int64_t>(input_data_size), input_shape.data(),
             input_shape.size(), repeat_factors.data(), repeat_factors.size(),
             static_cast<int32_t *>(output_data), output_size,
             output_shape.data(), output_shape.size());
      break;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
      Repeat(static_cast<int64_t *>(input_data),
             static_cast<int64_t>(input_data_size), input_shape.data(),
             input_shape.size(), repeat_factors.data(), repeat_factors.size(),
             static_cast<int64_t *>(output_data), output_size,
             output_shape.data(), output_shape.size());
      break;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
      Repeat(static_cast<float *>(input_data),
             static_cast<int64_t>(input_data_size), input_shape.data(),
             input_shape.size(), repeat_factors.data(), repeat_factors.size(),
             static_cast<float *>(output_data), output_size,
             output_shape.data(), output_shape.size());
      break;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:
      Repeat(static_cast<bool *>(input_data),
             static_cast<int64_t>(input_data_size), input_shape.data(),
             input_shape.size(), repeat_factors.data(), repeat_factors.size(),
             static_cast<bool *>(output_data), output_size, output_shape.data(),
             output_shape.size());
      break;
    default:
      throw std::runtime_error("unsupported data type");
      break;
  }

  return output_tensor;
}