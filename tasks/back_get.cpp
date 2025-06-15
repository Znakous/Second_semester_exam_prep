#include <iostream>
#include <tuple>


int main() {
    auto t = std::make_tuple(1, 2, 3);

    // // std deprecated 
    std::cout << "rget:\n";
    std::cout << rget<0>(t) << std::endl; // 3
    std::cout << rget<1>(t) << std::endl; // 2
    std::cout << rget<2>(t) << std::endl; // 1
}