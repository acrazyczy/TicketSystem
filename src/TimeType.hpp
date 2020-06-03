//
// Created by hanchong on 2020/5/23.
//

#ifndef TICKET_TIMETYPE_HPP
#define TICKET_TIMETYPE_HPP

#include <iostream>

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
            timeType tmp = *this;
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
        friend std::ostream &operator<<(std::ostream &output, const timeType &obj) {
            if (obj.month < 10) {
                if (obj.day < 10) {
                    if (obj.hour < 10) {
                        if (obj.minute < 10) output << '0' << obj.month << '-' << '0' << obj.day << " 0" << obj.hour << ":0" << obj.minute;
                        else output << '0' << obj.month << '-' << '0' << obj.day << " 0" << obj.hour << ":" << obj.minute;
                    }
                    else {
                        if (obj.minute < 10) output << '0' << obj.month << '-' << '0' << obj.day << " " << obj.hour << ":0" << obj.minute;
                        else output << '0' << obj.month << '-' << '0' << obj.day << " " << obj.hour << ":" << obj.minute;
                    }
                }
                else {
                    if (obj.hour < 10) {
                        if (obj.minute < 10) output << '0' << obj.month << '-' << obj.day << " 0" << obj.hour << ":0" << obj.minute;
                        else output << '0' << obj.month << '-' <<  obj.day << " 0" << obj.hour << ":" << obj.minute;
                    }
                    else {
                        if (obj.minute < 10) output << '0' << obj.month << '-' <<  obj.day << " " << obj.hour << ":0" << obj.minute;
                        else output << '0' << obj.month << '-' <<  obj.day << " " << obj.hour << ":" << obj.minute;
                    }
                }
            }
            else {
                if (obj.day < 10) {
                    if (obj.hour < 10) {
                        if (obj.minute < 10) output << obj.month << '-' << '0' << obj.day << " 0" << obj.hour << ":0" << obj.minute;
                        else output << obj.month << '-' << '0' << obj.day << " 0" << obj.hour << ":" << obj.minute;
                    }
                    else {
                        if (obj.minute < 10) output << obj.month << '-' << '0' << obj.day << " " << obj.hour << ":0" << obj.minute;
                        else output << obj.month << '-' << '0' << obj.day << " " << obj.hour << ":" << obj.minute;
                    }
                }
                else {
                    if (obj.hour < 10) {
                        if (obj.minute < 10) output << obj.month << '-' << obj.day << " 0" << obj.hour << ":0" << obj.minute;
                        else output << obj.month << '-' <<  obj.day << " 0" << obj.hour << ":" << obj.minute;
                    }
                    else {
                        if (obj.minute < 10) output << obj.month << '-' <<  obj.day << " " << obj.hour << ":0" << obj.minute;
                        else output << obj.month << '-' <<  obj.day << " " << obj.hour << ":" << obj.minute;
                    }
                }
            }
            return output;
        }
    };
}


#endif //TICKET_TIMETYPE_HPP
