#pragma once

#include <iostream>

int p1264_impl(int m, int n) {
    return m * (n + 1);
}

void p1264() {
    int a, b;
    scanf("%d%d", &a, &b);
    printf("%d\n", p1264_impl(a, b));
}

void p1409() {
    int harry_init, larry_init;
    scanf("%d%d", &harry_init, &larry_init);
    int total_bottles = harry_init + larry_init - 1;
    printf("%d %d\n", total_bottles - harry_init, total_bottles - larry_init);
}

void p1787() {
    int number_of_cars_per_minute, watch_minuts;
    scanf("%d%d", &number_of_cars_per_minute, &watch_minuts);
    int cars_in_queue = 0;
    for (int i = 0; i < watch_minuts; i++) {
        int new_cars_in_tick;
        scanf("%d", &new_cars_in_tick);
        cars_in_queue += new_cars_in_tick;
        cars_in_queue = cars_in_queue - number_of_cars_per_minute > 0 ? cars_in_queue - number_of_cars_per_minute : 0;
    }

    printf("%d\n", cars_in_queue);
}

void p1877() {
    char first_code[5];
    char second_code[5];

    scanf("%4s %4s", first_code, second_code);

    size_t pos = 3;
    bool first_code_int = std::stoi(first_code, &pos) % 2 == 0;
    bool second_code_int = std::stoi(second_code, &pos) % 2;

    if (first_code_int || second_code_int) {
        printf("yes\n");
    } else {
        printf("no\n");
    }

    return;
}

int p2001() {
    int a1, b1, a2_full, b2_empty, a3_empty, b3_full;
    scanf("%d%d%d%d%d%d", &a1, &b1, &a2_full, &b2_empty, &a3_empty, &b3_full);
    int a_weight = a1 - a3_empty;
    int b_weight = b1 - b2_empty;

    printf("%d %d\n", a_weight, b_weight);

    return 0;
}
