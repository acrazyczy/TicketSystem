//
// Created by crazy_cloud on 2020/6/11.
//
#include "TrainManager.hpp"
#include "OrderManager.hpp"

namespace sjtu
{
	bool TrainManager::buy_ticket(orderType *order)
	{
		auto ret = TrainBpTree -> find(hasher(order -> trainID));
		if (ret.second == false)
		{
			order -> status = pending;
			return 0;
		}
		else
		{
			trainType *train = TrainFile -> read(ret.first);
			int min_ticket_num = order -> num , day_id = timeType::dateminus(order -> date[0] , train -> saleDate[0]) + train -> get_Delta_date(std::string(order -> station[0])) , single_ticket_price = 0;
			if (day_id < 0 || day_id > timeType::dateminus(train -> saleDate[1] , train -> saleDate[0]) || train -> is_released == false || order -> num > train -> seatNum) return 0;
			else
			{
				timeType current_time = train -> startTime;
				current_time.month = order -> date[0].month , current_time.day = order -> date[0].day;
				bool flag = false;
				for (int i = 0;i < train -> stationNum;++ i)
				{
					if (i) current_time = current_time + train -> stations[i].travelTime;
					if (flag) single_ticket_price += train -> stations[i].price , min_ticket_num = min(min_ticket_num , train -> stations[i].seatNum[day_id]);
					if (std::string(train -> stations[i].stationName) == std::string(order -> station[1]))
					{
						order -> date[1] = current_time , order -> price = single_ticket_price;
						break;
					}
					if (i) current_time = current_time + train -> stations[i].stopoverTime;
					if (std::string(train -> stations[i].stationName) == std::string(order -> station[0]))
					{
						current_time.month = order -> date[0].month , current_time.day = order -> date[0].day;
						order -> date[0] = current_time , flag = true;
					}
				}
				if (min_ticket_num == order -> num)
				{
					flag = false;
					for (int i = 0;i < train -> stationNum;++ i)
					{
						if (flag) train -> stations[i].seatNum[day_id] -= order -> num;
						if (std::string(train -> stations[i].stationName) == std::string(order -> station[1])) break;
						if (std::string(train -> stations[i].stationName) == std::string(order -> station[0])) flag = true;
					}
					order -> status = success;
				}
				else order -> status = pending;
				TrainFile -> save(train -> offset);
				return 1;
			}
		}
	}

	void TrainManager::refund_ticket(orderType *order)
	{
		auto ret = TrainBpTree -> find(hasher(order -> trainID));
		trainType *train = TrainFile -> read(ret.first);
		int day_id = timeType::dateminus(order -> date[0] , train -> saleDate[0]) + train -> get_Delta_date(std::string(order -> station[0]));
		bool flag = false;
		for (int i = 0;i < train -> stationNum;++ i)
		{
			if (flag) train -> stations[i].seatNum[day_id] += order -> num;
			if (std::string(train -> stations[i].stationName) == std::string(order -> station[1])) break;
			if (std::string(train -> stations[i].stationName) == std::string(order -> station[0])) flag = true;
		}
		TrainFile -> save(train -> offset);
	}
}