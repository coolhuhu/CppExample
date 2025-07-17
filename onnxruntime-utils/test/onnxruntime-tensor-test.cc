#include "onnxruntime-tensor.h"

#include <iostream>
#include <vector>

template <typename T>
bool CheckEqual(const T *left, size_t left_n, const T *right, size_t right_n) {
  if (left_n != right_n) {
    return false;
  }
  bool flag = true;
  for (size_t i = 0; i < left_n; i++) {
    if (left[i] != right[i]) {
      std::cerr << "The elements with subscript " << i
                << " in left and right are not equal" << std::endl;
      flag = false;
    }
  }
  return flag;
}

template <typename T>
void PrintVector(const std::vector<T> &data) {
  for (auto v : data) {
    std::cout << v << " ";
  }
  std::cout << std::endl;
}

template <typename T>
bool CheckEqual(const std::vector<T> &left, const std::vector<T> &right) {
  return CheckEqual(left.data(), left.size(), right.data(), right.size());
}

// 如何有效的测试 Repeat 函数？
// 一个方法是，使用 torch 构造输入， torch.repeat 构造输出，然后将输入和输出的
// tensor 以 nunmy 的形式写成文件，这样就有了正确的测试输入和输出；然后借助
// cnpy(https://github.com/rogersce/cnpy) 读取 tensor，最后调用函数进行测试。
void TestRepeat() {
  std::cout << "TestRepeat start..." << std::endl;

  std::vector<float> tensor{1, 3, 4, 2, 5, 6};
  std::vector<int64_t> shape{2, 3};

  std::vector<int64_t> repeat_factors_1{1, 3};
  std::vector<int64_t> repeat_factors_2{3, 1};
  std::vector<int64_t> repeat_factors_3{1, 1};
  std::vector<int64_t> repeat_factors_4{2, 1};

  std::vector<int64_t> output_shape_1{2, 9};
  std::vector<float> output_tensor_1{1, 3, 4, 1, 3, 4, 1, 3, 4,
                                     2, 5, 6, 2, 5, 6, 2, 5, 6};

  std::vector<int64_t> output_shape_2{6, 3};
  std::vector<float> output_tensor_2{1, 3, 4, 2, 5, 6, 1, 3, 4,
                                     2, 5, 6, 1, 3, 4, 2, 5, 6};

  std::vector<int64_t> output_shape_3{2, 3};
  std::vector<float> output_tensor_3{1, 3, 4, 2, 5, 6};

  std::vector<int64_t> output_shape_4{4, 3};
  std::vector<float> output_tensor_4{1, 3, 4, 2, 5, 6, 1, 3, 4, 2, 5, 6};

  auto output_1 = Repeat(tensor, shape, repeat_factors_1);
  CheckEqual(output_1.second, output_shape_1);
  CheckEqual(output_1.first, output_tensor_1);

  auto output_2 = Repeat(tensor, shape, repeat_factors_2);
  CheckEqual(output_2.second, output_shape_2);
  CheckEqual(output_2.first, output_tensor_2);

  auto output_3 = Repeat(tensor, shape, repeat_factors_3);
  CheckEqual(output_3.second, output_shape_3);
  CheckEqual(output_3.first, output_tensor_3);

  auto output_4 = Repeat(tensor, shape, repeat_factors_4);
  CheckEqual(output_4.second, output_shape_4);
  CheckEqual(output_4.first, output_tensor_4);

  std::cout << "TestRepeat passed!" << std::endl;
}

void TestUnsqueeze() {
  std::cout << "TestUnsqueeze start..." << std::endl;

  std::vector<int64_t> shape{2, 4, 2, 6};

  std::vector<int64_t> output_shape_1{1, 2, 4, 2, 6};
  CheckEqual(Unsqueeze(shape, 0), output_shape_1);

  std::vector<int64_t> output_shape_2{2, 1, 4, 2, 6};
  CheckEqual(Unsqueeze(shape, 1), output_shape_2);

  std::vector<int64_t> output_shape_3{2, 4, 1, 2, 6};
  CheckEqual(Unsqueeze(shape, 2), output_shape_3);

  std::vector<int64_t> output_shape_4{2, 4, 2, 1, 6};
  CheckEqual(Unsqueeze(shape, 3), output_shape_4);

  std::vector<int64_t> output_shape_5{2, 4, 2, 6, 1};
  CheckEqual(Unsqueeze(shape, 4), output_shape_5);

  std::vector<int64_t> input_shape = shape;
  Unsqueeze(&input_shape, 0);
  CheckEqual(input_shape, output_shape_1);

  input_shape = shape;
  Unsqueeze(&input_shape, 1);
  CheckEqual(input_shape, output_shape_2);

  input_shape = shape;
  Unsqueeze(&input_shape, 2);
  CheckEqual(input_shape, output_shape_3);

  input_shape = shape;
  Unsqueeze(&input_shape, 3);
  CheckEqual(input_shape, output_shape_4);

  input_shape = shape;
  Unsqueeze(&input_shape, 4);
  CheckEqual(input_shape, output_shape_5);

  std::cout << "TestUnsqueeze passed!" << std::endl;
}

