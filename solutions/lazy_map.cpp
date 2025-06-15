#include <iostream>
#include <optional>
#include <functional>
#include <tuple>
#include <string>
#include <unordered_map>
#include <type_traits>

template<typename RetType, typename... Args>
struct FuncHolder {
public:
    FuncHolder() = default;
    ~FuncHolder() noexcept = default;

    using func_type = std::function<RetType(Args...)>;

    FuncHolder(const FuncHolder& other) 
    requires (std::is_copy_constructible_v<func_type>) && (std::is_copy_constructible_v<Args> && ...)
     : function_(other.function_), args_(other.args_)
    {}

    FuncHolder(FuncHolder&& other) 
    requires (std::is_move_constructible_v<func_type>) && (std::is_move_constructible_v<Args> && ...)
     : function_(std::move(other.function_)), args_(std::move(other.args_))
    {}

    FuncHolder& operator=(const FuncHolder& other) 
    requires (std::is_copy_assignable_v<func_type>) && (std::is_copy_assignable_v<Args> && ...)
    {
        function_ = other.function_;
    }
    FuncHolder& operator=(FuncHolder&& other) 
    requires (std::is_move_assignable_v<func_type>) && (std::is_move_assignable_v<Args> && ...)
    {
        function_ = std::exchange(other.function_, {});
    }
    
    
    template<typename T_=RetType, typename... Args_>
    FuncHolder(std::function<T_(Args_...)>&& function, Args_&&... args)
     : function_(std::forward(function)), args_(std::forward(args)...)
    {}
    
    FuncHolder& FromTuple(const std::tuple<func_type, Args...>& tuple) 
    {
        function_ = std::get<0> (tuple);
        args_ = std::make_tuple(std::get<Args> (tuple)...);
        return *this;
    }

    operator RetType() 
    {
        if (!function_) { throw std::runtime_error("no function set\n"); }
        if (!value_) { value_ = std::apply(*function_, args_); }
        return *value_;
    }

    FuncHolder& operator=(const func_type& function) {
        function_ = function;
        value_ = std::nullopt;
        return *this;
    }

    void SetArgs(const Args&... args) {
        args_ = std::make_tuple(args...);
    }   
private:
    std::tuple<Args...> args_;
    std::optional<std::function<RetType(Args...)>> function_;
    std::optional<RetType> value_;
};


// std::any and std::variant deprecated
template<typename RetType, typename KeyType, typename... Args>
class LazyEvaluatedMap {
public:
    LazyEvaluatedMap() = default;
    ~LazyEvaluatedMap() noexcept = default;

    LazyEvaluatedMap(const LazyEvaluatedMap& other) 
    requires (std::is_copy_constructible_v<RetType>) && (std::is_copy_constructible_v<KeyType>) && (std::is_copy_constructible_v<Args> && ...)
     : funcs_(other.funcs_)
    { std::cout << "copied\n";} // just for tests

    LazyEvaluatedMap(LazyEvaluatedMap&& other) 
    requires (std::is_move_constructible_v<RetType>) && (std::is_move_constructible_v<KeyType>) && (std::is_move_constructible_v<Args> && ...)
     : funcs_(std::move(other.funcs_))
    { std::cout << "moved\n";} // just for tests

    LazyEvaluatedMap& operator=(const LazyEvaluatedMap& other) 
    requires (std::is_copy_assignable_v<RetType>) && (std::is_copy_assignable_v<KeyType>) && (std::is_copy_assignable_v<Args> && ...)
    {
        funcs_ = other.funcs_;
    }
    LazyEvaluatedMap& operator=(LazyEvaluatedMap&& other) 
    requires (std::is_move_assignable_v<RetType>) && (std::is_move_assignable_v<KeyType>) && (std::is_move_assignable_v<Args> && ...)
    {
        funcs_ = std::exchange(other.funcs_, {});
    }

