#include <random> // mt19937_64 mersenne twister initialize with std::mtd19937_64 rng(0)
#include <cstdint> // uint64_t for rng to return

#include <chrono> // timer
#include <vector> // arrays
#include <iostream> // std::cout


int main() { 

    constexpr std::size_t numElements = 16'000'000;

    std::vector<uint64_t> array1(numElements);
    std::vector<uint64_t> array2(numElements);

    // Have two separate for loops to do this to ensure the seed is identical
    // for the row major and column major arrays, respectively

    std::mt19937_64 rng(0);

    for (int x=0; x < 4000; ++x){
        for (int y=0; y < 4000; ++y){
            array1[x * 4000 + y] = rng();
        }
    }

    rng.seed(0);

    for (int y=0; y < 4000; ++y){
        for (int x=0; x < 4000; ++x) {
            array2[x * 4000 + y] = rng();
        }
    }

    // First, sum in row access order
    // Then, print the time to access in microseconds and the sum, respectively

    uint64_t sum = 0;

    auto t1 = std::chrono::steady_clock::now();

    for (int x=0; x < 4000; ++x){
        for (int y=0; y < 4000; ++y){
            sum += array1[x * 4000 + y];
        }
    }   

    auto t2 = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " "
              << sum << "\n";

    // First, sum in column access order
    // Then, print the time to access in microseconds and the sum, respectively

    sum = 0;
    
    t1 = std::chrono::steady_clock::now();

    for (int y=0; y < 4000; ++y){
        for (int x=0; x < 4000; ++x){
            sum += array2[x * 4000 + y];
        }
    }

    t2 = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " "
              << sum << "\n";

    return 0;
}
