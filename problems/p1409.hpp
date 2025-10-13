#include <iostream>

void p1409() {
    int harry_init, larry_init;
    scanf("%d%d", &harry_init, &larry_init);
    int total_bottles = harry_init + larry_init - 1;
    printf("%d %d\n", total_bottles - harry_init, total_bottles - larry_init);
}