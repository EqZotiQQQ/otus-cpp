#include <iostream>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

/**
Просто создать такую штуку нельзя:
template <typename T, typename Container>
struct SimpleAdapter1 {
    Container<T> ctr;

    void append(const T& value) {
        ctr.push_back(value);
    }
};
*/

/**
Шаблон, принимающий другой класс, который является шаблонным.
Подойдёт, если нужно использовать что-то типа адаптера или что-то ещё
*/
template <typename T, template <typename> class Container>
struct SimpleAdapter {
    using ContainerType = Container<T>;

    ContainerType ctr;

    void append(const T& value) {
        ctr.push_back(value);
    }
};

/**
Вариадик шаблон, принимающий другой класс, который является шаблонным.
Подойдёт, если нужно использовать что-то типа адаптера или что-то ещё, если есть сколько угодно параметров шаблона, которые можно положить
во внутренний контейнер.
*/
template <typename K, typename V, template <typename, typename...> class Container>
struct AdapterWithMultipleTemplates {
    Container<K, V> ctr;

    void insert(const K& key, V&& val) {
        ctr.emplace(std::make_pair(key, std::move(val)));
    }
};

int run_templates() {
    SimpleAdapter<int, std::vector> foo;
    foo.append(42);
    std::cout << std::format("foo: {}", foo.ctr) << std::endl;
    SimpleAdapter<int, std::list> foo1;
    foo1.append(42);
    std::cout << std::format("foo1: {}", foo1.ctr) << std::endl;

    AdapterWithMultipleTemplates<int, std::string, std::unordered_map> map;
    map.insert(42, "zhopa");
    std::cout << std::format("map: {}", map.ctr) << std::endl;
    AdapterWithMultipleTemplates<int, std::string, std::map> map1;
    map1.insert(42, "debil");
    std::cout << std::format("map1: {}", map1.ctr) << std::endl;

    return 0;
}