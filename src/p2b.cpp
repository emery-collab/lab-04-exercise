#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include "p2a.h"
#include "timer.h"


constexpr size_t chunkSize = 256;
constexpr size_t K = 4;
constexpr size_t output = chunkSize / K;

constexpr size_t inputEle = chunkSize * chunkSize * chunkSize;
constexpr size_t kEle = K * K * K;
constexpr size_t outputEle = output * output * output;

size_t rowMajorIndexA(size_t x, size_t y, size_t z) {
    return z * chunkSize * chunkSize + y * chunkSize + x;
}

size_t rowMajorIndexK(size_t x, size_t y, size_t z) {
    return z * K * K + y * K + x;
}

size_t rowMajorIndexConv(size_t x, size_t y, size_t z) {
    return z * output * output + y * output + x;
}

int main(){

    // allocate input arrays
    std::vector<uint64_t> arrayA(inputEle);
    std::vector<uint64_t> B(inputEle);

    // initialize a with random values
    std::mt19937_64 rng(0);

    for (size_t i = 0; i < inputEle; ++i) {
        arrayA[i] = rng();
    }

    // create b in morton order
    for (size_t z = 0; z < chunkSize; ++z) {
        for (size_t y = 0; y < chunkSize; ++y) {
            for (size_t x = 0; x < chunkSize; ++x) {
                B[morton3d(x, y, z)] =
                    arrayA[rowMajorIndexA(x, y, z)];
            }
        }
    }

    // create the two kernels
    std::vector<uint64_t> kernelarrayA(kEle);
    std::vector<uint64_t> kernelB(kEle);

    for (size_t z = 0; z < K; ++z) {
        for (size_t y = 0; y < K; ++y) {
            for (size_t x = 0; x < K; ++x) {
                uint64_t varrayAlue = x + y + z;

                // row major kernel
                kernelarrayA[rowMajorIndexK(x, y, z)] = varrayAlue;

                // morton order kernel
                kernelB[morton3d(x, y, z)] = varrayAlue;
            }
        }
    }

    // allocate output arrays
    std::vector<uint64_t> outputarrayA(outputEle);
    std::vector<uint64_t> outputB(outputEle);

    // row major convolution
    Timer timer;
    timer.restart();

    for (size_t z = 0; z < output; ++z) {
        for (size_t y = 0; y < output; ++y) {
            for (size_t x = 0; x < output; ++x) {
                uint64_t sum = 0;

                // starting position of this 4x4x4 chunk
                size_t inputX = x * K;
                size_t inputY = y * K;
                size_t inputZ = z * K;

                // walk through the 4x4x4 kernel
                for (size_t kz = 0; kz < K; ++kz) {
                    for (size_t ky = 0; ky < K; ++ky) {
                        for (size_t kx = 0; kx < K; ++kx) {
                            size_t inputIdx =
                                rowMajorIndexA(
                                    inputX + kx,
                                    inputY + ky,
                                    inputZ + kz
                                );
                            size_t kernelIdx =
                                rowMajorIndexK(kx, ky, kz);

                            sum +=
                                arrayA[inputIdx] *
                                kernelarrayA[kernelIdx];
                        }
                    }
                }
                outputarrayA[rowMajorIndexConv(x, y, z)] = sum;
            }
        }
    }

    uint64_t rowMarrayAjorTime =
        timer.click<Timer::Micros>();

    // morton convolution
    timer.restart();

    for (size_t z = 0; z < output; ++z) {
        for (size_t y = 0; y < output; ++y) {
            for (size_t x = 0; x < output; ++x) {
                uint64_t sum = 0;

                // starting coordinate of this 4x4x4 block.
                size_t inputX = x * K;
                size_t inputY = y * K;
                size_t inputZ = z * K;

                // walk through the kernel.
                for (size_t kz = 0; kz < K; ++kz) {
                    for (size_t ky = 0; ky < K; ++ky) {
                        for (size_t kx = 0; kx < K; ++kx) {
                            size_t inputIdx =
                                morton3d(
                                    inputX + kx,
                                    inputY + ky,
                                    inputZ + kz
                                );

                            size_t kernelIdx =
                                morton3d(kx, ky, kz);


                            sum +=
                                B[inputIdx] *
                                kernelB[kernelIdx];
                        }
                    }
                }
                // output b is kept in row-major order so
                // that it can easily be compared with output a.
                outputB[rowMajorIndexConv(x, y, z)] = sum;
            }
        }
    }
    uint64_t mortonTime = timer.click<Timer::Micros>();

    // verify the two convolutions produced the same result
    for (size_t z = 0; z < output; ++z) {

        for (size_t y = 0; y < output; ++y) {

            for (size_t x = 0; x < output; ++x) {

                size_t idx = rowMajorIndexConv(x, y, z);

                assert(outputarrayA[idx] == outputB[idx]);
            }
        }
    }

    // output
    std::cout
        << rowMarrayAjorTime
        << "\n";

    std::cout
        << mortonTime
        << "\n";

    return 0;
}
