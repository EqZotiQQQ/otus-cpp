#include <iostream>
#include <vector>

template <typename head, template <typename> class get_rest>
struct List {
    using first = head;
    using rest = typename get_rest<head>::type;
};

template <typename L>
using car = typename L::first;

template <typename L>
using cdr = typename L::rest;

template <typename x>
struct constant {
    template <typename>
    struct apply {
        using type = x;
    };
};

template <typename x, typename l>
using cons = List<x, constant<l>::template apply>;

// по аналогии, это возвращает константу:
// auto always_five = [](auto) { return 5; };
/**
using always_int = constant<int>; - запомнил int

using A = always_int::apply<double>::type; - всё равно будет интом
using B = always_int::apply<std::string>::type; - всё равно будет интом

static_assert(std::is_same_v<A, int>); # 1
static_assert(std::is_same_v<B, int>); # 1
*/

void list_experiment() {
    // std::tuple<int, int> a;
}
