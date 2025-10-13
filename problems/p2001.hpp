#include <iostream>

int p2001() {
    int a1, b1, a2_full, b2_empty, a3_empty, b3_full;
    scanf("%d%d%d%d%d%d", &a1, &b1, &a2_full, &b2_empty, &a3_empty, &b3_full);
    int a_weight = a1 - a3_empty;
    int b_weight = b1 - b2_empty;

    printf("%d %d\n", a_weight, b_weight);

    return 0;
}