#include <iostream>

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