#pragma once
#include <cmath>
#include <iostream>
#include <queue>
#include <set>

std::string multiply(std::string a, std::string b) {
    int n = a.size(), m = b.size();
    std::vector<int> res(n + m, 0);

    for (int i = n - 1; i >= 0; --i)
        for (int j = m - 1; j >= 0; --j)
            res[i + j + 1] += (a[i] - '0') * (b[j] - '0');

    for (int i = n + m - 1; i > 0; --i) {
        res[i - 1] += res[i] / 10;
        res[i] %= 10;
    }

    std::string s;
    int i = (res[0] == 0);
    for (; i < n + m; ++i)
        s += res[i] + '0';
    return s;
}

int p1295() {
    int n;
    scanf("%d", &n);
    int zeros = 0;
    unsigned long long res = 1;
    for (int i = 2; i < 5; i++) {
        unsigned long long sub_res = i;
        for (int j = 2; j < n + 1; j++) {
            sub_res = (sub_res * i);
        }
        res += sub_res;
    }

    std::string s = std::to_string(res);
    
    for (auto it_back = --s.cend(); it_back != s.cbegin(); it_back--) {
        if (*it_back != '0') {
            break;
        }
        zeros++;
    }
    std::printf("%llu %d\n", res, zeros);

    return 0;
}

int p1017() {

    /**
    
    5:
    ****
    *

    ***
    **

    6:
    *****
    *

    ****
    **

    ***
    **
    *

    7:
    ...
    */

    using ull = unsigned long long;

    int n;
    scanf("%d", &n);

    ull ret = 0;

    return 0;
}

int p1026() {
    int write_data_count;
    scanf("%d", &write_data_count);
    std::multiset<int> data;
    std::vector<int> v;

    int input_value;
    for (int i = 0; i < write_data_count; i++) {
        scanf("%d", &input_value);
        data.insert(input_value);
    }

    char delim_active[3];
    scanf("%s", delim_active);
    // std::print("delim: {}\n", delim_active);

    int requests_count;
    scanf("%d", &requests_count);

    int request_key;
    for (int i = 0; i < requests_count; i++) {
        // std::print("Next value... ");
        scanf("%d", &request_key);
        // std::print("Entered: {}", request_key);
        
        auto iter = data.cbegin();
        int j = 1;
        for (; iter != data.cend() && j < request_key; iter++, j++) {}
        printf("%d\n", *iter);
    }
    return 0;
}


void dbg(std::vector<bool> s) {
    std::string ss;
    for (const auto& i: s) {
        ss.push_back(i?'>':'<');
    }
    std::print("{}\n", ss);
}

void bad_impl(std::vector<bool>& s) {

    std::set<std::vector<bool>> mem;
    bool cycle = false;
    int cycle_count = 0;
    
    bool lb_face = false;
    bool rb_face = false;

    int lb = 0;
    int rb = s.size() - 1;

    // std::print("initial: {}: {}-{}\n", s, lb, rb);

    for (int i = lb; i < rb && !lb_face; i++) {
        if (!s[i]) {
            lb++;
        } else {
            break;
        }
    }

    for (int i = rb; i > lb && !rb_face; i--) {
        if (s[i]) {
            rb--;
        } else {
            break;
        }
    }
    // std::print("{}: {}-{}\n", s, lb, rb);

    while (true) {
        bool had_changes = false;
        for (int i = lb + 1; i <= rb; i++) {
            if (s[i - 1] && !s[i]) {
                // std::print("Swap {} {}: {} {}\n", s[i - 1], s[i], i - 1, i);
                // std::swap(s[i - 1], s[i]);
                s[i - 1] = !s[i - 1];
                s[i] = !s[i];
                cycle_count++;
                had_changes = true;
                i++;
            }
        }
        
        if (!had_changes) {
            break;
        }

        lb_face = false;
        rb_face = false;

        // move right righter
        // std::print("before r shift: {}: {}-{}\n", s, lb, rb);
        for (int i = rb; i >= lb && !rb_face; i--) {
            // std::print("Shift? {}>={} {}\n", rb, i, s[i]);
            if (s[i]) {
                // std::cout << "shift rb" << std::endl;
                rb--;
            } else {
                // std::cout << "skip shift rb" << std::endl;
                break;
            }
        }

        for (int i = lb; i <= rb && !rb_face; i++) {
            if (!s[i]) {
                lb++;
            } else {
                break;
            }
        }
        // dbg(s);
        // std::print("iter end: {}: {}-{}\n\n", s, lb, rb);

    }
    // std::print("last: {}: {}-{}\n", s, lb, rb);
}

std::vector<bool> input() {
    int n;
    scanf("%d", &n);
    std::vector<bool> s;
    s.reserve(n);
    char c;
    for (int i = 0;i < n; i++) {
        scanf("%c", &c);
        if (c == '\n' || c == '\t' || c == ' ') {
            i--;
            continue;
        }
        s.push_back(c == '>');
    }
    return s;
}

int p1135() {
    std::vector<bool> s = input();
    // bad_impl(s);
    // std::vector<bool> s = {1, 1, 0, 0, 1, 0};
    //                     >  >  <  <  >  <
    //                     0  0  0  1  1  1   
    //                     <  <  <  >  >  >        

    bool changes_done = false;

    int offset_size = 0;
    for (int i = s.size() - 1; i >= 0; i--) {
        if (!s[i]) {
            // std::print("Object {} is false\n", i);
            for (int j = i - 1; j >= 0; j--) {
                if (s[j]) {
                    s[i] = true;
                    s[j] = false;
                    offset_size += i - j;
                    break;
                }
            }
        }
    }

    printf("%d\n", offset_size);
    
    return 0;
}
