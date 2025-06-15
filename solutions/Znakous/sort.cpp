#include <type_traits>

template <int... values>
struct vector {};


template<int a, int... others>
struct vector<a, others...> {
    static constexpr int length = vector<others...>::length + 1;
};

template <>
struct vector<> {
    static constexpr int length = 0;
};

template<int val, typename Vec>
struct prepend{};

template<int a, int... others>
struct prepend<a, vector<others...>> {
    using type = vector<a, others...>;
};

template<int val, typename T>
using prepend_t = prepend<val, T>::type;

template<bool expr, typename T, typename F>
struct conditional{};

template<typename T, typename F>
struct conditional<true, T, F> {
    using type = T;
};
template<typename T, typename F>
struct conditional<false, T, F> {
    using type = F;
};

template<typename T>
struct sep_min {};

template<int a>
struct sep_min <vector<a>> {
    using this_type = vector<a>;
    using without = vector<>;
    static constexpr int min_val = a;
    static constexpr int size = 1;
};
template<>
struct sep_min <vector<>> {
    using this_type = vector<>;
    using without = vector<>;
    static constexpr int min_val = 1e9;
    static constexpr int size = 0;
};

template<int a, int... others>
struct sep_min <vector<a, others...>> {
    static constexpr int size = vector<others...>::size;
    using other_sep = sep_min<vector<others...>>;
    static constexpr int min_val = (a < other_sep::min_val) ? a : other_sep::min_val;
    using this_type = vector<a, others...>;
    using without = conditional<a < other_sep::min_val, typename other_sep::this_type, prepend_t<a, typename other_sep::without>>::type;
};

template<typename T>
struct sort{};

template<int... others>
struct sort <vector<others...>> {
    using sep_min = sep_min<vector<others...>>;
    using type = prepend_t<sep_min::min_val, typename sort<typename sep_min::without>::type>;
};
template<int a>
struct sort <vector<a>> {
    using type = vector<a>;
};
template<>
struct sort <vector<>> {
    using type = vector<>;
};

int main() {
    static_assert(std::is_same_v<sort<vector<4,1,2,5,6,3>>::type, vector<1,2,3,4,5,6>>);
}