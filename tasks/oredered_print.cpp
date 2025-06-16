// 10-15 баллов
// доп таска до 20 - делать сравнение с помощью tag dispatch idiom или sfinae
#include <iostream>

int main() {
    //constexpr function deprecated 
    ordered_print<5, 3, 4, 1, 2>(); // 1 2 3 4 5

}
