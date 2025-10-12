#include <iostream>
#include <vector>

struct add_pointer {
    template <typename T>
    struct apply {
        using type = T*;
    };
};

template <typename T>
struct Wrapper {
    using rest = typename add_pointer::template apply<T>::type;
};

template <typename T>
using WrapperT = Wrapper<T>::rest;

void list_experiment() {
    int foo = 42;
    WrapperT<int> a = &foo;
    std::cout << *a << std::endl;
}

/**
add_pointer - контейнер для метафункции
apply - метафункция, производящая операцию превращения Т в Т*
typename add_pointer::template apply<T>::type - это вызов метафункции add_pointer, а именно apply.
WrapperT - просто удобная хуйня, чтобы скрыть ::rest

С точки зрения комплиятора:
При встрече Wrapper<int>:
подставляет T = int;
раскрывает rest = typename add_pointer::template apply<int>::type;
видит add_pointer::apply<int>::type = int*;
значит rest = int*.
В main:
Wrapper<int>::rest → int*;
переменная ptr имеет тип int*.
*/