    template<typename KT=KeyType>
    FuncHolder<RetType, Args...>& operator[](KT&& ind) {
        return funcs_[std::forward<KT>(ind)];
    }
private:
    std::unordered_map<KeyType, FuncHolder<RetType, Args...>> funcs_;
};

void MyTestArgs();
void MyTestArgsTuple();
void MyTestCopyMove();

int main() {
    LazyEvaluatedMap<int, const char*> m;

    m["x"] = [](){ std::cout << "x "; return 2; };
    m["y"] = [](){ std::cout << "y "; return 3; };

    std::cout << m["x"] << '\n'; // x 2
    std::cout << m["x"] << '\n'; // 2
    
    std::cout << m["y"] << '\n'; // y 3
    std::cout << m["y"] << '\n'; // 3

    m[std::move("x")]; // дописали на защите

    MyTestArgs();
    MyTestArgsTuple();
    MyTestCopyMove();
    return 0;
}

void MyTestArgs() {
    LazyEvaluatedMap<int, const char*, int> m;
    int valx = 10;
    int valy = 11;
    m["x"] = [&](int v){ std::cout << "x val: " << v << " "; return 2; };
    m["y"] = [&](int v){ std::cout << "y val: " << v << " "; return 3; };

    m["x"].SetArgs(valx);
    m["y"].SetArgs(valy);

    std::cout << m["x"] << '\n'; // x 2
    std::cout << m["x"] << '\n'; // 2
    
    std::cout << m["y"] << '\n'; // y 3
    std::cout << m["y"] << '\n'; // 3
}

void MyTestArgsTuple() {
    LazyEvaluatedMap<int, const char*, int, char> m;

    int valx = 100;
    int valy = 101;

    char cx = 'a';
    char cy = 'b';

    m["x"].FromTuple(std::tuple{[&](int v, char c){ std::cout << "x val: " << v << " char: " << c << " "; return 2; }, valx, cx});
    m["y"].FromTuple(std::tuple{[&](int v, char c){ std::cout << "y val: " << v << " char: " << c << " "; return 3; }, valy, cy});

    std::cout << m["x"] << '\n'; // x 2
    std::cout << m["x"] << '\n'; // 2
    
    std::cout << m["y"] << '\n'; // y 3
    std::cout << m["y"] << '\n'; // 3
    
    try {
        std::cout << m["z"] << '\n'; // 3
    }
    catch (const std::exception& ex) {
        std::cout << "caught: " << ex.what();
    }
}

struct Movable {
    Movable(Movable&&) = default;
    Movable& operator=(Movable&&) = default;
    Movable(const Movable&) = delete;
    Movable& operator=(const Movable&) = delete;
};
struct Copyable {
    Copyable(Copyable&&) = delete;
    Copyable& operator=(Copyable&&) = delete;
    Copyable(const Copyable&) = default;
    Copyable& operator=(const Copyable&) = default;
};



void MyTestCopyMove() {
    LazyEvaluatedMap<Movable, const char*, int, char> m_Movable;
    // LazyEvaluatedMap<Movable, const char*, int, char> m_Movable_2 = m_Movable; // should be compile-error
    std::cout << "expected move: ";
    LazyEvaluatedMap<Movable, const char*, int, char> m_Movable_2 = std::move(m_Movable);

    LazyEvaluatedMap<Copyable, const char*, int, char> m_Copyable;
    std::cout << "expected copy: ";
    LazyEvaluatedMap<Copyable, const char*, int, char> m_Copyable_3 = m_Copyable; 
    std::cout << "expected copy: ";
    LazyEvaluatedMap<Copyable, const char*, int, char> m_Copyable_2 = std::move(m_Copyable); // should be copied instead of move
    
    LazyEvaluatedMap<int, const char*, int, char> m_int;
    std::cout << "expected copy: ";
    LazyEvaluatedMap<int, const char*, int, char> m_int_2 = m_int; 
    std::cout << "expected move: ";
    LazyEvaluatedMap<int, const char*, int, char> m_int_3 = std::move(m_int); 
}