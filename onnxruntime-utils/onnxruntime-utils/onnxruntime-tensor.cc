#include "onnxruntime-tensor.h"

std::vector<int64_t> ComputeStrides(const std::vector<int64_t> &shape)
{
    int64_t ndim = shape.size();
    std::vector<int64_t> strides(ndim);
    strides[ndim - 1] = 1;
    for (int64_t i = ndim - 2; i >= 0; --i)
    {
        strides[i] = strides[i + 1] * shape[i + 1];
    }
    return strides;
}


