//
// Created by hanchong on 2020/5/23.
//

#ifndef TICKET_TIMETYPE_HPP
#define TICKET_TIMETYPE_HPP

#include <iostream>

class timeType {
private:

    static constexpr int months[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

public:
    int month;
    int day;
    int hour;
    int minute;
    timeType() {
        month = day = hour = minute = 0;
    }
    timeType(std::string d) {
        month = (d[0] - '0') * 10 + (d[1] - '0');
        day = (d[3] - '0') * 10 + (d[4] - '0');
        hour = minute = 0;
    }
    timeType(int m, int d, int h, int min) {
        month = m;
        day = d;
        hour = h;
        minute = min;
    }
    timeType(const timeType &other) {
        month = other.month;
        day = other.day;
        hour = other.hour;
        minute = other.minute;
    }
    timeType operator+(const int &time) {
        TimeType tmp = *this;
        tmp.minute += time;
        while (tmp.minute >= 60) {
            tmp.minute -= 60;
            ++tmp.hour;
            while (tmp.hour >= 24) {
                tmp.hour -= 24;
                ++tmp.day;
                while (tmp.day >= months[tmp.month]) {
                    tmp.day -= months[tmp.month];
                    ++tmp.month;
                }
            }
        }
        return tmp;
    }
};

#endif //TICKET_TIMETYPE_HPP
