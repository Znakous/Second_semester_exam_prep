#include <iostream>
#include <type_traits>

namespace {

/**
 * 1. Define Vector, a template level list of integers.
 * Hint: Use non-type template parameter pack.
 */
template <int... values>
struct Vector {};


template<int a, int... others>
struct Vector<a, others...> {
    static constexpr int length = Vector<others...>::length + 1;
};

template <>
struct Vector<> {
    static constexpr int length = 0;
};



// ^ Your code goes here

static_assert(std::is_same_v<Vector<1,2>, Vector<1,2>>);


/**
 * 2. Define function print() that prints Vector-s.
 * Example: print(Vector<1,2,3>{}); // prints "1 2 3" (newline)
 * See main() below.
 */
template<int... a>
void print(Vector<a...>) {}

template<int tp, int... others>
void print(Vector<tp, others...>) {
    std::cout << tp << " ";
    print(Vector<others...>());
}

template<>
void print(Vector<>) {
    std::cout << "\n";
}


// ^ Your code goes here


/**
 * 3. Define Prepend.
 * Hint: Use `using type = ...` inside a struct that has both non-type and type template parameters.
 */

template<int val, typename Vec>
struct Prepend{};

template<int a, int... others>
struct Prepend<a, Vector<others...>> {
    using type = Vector<a, others...>;
};

// ^ Your code goes here

static_assert(std::is_same_v<Prepend<1, Vector<2,3>>::type, Vector<1,2,3>>);


/**
 * 4. Define PrependT that can be used without ::type.
 * Hint: See how enable_if_t is defined in terms of enable_if.
 *
 * This technique is not used further to reduce boilerplate.
 */
template<int val, typename T>
using PrependT = Prepend<val, T>::type;

// ^ Your code goes here

static_assert(std::is_same_v<PrependT<1, Vector<2,3>>, Vector<1,2,3>>);


/**
 * 5. Define Append.
 */

template<int val, typename Vec>
struct Append{};

template<int a, int... others>
struct Append<a, Vector<others...>> {
    using type = Vector<others..., a>;
};

// ^ Your code goes here

static_assert(std::is_same_v< Append<4, Vector<1,2,3>>::type , Vector<1,2,3,4> >);


/**
 * 6. Define PopBack.
 */


// ^ Your code goes here

template<typename>
struct PopBack{};

template<int V, int V1, int... others>
struct PopBack<Vector<V, V1, others...>> {
    using type = PrependT<V, typename PopBack<Vector<V1,  others...>>::type>;
};

template<int V, int... o>
struct PopBack<Vector<V, o...>> {
    using type = Vector<>;
};

static_assert(std::is_same_v< PopBack<Vector<1, 2>>::type , Vector<1> >);


/**
 * 7. Define RemoveFirst, that removes the first occurence of element R from vector V.
 */


template<int a, typename>
struct RemoveFirst{};

template<int a, int... vals>
struct RemoveFirst<a, Vector<a, vals...>> {
    using type = Vector<vals...>;
};

template<int a, int wr, int... vals>
struct RemoveFirst<a, Vector<wr, vals...>> {
    using type = PrependT<wr, typename RemoveFirst<a, Vector<vals...>>::type>;
};

template<int a>
struct RemoveFirst<a, Vector<>> {
    using type = Vector<>;
};

// ^ Your code goes here

static_assert(std::is_same_v<RemoveFirst<1, Vector<1,1,2>>::type, Vector<1,2>>);


/**
 * 8. Define RemoveAll, that removes all occurences of element R from vector V.
 */

template<int a, typename T>
struct RemoveAll {};

template<int a, int... others>
struct RemoveAll<a, Vector<a, others...>> {
    using type = typename RemoveAll<a, Vector<others...>>::type;
};

template<int a, int b, int... others>
struct RemoveAll<a, Vector<b, others...>> {
    using type = PrependT<b, typename RemoveAll<a, Vector<others...>>::type>;
};
template<int a, int... others>
struct RemoveAll<a, Vector<others...>> {
    using type = Vector<others...>;
};

// ^ Your code goes here

static_assert(std::is_same_v<RemoveAll<9, Vector<1,9,2,9,3,9>>::type, Vector<1,2,3>>);


/**
 * 9. Define Length.
 * Hint: Use `static constexpr int value = ...` inside the struct.
 */

template<typename T>
struct Length {
    static constexpr int value = T::length;
};

// ^ Your code goes here

static_assert(Length<Vector<1,2,3>>::value == 3);


/**
 * 10. Define length, which works like Length<V>::value.
 * Hint: See how is_same_v is defined in terms of is_same.
 */


// ^ Your code goes here

template<typename T>
constexpr int length = Length<T>::value;

static_assert(length<Vector<>> == 0);
static_assert(length<Vector<1,2,3>> == 3);


/**
 * 11. Define Min, that stores the minimum of a vector in its property `value`.
 */

template<typename T>
struct Min{};

template<int a>
struct Min<Vector<a>> {
    static constexpr int value = a;
};

template<int a, int... others>
struct Min<Vector<a, others...>> {
    static constexpr int value = (a < Min<Vector<others...>>::value) ? (a) : (Min<Vector<others...>>::value);
};



// ^ Your code goes here

static_assert(Min<Vector<3,1,2>>::value == 1);
static_assert(Min<Vector<1,2,3>>::value == 1);
static_assert(Min<Vector<3,2,1>>::value == 1);


/**
 * 12. Define Sort.
 */

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
struct SepMin {};

template<int a>
struct SepMin <Vector<a>> {
    using this_type = Vector<a>;
    using without = Vector<>;
    static constexpr int min_val = a;
    static constexpr int size = 1;
};
template<>
struct SepMin <Vector<>> {
    using this_type = Vector<>;
    using without = Vector<>;
    static constexpr int min_val = 1e9;
    static constexpr int size = 0;
};

template<int a, int... others>
struct SepMin <Vector<a, others...>> {
    static constexpr int size = Vector<others...>::size;
    using other_sep = SepMin<Vector<others...>>;
    static constexpr int min_val = (a < other_sep::min_val) ? a : other_sep::min_val;
    using this_type = Vector<a, others...>;
    using without = conditional<a < other_sep::min_val, typename other_sep::this_type, PrependT<a, typename other_sep::without>>::type;
};

template<typename T>
struct Sort{};

template<int... others>
struct Sort <Vector<others...>> {
    using sep_min = SepMin<Vector<others...>>;
    using type = PrependT<sep_min::min_val, typename Sort<typename sep_min::without>::type>;
};
template<int a>
struct Sort <Vector<a>> {
    using type = Vector<a>;
};
template<>
struct Sort <Vector<>> {
    using type = Vector<>;
};

// ^ Your code goes here

static_assert(std::is_same_v<Sort<Vector<>>::type, Vector<>>);
static_assert(std::is_same_v<Sort<Vector<2>>::type, Vector<2>>);
static_assert(std::is_same_v<Sort<Vector<4,1,2,5,6,3>>::type, Vector<1,2,3,4,5,6>>);
static_assert(std::is_same_v<Sort<Vector<3,3,1,1,2,2>>::type, Vector<1,1,2,2,3,3>>);
static_assert(std::is_same_v<Sort<Vector<2,2,1,1,3,3>>::type, Vector<1,1,2,2,3,3>>);


/**
 * 13. Define Uniq.
 */


template<typename T>
struct UniqImpl{};

template<int a, int... others>
struct UniqImpl<Vector<a, a, others...>> {
    using type = UniqImpl<Vector<a, others...>>::type;
};
template<int a, int b, int... others>
struct UniqImpl<Vector<a, b, others...>> {
    using type = PrependT<a, typename UniqImpl<Vector<b, others...>>::type>;
};
template<int a>
struct UniqImpl<Vector<a>> {
    using type = Vector<a>;
};

template<typename T>
struct Uniq {
    using type = UniqImpl<T>::type;
};


// ^ Your code goes here

static_assert(std::is_same_v<Uniq<Vector<1,1,2,2,1,1>>::type, Vector<1,2,1>>);


/**
 * 14. Define type Set.
 */

template<int... others>
struct Set {
    using type = UniqImpl<typename Sort<Vector<others...>>::type>::type;
};

// ^ Your code goes here

static_assert(std::is_same_v<Set<2,1,3,1,2,3>::type, Set<1,2,3>::type>);


/**
 * 15. Define SetFrom.
 */
template<typename T>
struct SetFrom {
    using type = UniqImpl<typename Sort<T>::type>::type;
};

// ^ Your code goes here

static_assert(std::is_same_v<SetFrom<Vector<2,1,3,1,2,3>>::type, Set<1,2,3>::type>);


/**
 * 16. Define Get for Vector.
 * Provide an improved error message when accessing outside of Vector bounds.
 */
struct OutOfBounds{};

template<int ind, typename T>
struct Get{};

template<int ind, int a, int... vals>
struct Get<ind, Vector<a, vals...>> {
    static constexpr int value = Get<ind-1, Vector<vals...>>::value;
};
template<int a, int... vals>
struct Get<0, Vector<a, vals...>> {
    static constexpr int value = a;
};

template<int ind>
struct Get<ind, Vector<>> {};



// ^ Your code goes here

static_assert(Get<0, Vector<0,1,2>>::value == 0);
static_assert(Get<1, Vector<0,1,2>>::value == 1);
static_assert(Get<2, Vector<0,1,2>>::value == 2);
// static_assert(Get<9, Vector<0,1,2>>::value == 2); // How good is your error message?


/**
 * 17. Define BisectLeft for Vector.
 * Given n and arr, return the first index i such that arr[i] >= n.
 * If it doesn't exist, return the length of the array.
 *
 * Don't worry about complexity but aim for the binary search pattern.
 *
 * Hint: You might find it convenient to define a constexpr helper function.
 */

template<bool expr, int T, int F>
struct condit_int {};

template<int T, int F>
struct condit_int<true, T, F> {
    static constexpr int value = T;
};

template<int T, int F>
struct condit_int<false, T, F> {
    static constexpr int value = F;
};

template<int cur_ind, int val, typename T>
struct BisectLeftImpl {};

template<int cur_ind, int val, int a, int... others>
struct BisectLeftImpl<cur_ind, val, Vector<a, others...>> {
    static constexpr int value = condit_int<(a >= val), cur_ind, BisectLeftImpl<cur_ind + 1, val, Vector<others...>>::value>::value;
};
template<int cur_ind, int val>
struct BisectLeftImpl<cur_ind, val, Vector<>> {
    static constexpr int value = cur_ind;
};

template<int val, typename T>
struct BisectLeft {
    static constexpr int value = BisectLeftImpl<0, val, T>::value;
};

// ^ Your code goes here

static_assert(BisectLeft<  3, Vector<0,1,2,3,4>>::value == 3);
static_assert(BisectLeft<  3, Vector<0,1,2,4,5>>::value == 3);
static_assert(BisectLeft<  9, Vector<0,1,2,4,5>>::value == 5);
static_assert(BisectLeft< -1, Vector<0,1,2,4,5>>::value == 0);
static_assert(BisectLeft<  2, Vector<0,2,2,2,2,2>>::value == 1);


/**
 * 18. Define Insert for Vector, it should take position, value and vector.
 * Don't worry about bounds.
 * Hint: use the enable_if trick, e.g.
 *   template<typename X, typename Enable = void> struct Foo;
 *   template<typename X> struct<std::enable_if_t<..something      about X..>> Foo {...};
 *   template<typename X> struct<std::enable_if_t<..something else about X..>> Foo {...};
 */

template<int ind, int val, typename T>
struct Insert {};

template<int ind, int val, int a, int... others>
struct Insert<ind, val, Vector<a, others...>> {
    using type = PrependT<a, typename Insert<ind-1, val, Vector<others...>>::type>;
};
template<int val, int a, int... others>
struct Insert<0, val, Vector<a, others...>> {
    using type = PrependT<val, Vector<a, others...>>;
};

template<int val>
struct Insert<0, val, Vector<>> {
    using type = PrependT<val, Vector<>>;
};


static_assert(std::is_same_v<Insert<0, 3, Vector<4,5,6>>::type, Vector<3,4,5,6>>);
static_assert(std::is_same_v<Insert<1, 3, Vector<4,5,6>>::type, Vector<4,3,5,6>>);
static_assert(std::is_same_v<Insert<2, 3, Vector<4,5,6>>::type, Vector<4,5,3,6>>);
static_assert(std::is_same_v<Insert<3, 3, Vector<4,5,6>>::type, Vector<4,5,6,3>>);

}

int main()
{
    print(Vector<>{});
    print(Vector<1>{});
    print(Vector<1,2,3,4,5,6>{});
    // std::cout << typeid(Vector<1,2,3,4,5,6>{}).name() << '\n';
}