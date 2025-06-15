#include <iostream>
#include <tuple>


template<typename T>
constexpr int length = -1;

template<typename T, typename... others>
constexpr int length<std::tuple<T, others...>> = length<std::tuple<others...>> + 1;

template<>
constexpr int length<std::tuple<>> = 0;

template<int ind, typename A>
auto rget(A a) {
    return std::get<length<A> - ind-1>(a); // вроде кому-то запрещали пользоваться std::get, но тогда кажется нужен свой tuple, ведь поля там приватные
} 

int main() {
    auto t = std::make_tuple(1, 2, 3);

    // // std deprecated 
    std::cout << "rget:\n";
    std::cout << rget<0>(t) << std::endl; // 3
    std::cout << rget<1>(t) << std::endl; // 2
    std::cout << rget<2>(t) << std::endl; // 1
}