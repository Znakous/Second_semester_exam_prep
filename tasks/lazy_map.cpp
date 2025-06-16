// 15-20 баллов
#include <iostream>

// std::any and std::variant deprecated
class LazyEvaluatedMap;

int main() {
    LazyEvaluatedMap m;

    m["x"] = [](){ std::cout << "x "; return 2; };
    m["y"] = [](){ std::cout << "y "; return 3; };

    std::cout << m["x"] << '\n'; // x 2
    std::cout << m["x"] << '\n'; // 2
    
    std::cout << m["y"] << '\n'; // y 3
    std::cout << m["y"] << '\n'; // 3

    return 0;
}