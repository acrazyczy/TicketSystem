//
// Created by hanchong on 2020/5/23.
//

#ifndef TICKET_TIMETYPE_HPP
#define TICKET_TIMETYPE_HPP

#include <iostream>

class TimeType {
private:

    static constexpr int months[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

public:
    int month;
    int day;
    int hour;
    int minute;
    TimeType() {
        month = day = hour = minute = 0;
    }
    TimeType(int m, int d, int h, int min) {
        month = m;
        day = d;
        hour = h;
        minute = min;
    }
    TimeType(const TimeType &other) {
        month = other.month;
        day = other.day;
        hour = other.hour;
        minute = other.minute;
    }
    TimeType operator+(const int &time) {
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
