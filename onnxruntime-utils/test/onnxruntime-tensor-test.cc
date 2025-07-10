#include "onnxruntime-tensor.h"

#include <iostream>
#include <vector>

template <typename T>
bool CheckEqual(const std::vector<T> &left, const std::vector<T> &right)
{
    if (left.size() != right.size())
        return false;
    bool flag = true;
    for (size_t i = 0; i < left.size(); i++)
    {
        if (left[i] != right[i])
        {
            std::cerr << "The elements with subscript " << i << " in left and right are not equal" << std::endl;
            flag = false;
        }
    }
    return flag;
}

void TestRepeat()
{
    std::vector<float> tensor{1, 3, 4, 2, 5, 6};
    std::vector<int64_t> shape{2, 3};

    std::vector<int64_t> repeat_factors_1{1, 3};
    std::vector<int64_t> repeat_factors_2{3, 1};
    std::vector<int64_t> repeat_factors_3{1, 1};
    std::vector<int64_t> repeat_factors_4{2, 3};

    std::vector<int64_t> output_shape_1{2, 9};
    std::vector<float> output_tensor_1{1, 3, 4, 1, 3, 4, 1, 3, 4, 2, 5, 6, 2, 5, 6, 2, 5, 6};

    std::vector<int64_t> output_shape_2{6, 3};
    std::vector<float> output_tensor_2{1, 3, 4, 2, 5, 6, 1, 3, 4, 2, 5, 6, 1, 3, 4, 2, 5, 6};

    std::vector<int64_t> output_shape_3{2, 3};
    std::vector<float> output_tensor_3{1, 3, 4, 2, 5, 6};

    std::vector<int64_t> output_shape_4{4, 9};
    std::vector<float> output_tensor_4{1, 3, 4, 1, 3, 4, 1, 3, 4, 2, 5, 6, 2, 5, 6, 2, 5, 6, 1, 3, 4, 1, 3, 4, 1, 3, 4, 2, 5, 6, 2, 5, 6, 2, 5, 6};

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

int main()
{
    TestRepeat();
}
