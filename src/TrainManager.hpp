//
// Created by crazy_cloud on 2020/6/2.
//

#ifndef TICKETSYSTEM_TRAINMANAGER_HPP
#define TICKETSYSTEM_TRAINMANAGER_HPP

#include "TypesAndHeaders.hpp"
#include "FileManager.hpp"
#include "OrderManager.hpp"
#include "BplusTree.hpp"
#include <cstring>
#include <string>

namespace sjtu
{
	struct stationType
	{
		char stationName[41];
		int seatNum[92];
		int price , stopoverTime , duration;
	};

	struct trainType
	{
		char trainID[21];
		int stationNum;
		stationType stations[101];
		timeType saleDate[2] , startTime;
		char type;
		bool is_released;
		locType offset;

		void display_single(int day_id , const timeType &date , int start , int end)
		{
			timeType current_time = startTime;
			startTime.month = data.month , startTime.day = date.month;
			int price_sum = 0;
			for (int i = 0;i < end;++ i)
			{
				if (i >= start) std::cout << stations[i].stationName << " ";
				if (!i) {if (i >= start) std::cout << "xx-xx xx:xx -> ";}
				else
				{
					current_time = current_time + stations[i].duration;
					price_sum += stations[i].price;
					if (i >= start) std::cout << current_time << " -> ";
					current_time = current_time + stations[i].stopoverTime;
					if (i >= start) std::cout << current_time << std::endl;
				}
				if (i >= start)
				{
					std::cout << " " << price_sum;
					if (i == stationNum - 1) std::cout << " x" << std::endl;
					else std::cout << " " << stations[i + 1].seatNum[day_id] << std::endl; 
				}
			}
		}

		void display(timeType date)
		{
			int day_id = /*to do*/;
			if (day_id < 0 || day_id >= 92) std::cout << -1 << std::endl;
			else
			{
				std::cout << trainID << " " << type << std::endl;
				display_single(day_id , date , 0 , stationNum);
			}
		}

		int get_Delta_date(std::string stationName)
		{
		}
	};

	class TrainManager
	{
	private:
		BpTree<StringHasher::hashType , unsigned int> *StationBpTree;
		BpTree<StringHasher::hashType , locType> *TrainBpTree;
		StringHasher hasher;
	public:
		DynamicFileManager<trainType> *TrainFile;

		void init(bool is_reset = false)
		{
			if (is_reset)
			{
				StationBpTree -> init("StationBpTree.dat" , true);
				TrainBpTree -> init("TrainBpTree.dat" , true);
				TrainFile -> init("TrainFile.dat" , true);
			}
			else
			{
				StationBpTree = new BpTree<StringHasher::hashType , unsigned int> ("StationBpTree.dat");
				TrainBpTree = new BpTree<StringHasher::hashType , locType> ("TrainBpTree.dat");
				TrainFile = new DynamicFileManager<trainType> ("TrainFile.dat");
			}
			if (TrainFile -> is_newfile)
			{
				//to do : bitset init
			}
		}

		TrainManager(){init();}

		void add_train(int argc , std::string *argv)
		{
			std::string trainID , stationNum , seatNum , stations , prices , startTime , travelTimes , stopoverTimes , saleDate , type;
			for (int i = 0;i < argc;i += 2)
				if (argv[i] == "-i") trainID = argv[i + 1];
				else if (argv[i] == "-n") stationNum = argv[i + 1];
				else if (argv[i] == "-m") seatNum = argv[i + 1];
				else if (argv[i] == "-s") stations = argv[i + 1];
				else if (argv[i] == "-p") prices = argv[i + 1];
				else if (argv[i] == "-x") startTime = argv[i + 1];
				else if (argv[i] == "-t") travelTimes = argv[i + 1];
				else if (argv[i] == "-o") stopoverTimes = argv[i + 1];
				else if (argv[i] == "-d") saleDate = argv[i + 1];
				else if (argv[i] == "-y") type = argv[i + 1];
				else throw invalid_command();
			if (TrainBpTree -> find(hasher(trainID)).second) std::cout << -1 << std::endl;
			else
			{
				auto ret = TrainFile -> newspace();
				trainType *train = ret.first;
				train -> offset = ret -> second , train -> is_released = false;
				strcpy(train -> trainID , trainID.c_str());
				train -> stationNum = stoi(stationNum);
				for (int i = 0 , seatNum_ = stoi(seatNum);i < train -> stationNum;++ i)
					for (int j = 0;j < 92;++ j)
						train -> stations[i].seatNum[j] = seatNum_;
				for (int i = 0 , l = 0 , r , len = stations.size();i < train -> stationNum;++ i , l = r + 1)
				{
					for (r = l;r < len && stations[r] != '|';++ r);
					strcpy(train -> stations[i].stationName , stations.substr(l , r - l).c_str());
					//to do : Station Record
				}
				for (int i = 1 , l = 0 , r , len = prices.size();i < train -> stationNum;++ i , l = r + 1)
				{
					for (r = l;r < len && prices[r] != '|';++ r);
					strcpy(train -> stations[i].price , prices.substr(l , r - l));
				}
				train -> startTime = timeType(startTime);
				for (int i = 1 , l = 0 , r , len = travelTimes.size();i < train -> stationNum;++ i , l = r + 1)
				{
					for (r = l;r < len && travelTime[r] != '|';++ r);
					train -> stations[i].duration = stoi(travelTimes.substr(l , r - l));
				}
				for (int i = 1 , l = 0 , r , len = stopoverTimes.size();i < train -> stationNum - 1;++ i)
				{
					for (r = l;r < len && stopoverTimes[r] != '|';++ r);
					train -> stations[i].stopoverTime = stoi(stopoverTimes.substr(l , r - l));
				}
				train -> saleDate[0] = timeType(saleDate.substr(0 , 4)) , train -> saleDate[1] = timeType(saleDate.substr(6 , 10));
				train -> type = type[0];
				TrainFile -> save(train -> offset) , TrainBpTree -> insert(std::make_pair<hasher(train -> trainID) , train -> offset>);
				std::cout << 0 << std::endl;
			}
			//to do : bitset add
		}

