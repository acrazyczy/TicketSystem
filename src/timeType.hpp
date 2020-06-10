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
        timeType operator+(const int &time) {
            if (time >= 0)
            {
                minute += time , hour += minute / 60 , minute %= 60;
                int day_add = hour / 24;hour %= 24;
                for (;day_add + day > months[month];++ month) day_add -= months[month] - day + 1 , day = 1;
                day += day_add;
            }
        }
        static int dateminus(const timeType &lhs, const timeType &rhs) {
            int tmp1 = 0, tmp2 = 0;
            for (int i = 1; i <= lhs.month; ++i)
                tmp1 += lhs.months[i];
            tmp1 += lhs.day;
            for (int i = 1; i <= rhs.month; ++i)
                tmp2 += rhs.months[i];
            tmp2 += rhs.day;
            return (tmp1 - tmp2);
        }
        bool operator<(const timeType &other) {
            int tmp1 = 0, tmp2 = 0;
            for (int i = 1; i <= month; ++i)
                tmp1 += months[i];
            tmp1 += day;
            tmp1 *= 24;
            tmp1 += hour;
            tmp1 *= 60;
            tmp1 += minute;
            for (int i = 1; i <= other.month; ++i)
                tmp2 += other.months[i];
            tmp2 += other.day;
            tmp2 *= 24;
            tmp2 += other.hour;
            tmp2 *= 60;
            tmp2 += other.minute;
            return (tmp1 < tmp2);
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
