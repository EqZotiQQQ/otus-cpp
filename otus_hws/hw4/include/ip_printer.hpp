#pragma once

#include <iostream>
#include <list>
#include <string>
#include <vector>

// template <typename... Args>
// std::vector<std::string> split(Args... args) {
// }

/////// Tuple traits

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

template <typename Container, typename = void>
struct is_tuple_type : std::false_type {};

template <typename... Ts>
struct is_tuple_type<std::tuple<Ts...>> : std::true_type {};

/**
 * @brief prints tuple where elements are splitted by '.'
 *
 * @tparam Tuple
 * @param tuple
 * @return std::enable_if_t<is_tuple_type<Tuple>::value && is_tuple_args_has_same_type<Tuple>::value, void>
 */
template <typename Tuple>
std::enable_if_t<is_tuple_type<Tuple>::value && is_tuple_args_has_same_type<Tuple>::value, void> print_ip(Tuple tuple) {
    std::apply(
        [](const auto&... args) {
            size_t n = 0;
            ((std::cout << args << (++n < sizeof...(args) ? "." : "")), ...);
            std::cout << std::endl;
        },
        tuple);
}

/////// String traits

template <typename Container, typename = void>
struct is_string_t : std::false_type {};

template <typename Container>
struct is_string_t<Container, std::enable_if_t<std::is_same_v<Container, std::string>>> : std::true_type {};

template <typename Container>
std::enable_if_t<is_string_t<Container>::value, void> print_ip(const Container& value) {
    std::cout << value << std::endl;
}

/////// Vector/list traits

template <typename Container, typename = void>
struct is_vec_list_type : std::false_type {};

template <typename T, typename Alloc>
struct is_vec_list_type<std::vector<T, Alloc>> : std::true_type {};

template <typename T, typename Alloc>
struct is_vec_list_type<std::list<T, Alloc>> : std::true_type {};

template <typename Container>
std::enable_if_t<is_vec_list_type<Container>::value && !is_string_t<Container>::value, void> print_ip(const Container& value) {
    auto iter_end = value.cend();
    iter_end--;
    for (auto iter = value.cbegin(); iter != value.cend(); iter++) {
        std::cout << *iter;
        if (iter != iter_end) {
            std::cout << ".";
        }
    }
    std::cout << std::endl;
}

/////// Integer traits

template <typename T>
std::enable_if_t<std::is_integral_v<T>, void> print_ip(const T& value) {
    for (int i = sizeof(value) - 1; i >= 0; i--) {
        std::cout << static_cast<int>(static_cast<uint8_t>(value >> (i * 8)));
        if (i > 0) {
            std::cout << ".";
        }
    }
    std::cout << std::endl;
}

// namespace with_string_exclude_template {

// template <typename Container, typename = void>
// struct is_vector_type : std::false_type {};

// template <typename T, typename Alloc, template <typename, typename...> class Container>
// struct is_vector_type<Container<T, Alloc>> : std::true_type {};

// template <typename Container>
// std::enable_if_t<is_vector_type<Container>::value && !is_string_t<Container>::value, void> print_ip(const Container& value) {
//     auto iter_end = value.cend();
//     iter_end--;
//     for (auto iter = value.cbegin(); iter != value.cend(); iter++) {
//         std::cout << *iter;
//         if (iter != iter_end) {
//             std::cout << ".";
//         }
//     }
//     std::cout << std::endl;
// }
// }  // namespace with_string_exclude_template

// // string case
// template <typename T>
// std::enable_if_t<std::is_same_v<std::remove_cv_t<T>, std::string>, void> print_ip([[maybe_unused]] const T& value) {
//     std::cout << value << std::endl;
// }

// namespace old {
// template <typename T>
// // std::enable_if_t<!std::is_same_v<std::remove_cv_t<T>, std::string>, void> print_ip(const T&
// // value) {
// // auto print_ip(const T& value) -> decltype(value.emplace(42), void()) {
// auto print_ip(const T& value) -> std::enable_if_t<!std::is_same_v<std::remove_cv_t<T>, std::string> && !std::is_integral_v<T>, void> {
//     auto iter_end = value.cend();
//     iter_end--;
//     for (auto iter = value.cbegin(); iter != value.cend(); iter++) {
//         std::cout << *iter;
//         if (iter != iter_end) {
//             std::cout << ".";
//         }
//     }
//     std::cout << std::endl;
// }

// template <typename T>
// std::enable_if_t<std::is_integral_v<T>, void> print_ip([[maybe_unused]] const T& value) {
//     for (int i = sizeof(value) - 1; i >= 0; i--) {
//         std::cout << static_cast<int>(static_cast<uint8_t>(value >> (i * 8)));
//         if (i > 0) {
//             std::cout << ".";
//         }
//     }
//     std::cout << std::endl;
// }
// }  // namespace old