		void query_ticket();

		void query_transfer();

		void release_train(int argc , std::string *argv)
		{
			std::string trainID;
			for (int i = 0;i < argc;i += 2)
				if (argv[i] == "-i") trainID = argv[i + 1];
				else throw invalid_command();
			auto ret = TrainBpTree -> find(hasher(trainID));
			if (ret.second == false) std::cout << -1 << std::endl;
			else
			{
				trainType *train = TrainFile -> read(ret.first);
				train -> is_released = true , TrainFile -> save(train -> offset);
				std::cout << 0 << std::endl;
			}
		}

		void query_train(int argc , string *argv)
		{
			std::string trainID , date;
			for (int i = 0;i < argc;i += 2)
				if (argv[i] == "-i") trainID = argv[i + 1];
				else if (argv[i] == "-d") date = argv[i + 1];
				else throw invalid_command();
			auto ret = TrainBpTree -> find(hasher(trainID));
			if (ret.second == false) std::cout << -1 << std::endl;
			else TrainFile -> read(ret.first) -> display(timeType(date));
		}

		void delete_train(int argc , string *argv)
		{
			std::string trainID;
			for (int i = 0;i < argc;i += 2)
				if (argv[i] == "-i") trainID = argv[i + 1];
				else throw invalid_command();
			auto ret = TrainBpTree -> find(hasher(trainID));
			if (ret.second == false) std::cout << -1 << std::endl;
			else if (TrainFile -> read() -> is_released == true) std::cout << -1 << std::endl;
			else
			{
				TrainBpTree -> erase(hasher(trainID)) , TrainFile -> release(ret.first);
				std::cout << 0 << std::endl;
			}
		}

		bool buy_ticket(orderType *order)
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
				int min_ticket_num = order -> num , day_id = /**/ , single_ticket_price = 0;
				if (day_id < 0 || day_id >= 92) return 0;
				else
				{
					timeType current_time = train -> startTime;
					current_time.month = order -> date[0].month , current_time.day = order -> date[0].day;
					bool flag = false;
					for (int i = 0;i < train -> stationNum;++ i)
					{
						if (i) current_time = current_time + train -> station[i].duration;
						if (flag) single_ticket_price += train -> station[i].price , min_ticket_num = min_ticket_num < train -> station[i].seatNum[day_id] ? min_ticket_num : train -> station[i].seatNum[day_id];
						if (std::string(train -> station[i].stationName) == std::string(order -> station[1]))
						{
							order -> date[1] = current_time , order -> price = single_ticket_price * order -> num;
							break;
						}
						current_time = current_time + train -> station[i].stopoverTime;
						if (std::string(train -> station[i].stationName) == std::string(order -> station[0])) order -> date[0] = current_time , flag = true;
					}
					if (min_ticket_num == order -> num)
					{
						flag = false;
						for (int i = 0;i < train -> stationNum;++ i)
						{
							if (flag) train -> station[i].seatNum[day_id] -= order -> num;
							if (std::string(train -> station[i].stationName) == std::string(order -> station[1])) break;
							if (std::string(train -> station[i].stationName) == std::string(order -> station[0])) flag = true;
						}
						order -> status = success;
					}
					else order -> status = pending;
					return 1;
				}
			}
		}

		void refund_ticket(OrderType *order)
		{
			auto ret = TrainBpTree -> find(hasher(order -> trainID));
			trainType *train = TrainFile -> read(ret.first);
			int day_id = /**/ + train -> get_Delta_date(std::string(order -> station[0]));
			bool flag = false;
			for (int i = 0;i < train -> stationNum;++ i)
			{
				if (flag) train -> station[i].seatNum[day_id] += order -> num;
				if (std::string(train -> station[i].stationName) == std::string(order -> station[1])) break;
				if (std::string(train -> station[i].stationName) == std::string(order -> station[0])) flag = true;
			}
		}

		~TrainManager();
	};
}

#endif //TICKETSYSTEM_TRAINMANAGER_HPP
