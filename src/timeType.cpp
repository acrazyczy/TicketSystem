//
// Created by crazy_cloud on 2020/6/12.
//

#include "timeType.hpp"

namespace sjtu
{
        constexpr int timeType::months[13];

        timeType timeType::operator+(const int &time) const {
        	timeType ret;
                ret.minute = minute + time , ret.hour = hour + ret.minute / 60 , ret.minute %= 60;
                int day_add = ret.hour / 24;ret.hour %= 24;
                for (ret.day = day , ret.month = month;day_add + ret.day > months[ret.month];++ ret.month) day_add -= months[ret.month] - ret.day + 1 , ret.day = 1;
                ret.day += day_add;
        	return ret;
        }
        int timeType::dateminus(const timeType &lhs, const timeType &rhs) {
            int tmp1 = 0, tmp2 = 0;
            for (int i = 1; i < lhs.month; ++i)
                tmp1 += lhs.months[i];
            tmp1 += lhs.day;
            for (int i = 1; i < rhs.month; ++i)
                tmp2 += rhs.months[i];
            tmp2 += rhs.day;
            return (tmp1 - tmp2);
        }
        int timeType::operator-(const timeType &other) const
        {
            return (other.hour < hour || other.hour == hour && other.minute < minute) ?
                dateminus(*this , other) * 24 * 60 + (hour - other.hour) * 60 + minute - other.minute
            :   (dateminus(*this , other) - 1) * 24 * 60 + (24 + hour - other.hour) * 60 + minute - other.minute;
        }
        bool timeType::operator<(const timeType &other) const {
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
        std::ostream &operator<<(std::ostream &output, const timeType &obj) {
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
}