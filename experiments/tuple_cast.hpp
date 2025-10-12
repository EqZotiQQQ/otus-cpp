// cast all elements of tuple to provided type

#include <iostream>
#include <ostream>
#include <tuple>
#include <type_traits>

template <typename x>
struct constant {
    template <typename>
    struct apply {
        using type = x;
    };
};

template <typename Tuple, template <typename> class Func>
struct transform {};

template <template <typename> class Func, typename... Ts>
struct transform<std::tuple<Ts...>, Func> {
    using type = std::tuple<typename Func<Ts>::type...>;
};

using original = std::tuple<int, float, void, double>;
using transformed = transform<original, constant<float>::apply>::type;

void cast_tuple_types() {
    transformed a{4, 1, 2, 3};
    std::cout << std::format("{}", a) << std::endl;
}

template <typename T, typename... Ts>
constexpr bool is_same_types = (std::is_same_v<T, Ts> && ...);

void compare_types_template() {
    std::cout << is_same_types<int, int, int> << '\n';     // true
    std::cout << is_same_types<int, double, int> << '\n';  // false
}

template <typename... Ts>
struct is_args_types_equal;

template <>
struct is_args_types_equal<> : std::true_type {};

template <typename T>
struct is_args_types_equal<T> : std::true_type {};

template <typename T1, typename T2, typename... Rest>
struct is_args_types_equal<T1, T2, Rest...> : std::bool_constant<std::is_same_v<T1, T2> && is_args_types_equal<T2, Rest...>::value> {};

template <typename Tuple>
struct is_tuple_args_has_same_type {};

template <typename... Ts>
struct is_tuple_args_has_same_type<std::tuple<Ts...>> : is_args_types_equal<Ts...> {};

void compare_tuple_template() {
    auto t = std::make_tuple(42, "deb", 0.5);
    std::cout << "is args same for new tuple: " << is_tuple_args_has_same_type<std::tuple<int, float, double>>::value << '\n';  // false
    std::cout << "is args same for t: " << is_tuple_args_has_same_type<decltype(t)>::value << '\n';                             // false
    std::cout << "for equal types:" << is_tuple_args_has_same_type<std::tuple<int, int, int>>::value << '\n';                   // false
}
