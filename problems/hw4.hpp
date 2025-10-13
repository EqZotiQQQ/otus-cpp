#include <cmath>
#include <iostream>
#include <vector>

using ull = unsigned long long;
using ll = long long;

int p1001() {
    std::vector<ull> values;
    ull value;
    while (scanf("%llu", &value) != EOF) {
        values.push_back(value);
    }

    for (ll i = values.size() - 1; i >= 0; i--) {
#ifndef ONLINE_JUDGE
        // printf("m=%llu\n", value);
#endif
        double reverse_sqrt = std::sqrt(values[i]);
        printf("%.4f\n", reverse_sqrt);
    }

    return 0;
}

int p1293() {
    int n, a, b;
    scanf("%d%d%d", &n, &a, &b);
    printf("%d", 2 * n * a * b);
    return 0;
}

int p1000() {
    int a, b;
    scanf("%d%d", &a, &b);
    printf("%d", a + b);
    return 0;
}

int p1785() {
    int input;
    scanf("%d", &input);
    if (input < 5) {
        printf("few");
    } else if (input < 10) {
        printf("several");
    } else if (input < 20) {
        printf("pack");
    } else if (input < 50) {
        printf("lots");
    } else if (input < 100) {
        printf("horde");
    } else if (input < 250) {
        printf("throng");
    } else if (input < 500) {
        printf("swarm");
    } else if (input < 1000) {
        printf("zounds");
    } else {
        printf("legion");
    }
    return 0;
}

int p2012() {
    int problems_solved_per_first_hour;
    scanf("%d", &problems_solved_per_first_hour);
    printf(problems_solved_per_first_hour > 6 ? "yes" : "no");
    return 0;
}