void TestView() {
  std::cout << "TestView start..." << std::endl;

  std::vector<float> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  std::vector<int64_t> shape = {2, 3, 2};
  Ort::MemoryInfo memory_info =
      Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeDefault);
  Ort::Value tensor = Ort::Value::CreateTensor(
      memory_info, values.data(), values.size(), shape.data(), shape.size());

  std::vector<int64_t> shape_1{1, 2, 6};
  Ort::Value output_tensor_1 = View(&tensor, shape_1);
  CheckEqual(output_tensor_1.GetTensorTypeAndShapeInfo().GetShape(), shape_1);
  CheckEqual(output_tensor_1.GetTensorMutableData<float>(),
             tensor.GetTensorTypeAndShapeInfo().GetElementCount(),
             values.data(), values.size());

  std::vector<int64_t> shape_2{2, 1, 6};
  Ort::Value output_tensor_2 = View(&tensor, shape_2);
  CheckEqual(output_tensor_2.GetTensorTypeAndShapeInfo().GetShape(), shape_2);
  CheckEqual(output_tensor_2.GetTensorMutableData<float>(),
             tensor.GetTensorTypeAndShapeInfo().GetElementCount(),
             values.data(), values.size());

  std::vector<int64_t> shape_3{3, 2, 2};
  Ort::Value output_tensor_3 = View(&tensor, shape_3);
  CheckEqual(output_tensor_3.GetTensorTypeAndShapeInfo().GetShape(), shape_3);
  CheckEqual(output_tensor_3.GetTensorMutableData<float>(),
             tensor.GetTensorTypeAndShapeInfo().GetElementCount(),
             values.data(), values.size());

  std::cout << "TestView passed!" << std::endl;
}

void TestOrtValueRepeat() {
  std::cout << "TestOrtValueRepeat start..." << std::endl;

  std::vector<float> tensor{1, 3, 4, 2, 5, 6};
  std::vector<int64_t> shape{2, 3};

  Ort::MemoryInfo memory_info =
      Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeDefault);
  Ort::Value raw_tensor = Ort::Value::CreateTensor(
      memory_info, tensor.data(), tensor.size(), shape.data(), shape.size());

  std::vector<int64_t> repeat_factors_1{1, 3};
  std::vector<int64_t> output_shape_1{2, 9};
  std::vector<float> output_tensor_1{1, 3, 4, 1, 3, 4, 1, 3, 4,
                                     2, 5, 6, 2, 5, 6, 2, 5, 6};
  Ort::Value output_value_1 = Repeat(&raw_tensor, repeat_factors_1);
  CheckEqual(output_value_1.GetTensorTypeAndShapeInfo().GetShape(),
             output_shape_1);
  CheckEqual(output_value_1.GetTensorMutableData<float>(),
             output_value_1.GetTensorTypeAndShapeInfo().GetElementCount(),
             output_tensor_1.data(), output_tensor_1.size());

  std::vector<int64_t> repeat_factors_2{3, 1};
  std::vector<int64_t> output_shape_2{6, 3};
  std::vector<float> output_tensor_2{1, 3, 4, 2, 5, 6, 1, 3, 4,
                                     2, 5, 6, 1, 3, 4, 2, 5, 6};
  Ort::Value output_value_2 = Repeat(&raw_tensor, repeat_factors_2);
  CheckEqual(output_value_2.GetTensorTypeAndShapeInfo().GetShape(),
             output_shape_2);
  CheckEqual(output_value_2.GetTensorMutableData<float>(),
             output_value_2.GetTensorTypeAndShapeInfo().GetElementCount(),
             output_tensor_2.data(), output_tensor_2.size());

  std::cout << "TestOrtValueRepeat passed!" << std::endl;
}

void TestTopK() {
  std::cout << "TestTopK started..." << std::endl;
  std::vector<int> values{9, 1, 2, 7, 8, 5, 6, 4, 3, 0};
  int k = 3;

  auto topk_value_and_indices = TopK(values, k);
  std::vector<int> top_k{9, 8, 7};
  std::vector<int> top_k_index{0, 4, 3};
  CheckEqual(topk_value_and_indices.first, top_k);
  CheckEqual(topk_value_and_indices.second, top_k_index);

  topk_value_and_indices = TopK(values.data() + 3, 7, k);
  top_k = std::vector<int>{8, 7, 6};
  top_k_index = std::vector<int>{1, 0, 3};
  CheckEqual(topk_value_and_indices.first, top_k);
  CheckEqual(topk_value_and_indices.second, top_k_index);

  std::cout << "TestTopK passed!" << std::endl;
}

int main() {
  TestRepeat();
  TestUnsqueeze();
  TestView();
  TestOrtValueRepeat();
  TestTopK();
}
