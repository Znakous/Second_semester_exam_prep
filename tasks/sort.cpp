// 2024 год было +- такое на 20
#include <type_traits> // только для std::is_same_v, в остальном deprecated


int main() {
    static_assert(std::is_same_v<sort<vector<4,1,2,5,6,3>>::type, vector<1,2,3,4,5,6>>);
}