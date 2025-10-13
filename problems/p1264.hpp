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
