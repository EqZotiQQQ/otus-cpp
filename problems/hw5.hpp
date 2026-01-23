#include <algorithm>
#include <cstdint>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

int p1576() {
    int N1, C1, N2, T, C2, N3, K, minutes_wasted = 0;

    scanf("%d%d%d%d%d%d%d", &N1, &C1, &N2, &T, &C2, &N3, &K);

    std::string input;
    for (int i = 0; i < K; i++) {
        // faster approach is to use scanf with char [5], but it feels like grind for 0.001/100 kb
        std::cin >> input;
        int call_secs = std::stoi(std::string(input.cend() - 2, input.cend()));
        input.erase(input.end() - 3, input.end());
        int call_mins = std::stoi(input);
        std::cout << call_mins << ":" << call_secs << std::endl;
        minutes_wasted += (call_mins > 0 || call_secs > 6) ? (call_mins + call_secs > 6) + (call_secs > 6) : 0;
    }

    printf("Basic:     %d\n", N1 + C1 * minutes_wasted);
    printf("Combined:  %d\n", N2 + (minutes_wasted - T > 0 ? minutes_wasted - T : 0) * C2);
    printf("Unlimited: %d\n", N3);
    return 0;
}

int p1021() {
    int first_list_length, second_list_lenght;
    scanf("%d", &first_list_length);

    std::unordered_set<int16_t> incr_set;

    incr_set.reserve(first_list_length);

    int v;
    for (int i = 0; i < first_list_length; i++) {
        scanf("%i", &v);
        incr_set.insert(v);
    }

    scanf("%d", &second_list_lenght);

    bool has_required_value = false;
    for (int i = 0; i < second_list_lenght; i++) {
        scanf("%i", &v);
        if (!has_required_value && incr_set.contains(10000 - v)) {
            has_required_value = true;
        }
    }
    if (has_required_value) {
        printf("YES");
    } else {
        printf("NO");
    }

    return 0;
    // Accepted  0.062	1 824 КБ
}

int p1021_2() {
    int first_list_length, second_list_lenght;
    scanf("%d", &first_list_length);

    std::array<bool, 165536> incr_list{};

    int v;
    for (int i = 0; i < first_list_length; i++) {
        scanf("%i", &v);
        incr_list[v + 32768] = true;
    }

    scanf("%d", &second_list_lenght);

    bool has_required_value = false;
    for (int i = 0; i < second_list_lenght; i++) {
        scanf("%i", &v);
        has_required_value = incr_list[10000 - v + 32768] || has_required_value;
    }
    if (has_required_value) {
        printf("YES");
    } else {
        printf("NO");
    }

    return 0;
    // Accepted	0.062	284 КБ
}

int p1306() {
    int n;
    scanf("%d", &n);
    std::priority_queue<uint32_t> vec;
    uint32_t v;

    for (int i = 0; i < n; i++) {
        scanf("%d", &v);
        vec.push(v);
        if (i > n / 2) {
            vec.pop();
        }
    }
    if (n == 1) {
        printf("%u", v);
        return 0;
    }
    uint32_t ret1 = vec.top();

    vec.pop();

    uint32_t ret2 = vec.top();

    if (n % 2 == 0) {
        printf("%.1f", (ret1 / 2. + ret2 / 2.));
    } else {
        printf("%u", ret1);
    }
    return 0;
    // Accepted	0.203	988 КБ
}
