#include <iostream>

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
