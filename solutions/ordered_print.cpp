// 15-20 баллов, тут сразу со sfinae и tag dispatch + без std
#include <iostream>

struct True{}; 
struct False{};

// если разрешили std::enable_if, лучше использовать его
template<bool condition>
struct enable_if { using type = void; };

template<>
struct enable_if<false> { };

template<bool condition>
using enable_if_t = enable_if<condition>::type; // ключевой момент в том, что для false сломается подстановка, так как type не существует



// компаратор со sfinae
template<int a, int b, typename=void>
struct less_than {
    using type = False; 
};

template<int a, int b>
struct less_than<a, b, enable_if_t<(a < b)>> {
    using type = True;
};

template<int a, int b>
using less_than_t = less_than<a, b>::type; // такие штуки называются unwrap, на все метафункции желательно их писать


// просто контейнер для int-ов
template<int... values>
struct Sequence {
    using type = Sequence<values...>;
};




// добавить в начало контейнера
template<int val, typename T>
struct add_to_seq {};

template<int val, int... others>
struct add_to_seq<val, Sequence<others...>> {
    using type = Sequence<val, others...>;
};

template<int val, typename T>
using add_to_seq_t = add_to_seq<val, T>::type; 




// условный оператор с tag dispatch
template<typename condition, typename if_true, typename if_false>
struct conditional {};

template<typename if_true, typename if_false>
struct conditional<True, if_true, if_false> {
    using type = if_true;
};

template<typename if_true, typename if_false>
struct conditional<False, if_true, if_false> {
    using type = if_false;
};

template<typename condition, typename if_true, typename if_false>
using conditional_t = conditional<condition, if_true, if_false>::type;




// такой же conditional, но не для типов, а для значений
template<typename condition, int if_true, int if_false>
constexpr int conditional_int = if_false;

template<int if_true, int if_false>
constexpr int conditional_int<True, if_true, if_false> = if_true;




// отделяет минимум из пачки int-ов
template<typename T>
struct extract_min{};

template<int only_one> 
struct extract_min<Sequence<only_one>> {
    static constexpr int value = only_one;
    using remaining = Sequence<>;
};

template<int first, int... others>
struct extract_min<Sequence<first, others...>> {
    static constexpr int from_others = extract_min<Sequence<others...>>::value;

    using has_min_on_front = less_than_t<first, from_others>;

    static constexpr int value = conditional_int<has_min_on_front, first, from_others>;

    using remaining = conditional_t<has_min_on_front, // если минимум в начале
        Sequence<others...>, // забрать начало
        add_to_seq_t<first, typename extract_min<Sequence<others...>>::remaining>>; // иначе приклеить начало к тому, что осталось с предыдущего уровня
};



// проще работать с sequence, чем с int...
template<typename T>
void ordered_print_impl() {
    std::cout << extract_min<T>::value << " ";
    ordered_print_impl<typename extract_min<T>::remaining>();
}
template<>
void ordered_print_impl<Sequence<>>() {}



// то, что просили
template<int... values>
void ordered_print() {
    ordered_print_impl<Sequence<values...>>();
}

int main() {
    //constexpr function deprecated 
    ordered_print<5, 3, 4, 1, 2>(); // 1 2 3 4 5

}
