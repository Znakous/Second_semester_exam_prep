// в прошлом году было на много баллов
// в этом году всего на 5-7

int main() {
    static_assert(is_arithmetic_progression<>::value);
    static_assert(is_arithmetic_progression<1>::value);
    static_assert(is_arithmetic_progression<2, 3>::value);
    static_assert(is_arithmetic_progression<0, 1, 2, 3, 4, 5>::value);
    static_assert(!is_arithmetic_progression<0, 1, 2, 3, 4, 7>::value);
}
