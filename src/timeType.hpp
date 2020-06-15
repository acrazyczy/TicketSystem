//
// Created by hanchong on 2020/5/23.
//

#ifndef TICKET_TIMETYPE_HPP
#define TICKET_TIMETYPE_HPP

#include <iostream>
#include <cstring>
#include <string>

namespace sjtu {
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
            if (d[2] == '-') {
                month = (d[0] - '0') * 10 + (d[1] - '0');
                day = (d[3] - '0') * 10 + (d[4] - '0');
                hour = minute = 0;
            }
            else if (d[2] == ':') {
                hour = (d[0] - '0') * 10 + (d[1] - '0');
                minute = (d[3] - '0') * 10 + (d[4] - '0');
                month = day = 0;
            }
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
        timeType operator+(const int &) const;
        int operator-(const timeType &) const;

        static int dateminus(const timeType & , const timeType &);

        bool operator<(const timeType &) const;

        friend std::ostream &operator<<(std::ostream &, const timeType &);
    };
}


#endif //TICKET_TIMETYPE_HPP
