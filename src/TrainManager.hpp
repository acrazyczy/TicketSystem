//
// Created by crazy_cloud on 2020/6/2.
//

#ifndef TICKETSYSTEM_TRAINMANAGER_HPP
#define TICKETSYSTEM_TRAINMANAGER_HPP

#include "TypesAndHeaders.hpp"
#include "FileManager.hpp"
#include "OrderManager.hpp"
#include "BplusTree.hpp"

namespace sjtu
{
	class stationType
	{
		char stationName[41];
		int seatNum;
		int price;
		timeType duration , stop;
	};

	class trainType
	{
		char trainID[21];
		int stationNum;
		stationType stations[101];
		timeType saleDate;
		char type;
		bool is_released;
		locType offset;
	};

	class TrainManager
	{
	private:
		BpTree<StringHasher::hashType , unsigned int> StationBpTree;
		BpTree<StringHasher::hashType , locType> *TrainBpTree;
		StringHasher hasher;
	public:
		DynamicFileManager<trainType> *TrainFile;

		void init(bool is_reset = false)
		{
			
		}

		TrainManager(){init();}

		void add_train();

		void query_ticket();

		void query_transfer();

		void release_train();

		void query_train();

		void delete_train();

		bool buy_ticket();

		void refund_ticket();

		~TrainManager();
	};
}

#endif //TICKETSYSTEM_TRAINMANAGER_HPP
