// 20 баллов

// базовый шаблон
template<typename ValType, ValType Value, typename... Args>
class TreeNodeImpl{
public:
    static constexpr bool is_bst_node = false;
};

// для тех, у кого есть дети
template<int Value, typename Left, typename Right>
class TreeNodeImpl<int, Value, Left, Right> {
    using ImplType = TreeNodeImpl<int, Value, Left, Right>;
    using LeftImpl = typename Left::ImplType;
    using RightImpl = typename Right::ImplType;
    static constexpr bool all_ok_here = (LeftImpl::max < Value) && (RightImpl::min > Value);
public:
    static constexpr int min = LeftImpl::min;
    static constexpr int max = RightImpl::max;
    static constexpr bool is_bst_node = all_ok_here && (LeftImpl::is_bst_node) && RightImpl::is_bst_node;
};

// для листьев
template<int Value>
class TreeNodeImpl<int, Value> {
public:
    static constexpr bool is_bst_node = true;
    static constexpr int min = Value;
    static constexpr int max = Value;
};



// дальше кусок кода, который случился только из-за того, что в шаблонный параметр типа int можно подставить char
// иначе TreeNodeImpl был бы просто TreeNode и не было бы LeftImpl с RightImpl

template<auto Value, typename... Args>
struct TreeNode {
    using ImplType = TreeNodeImpl<decltype(Value), Value, Args...>;
};



// unwrap (развертка, красиво достающая имя)
template<typename T>
constexpr bool is_bst = T::ImplType::is_bst_node;

int main() {
    static_assert(
        is_bst<
            TreeNode<5,
                TreeNode<3,
                    TreeNode<1>,
                    TreeNode<4>
                >,
                TreeNode<8,
                    TreeNode<6>,
                    TreeNode<40>
                >    
            >
        >
    ); //true


    static_assert(!
        is_bst<
            TreeNode<5,
                TreeNode<3,
                    TreeNode<1>,
                    TreeNode<40>
                >,
                TreeNode<8,
                    TreeNode<6>,
                    TreeNode<40>
                >    
            >
        >
    ); //false


    static_assert(!
        is_bst<
            TreeNode<'k',
                TreeNode<0>,
                TreeNode<108>
            >
        >
    ); // false


    static_assert(
        is_bst<
            TreeNode<50, 
                TreeNode<5,
                    TreeNode<3,
                        TreeNode<1>,
                        TreeNode<4>
                    >,
                    TreeNode<8,
                        TreeNode<6>,
                        TreeNode<40>
                    >    
                >, 
                TreeNode<500,
                    TreeNode<300,
                        TreeNode<100>,
                        TreeNode<400>
                    >,
                    TreeNode<800,
                        TreeNode<600>,
                        TreeNode<4000>
                    >    
                >
            >
        >
    ); // true
}