//
// Created by crazy_cloud on 2020/6/2.
//

#ifndef TICKETSYSTEM_TRAINMANAGER_HPP
#define TICKETSYSTEM_TRAINMANAGER_HPP

#include "TypesAndHeaders.hpp"
#include "FileManager.hpp"
#include "BplusTree.hpp"

namespace sjtu
{
	const int max_train_num = 16384;
	const int W = 32;
	const int WS = max_train_num / W;

	class orderType;

	struct recordType
	{
		std::string trainID , station[2];
		timeType time[2];
		int price , seat , duration;

		friend std::ostream &operator<<(std::ostream &os , const recordType &rec)
		{
			os << rec.trainID << " " << rec.station[0] << " " << rec.time[0] << " " << rec.station[1] << " " << rec.time[1] << " " << rec.price << " " << rec.seat;
		}
	};

	struct cmp_cost
	{
		bool operator()(const recordType &lhs , const recordType &rhs) const {return lhs.price < rhs.price || lhs.price == rhs.price && lhs.trainID < rhs.trainID;}
	};

	struct cmp_time
	{
		bool operator()(const recordType &lhs , const recordType &rhs) const {return lhs.duration < rhs.duration || lhs.duration == rhs.duration && lhs.trainID < rhs.trainID;}
	};

	struct stationType
	{
		char stationName[41];
		int seatNum[92];
		int price , stopoverTime , travelTime;
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
			current_time.month = date.month , current_time.day = date.month;
			int price_sum = 0;
			for (int i = 0;i < end;++ i)
			{
				if (i >= start) std::cout << stations[i].stationName << " ";
				if (!i) {if (i >= start) std::cout << "xx-xx xx:xx -> ";}
				else
				{
					current_time = current_time + stations[i].travelTime;
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
			int day_id = timeType::dateminus(date , saleDate[0]);
			if (day_id < 0 || day_id > timeType::dateminus(saleDate[1] , saleDate[0])) std::cout << -1 << std::endl;
			else
			{
				std::cout << trainID << " " << type << std::endl;
				display_single(day_id , date , 0 , stationNum);
			}
		}

		int get_Delta_date(std::string stationName)
		{
			timeType current_time = startTime;
			current_time.month = 1 , current_time.day = 1;
			for (int i = 0;i < stationNum;++ i)
			{
				if (i) current_time = current_time + stations[i].travelTime;
				if (stationName == std::string(stations[i].stationName)) break;
				current_time = current_time + stations[i].stopoverTime;
			}
			return 1 - current_time.day;
		}

		bool getrecord(recordType *record)
		{
			if (!is_released) return false;
			timeType actual_time(startTime);
			bool found = false;
			for (int i = 0 , day_id;i < stationNum;++ i)
			{
				if (i) actual_time = actual_time + stations[i].travelTime;
				if (found)
				{
					record -> seat = record -> seat < stations[i].seatNum[day_id] ? record -> seat : stations[i].seatNum[day_id];
					record -> price += stations[i].price , record -> duration += stations[i].travelTime;
					if (stations[i].stationName == record -> station[1])
					{
						record -> time[1] = actual_time;
						break;
					}
					record -> duration += stations[i].stopoverTime;
				}
				else if (stations[i].stationName == record -> station[1]) break;
				if (i) actual_time = actual_time + stations[i].stopoverTime;
				if (stations[i].stationName == record -> station[0])
				{
					record -> price = 0 , record -> duration = 0 , record -> seat = 100000;
					actual_time.month = record -> time[0].month , actual_time.day = record -> time[0].day;
					if (actual_time < record -> time[0]) actual_time = actual_time + 24 * 60;
					day_id = timeType::dateminus(actual_time , saleDate[0]) + get_Delta_date(record -> station[0]);
					if (day_id < 0 || day_id > timeType::dateminus(saleDate[1] , saleDate[0])) break;
					found = true;
				}
			}
			if (!found) return false;
			return true;
		}
	};

	class TrainManager
	{
	private:
		BplusTree<StringHasher::hashType , unsigned int> *StationBpTree;
		BplusTree<StringHasher::hashType , locType> *TrainBpTree;
		StringHasher hasher;

		struct stationNameType
		{
			char name[41];
		};

		FileManager<stationNameType> *StationFile;

		void bitset_set(unsigned int x , unsigned int y , unsigned int bit = 1)
		{
			std::fstream bitset("Bitset.dat" , std::ios_base::in | std::ios_base::out | std::ios_base::binary);
			unsigned int bits;
			bitset.seekg((x * WS + y / W) * sizeof (int)) , bitset.read(reinterpret_cast<char *> (&bits) , sizeof (unsigned int));
			if (bit) bits |= 1 << y % W;
			else bits &= (~ 0) ^ (1 << y % W);
			bitset.seekp((x * WS + y / W) * sizeof (int)) , bitset.write(reinterpret_cast<char *> (&bits) , sizeof (unsigned int));
			bitset.close();
		}

		void bitset_query(unsigned int x , unsigned int *bits)
		{
			std::ifstream bitset("Bitset.dat" , std::ios_base::in | std::ios_base::binary);
			bitset.seekg((x * WS) * sizeof (int));
			for (int i = 0;i < WS;++ i) bitset.read(reinterpret_cast<char *> (bits + i) , sizeof (unsigned int));
			bitset.close();
		}

		unsigned int bitset_newblock(unsigned int y)
		{
			std::ofstream bitset("Bitset.dat" , std::ios_base::out | std::ios_base::app);
			unsigned int ret = bitset.tellp() / sizeof (unsigned int) / WS;
			for (int i = 0;i < WS;++ i)
			{
				unsigned int bit = (i == y / W) << y % W;
				bitset.write(reinterpret_cast<char *> (&bit) , sizeof (unsigned int));
			}
			return ret;
		}

	public:
		DynamicFileManager<trainType> *TrainFile;

		void init(bool is_reset = false)
		{
			if (is_reset)
			{
				StationBpTree -> init("StationBpTree.dat" , true);
				TrainBpTree -> init("TrainBpTree.dat" , true);
				TrainFile -> init("TrainFile.dat" , true);
				StationFile -> init("StationFile.dat" , true);
			}
			else
			{
				StationBpTree = new BplusTree<StringHasher::hashType , unsigned int> ("StationBpTree.dat");
				TrainBpTree = new BplusTree<StringHasher::hashType , locType> ("TrainBpTree.dat");
				TrainFile = new DynamicFileManager<trainType> ("TrainFile.dat");
				StationFile = new FileManager<stationNameType> ("StationFile.dat");
			}
			if (TrainFile -> is_newfile)
			{
				std::ofstream outfile("Bitset.dat" , std::ios_base::out | std::ios_base::binary);
				outfile.close();
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
				train -> offset = ret.second , train -> is_released = false;
				strcpy(train -> trainID , trainID.c_str());
				train -> stationNum = stoi(stationNum);
				for (int i = 0 , seatNum_ = stoi(seatNum);i < train -> stationNum;++ i)
					for (int j = 0;j < 92;++ j)
						train -> stations[i].seatNum[j] = seatNum_;
				for (int i = 0 , l = 0 , r , len = stations.size();i < train -> stationNum;++ i , l = r + 1)
				{
					for (r = l;r < len && stations[r] != '|';++ r);
					strcpy(train -> stations[i].stationName , stations.substr(l , r - l).c_str());
				}
				for (int i = 1 , l = 0 , r , len = prices.size();i < train -> stationNum;++ i , l = r + 1)
				{
					for (r = l;r < len && prices[r] != '|';++ r);
					train -> stations[i].price = stoi(prices.substr(l , r - l));
				}
				train -> startTime = timeType(startTime);
				for (int i = 1 , l = 0 , r , len = travelTimes.size();i < train -> stationNum;++ i , l = r + 1)
				{
					for (r = l;r < len && travelTimes[r] != '|';++ r);
					train -> stations[i].travelTime = stoi(travelTimes.substr(l , r - l));
				}
				for (int i = 1 , l = 0 , r , len = stopoverTimes.size();i < train -> stationNum - 1;++ i)
				{
					for (r = l;r < len && stopoverTimes[r] != '|';++ r);
					train -> stations[i].stopoverTime = stoi(stopoverTimes.substr(l , r - l));
				}
				train -> saleDate[0] = timeType(saleDate.substr(0 , 4)) , train -> saleDate[1] = timeType(saleDate.substr(6 , 10));
				train -> type = type[0];
				TrainFile -> save(train -> offset) , TrainBpTree -> insert(std::make_pair(hasher(train -> trainID) , train -> offset));
				std::cout << 0 << std::endl;
			}
		}

		void query_ticket(int argc , std::string *argv)
		{
			std::string station[2] , date , keyword = "";
			for (int i = 0;i < argc;i += 2)
				if (argv[i] == "-s") station[0] = argv[i + 1];
				else if (argv[i] == "-t") station[1] = argv[i + 1];
				else if (argv[i] == "-d") date = argv[i + 1];
				else if (argv[i] == "-p") keyword = argv[i + 1];
				else throw invalid_command();
			auto ret_s = StationBpTree -> find(hasher(station[0])) , ret_t = StationBpTree -> find(hasher(station[1]));
			if (ret_s.second == false || ret_t.second == false) std::cout << 0 << std::endl;
			else
			{
				unsigned int bits[2][WS];
				bitset_query(ret_s.first , bits[0]) , bitset_query(ret_t.first , bits[1]);
				vector<recordType> records;
				recordType *record;
				for (int i = 0;i < WS;++ i)
				{
					bits[0][i] &= bits[1][i];
					for (unsigned int w;bits[0][i];bits[0][i] ^= w)
					{
						w = bits[0][i] & (-bits[0][i]);
						int id = 0;
						for (;w;w >>= 1) ++ id;
						trainType *train = TrainFile -> read((i * W + id + 1) * sizeof (DynamicFileManager<trainType>::valueType));
						record = new recordType , record -> trainID = train -> trainID;
						record -> station[0] = station[0] , record -> station[1] = station[1];
						record -> time[0] = timeType(date);
						if (train -> getrecord(record)) records.push_back(*record);
						delete record;
					}
				}
				if (keyword == "time") std::sort(records.begin() , records.end() , cmp_time());
				else std::sort(records.begin() , records.end() , cmp_cost());
				std::cout << records.size() << std::endl;
				for (auto rec : records) std::cout << rec << std::endl;
			}
		}

		void query_transfer(int argc , std::string *argv)
		{
			std::string station[2] , date , keyword = "";
			for (int i = 0;i < argc;i += 2)
				if (argv[i] == "-s") station[0] = argv[i + 1];
				else if (argv[i] == "-t") station[1] = argv[i + 1];
				else if (argv[i] == "-d") date = argv[i + 1];
				else if (argv[i] == "-p") keyword = argv[i + 1];
				else throw invalid_command();
			auto ret_s = StationBpTree -> find(hasher(station[0])) , ret_t = StationBpTree -> find(hasher(station[1]));
			if (ret_s.second == false || ret_t.second == false) std::cout << 0 << std::endl;
			else
			{
				unsigned int bits[2][WS];
				bitset_query(ret_s.first , bits[0]);
				int station_count = StationBpTree -> size();
				recordType best[4];bool best_filled[4];
				best_filled[0] = best_filled[1] = best_filled[2] = false;
				for (int trs = 0;trs < station_count;++ trs)
					if (trs != ret_s.first && trs != ret_t.first)
					{
						std::string trs_station_name = "";
						bitset_query(trs , bits[1]);
						recordType *record;
						for (int i = 0;i < WS;++ i)
						{
							bits[0][i] &= bits[1][i];
							for (unsigned int w;bits[0][i];bits[0][i] ^= w)
							{
								w = bits[0][i] & (-bits[0][i]);
								int id = 0;
								for (;w;w >>= 1) ++ id;
								trainType *train = TrainFile -> read((i * W + id + 1) * sizeof (DynamicFileManager<trainType>::valueType));
								record = new recordType , record -> trainID = train -> trainID;
								record -> station[0] = station[0];
								if (trs_station_name == "")
								{
									stationNameType *stname = StationFile -> read(i * sizeof (FileManager<stationNameType>::valueType));
									trs_station_name = std::string(stname -> name);
								}
								record -> station[1] = trs_station_name;
								record -> time[0] = timeType(date);
								if (train -> getrecord(record))
								{
									if (!best_filled[0] || keyword == "time" && record -> time[1] < best[0].time[1] || keyword != "time" && record -> price < best[0].price)
									{
										if (best_filled[0]) best_filled[1] = true , best[1] = best[0];
										best_filled[0] = true , best[0] = *record;
									}
									else if (!best_filled[1] || keyword == "time" && record -> time[1] < best[1].time[1] || keyword != "time" && record -> price < best[1].price)
										best_filled[1] = true , best[1] = *record;
								}
								delete record;
							}
						}
					}
				bitset_query(ret_t.first , bits[1]);
				for (int trs = 0;trs < station_count;++ trs)
					if (trs != ret_s.first && trs != ret_t.first)
					{
						std::string trs_station_name = "";
						bitset_query(trs , bits[1]);
						recordType *record;
						for (int i = 0;i < WS;++ i)
						{
							bits[0][i] &= bits[1][i];
							for (unsigned int w;bits[0][i];bits[0][i] ^= w)
							{
								w = bits[0][i] & (-bits[0][i]);
								int id = 0;
								for (;w;w >>= 1) ++ id;
								trainType *train = TrainFile -> read((i * W + id + 1) * sizeof (DynamicFileManager<trainType>::valueType));
								record = new recordType , record -> trainID = train -> trainID;
								if (trs_station_name == "")
								{
									stationNameType *stname = StationFile -> read(i * sizeof (FileManager<stationNameType>::valueType));
									trs_station_name = std::string(stname -> name);
								}
								record -> station[0] = trs_station_name , record -> station[1] = station[1];
								int flag;
								if (best_filled[0] && best[0].trainID != record -> trainID) record -> time[0] = best[0].time[1] , flag = 0;
								else if (best_filled[1]) record -> time[1] = best[1].time[1] , flag = 1;
								else flag = -1;
								if (~ flag && train -> getrecord(record))
									if (!best_filled[2] || keyword == "time" && record -> time[1] < best[3].time[1] || keyword != "time" && record -> price < best[2].price)
										best_filled[2] = true , best[2] = best[flag] , best[3] = *record;
								delete record;
							}
						}
					}
				if (best_filled[2]) std::cout << best[2] << std::endl << best[3] << std::endl;
				else std::cout << 0 << std::endl;			
			}
		}

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
				for (int i = 0 , train_id = train -> offset / sizeof (DynamicFileManager<trainType>::valueType) - 1;i < train -> stationNum;++ i)
				{
					auto ret = StationBpTree -> find(hasher(train -> stations[i].stationName));
					if (ret.second) bitset_set(ret.first , train_id);
					else
					{
						StationBpTree -> insert(std::make_pair(hasher(train -> stations[i].stationName) , bitset_newblock(train_id)));
						auto ret_ = StationFile -> newspace();
						strcpy(ret_.first -> name , train -> stations[i].stationName);
						StationFile -> save(ret_.second);
					}
				}
				train -> is_released = true , TrainFile -> save(train -> offset);
				std::cout << 0 << std::endl;
			}
		}

		void query_train(int argc , std::string *argv)
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

		void delete_train(int argc , std::string *argv)
		{
			std::string trainID;
			for (int i = 0;i < argc;i += 2)
				if (argv[i] == "-i") trainID = argv[i + 1];
				else throw invalid_command();
			auto ret = TrainBpTree -> find(hasher(trainID));
			if (ret.second == false) std::cout << -1 << std::endl;
			else if (TrainFile -> read(ret.first) -> is_released == true) std::cout << -1 << std::endl;
			else
			{
				trainType *train = TrainFile -> read(ret.first);
				for (int i = 0 , train_id = train -> offset / sizeof (DynamicFileManager<trainType>::valueType) - 1;i < train -> stationNum;++ i)
				{
					int id = StationBpTree -> find(hasher(train -> stations[i].stationName)).first;
					bitset_set(id , train_id , 0);
				}
				TrainBpTree -> erase(hasher(trainID)) , TrainFile -> release(ret.first);
				std::cout << 0 << std::endl;
			}
		}

		bool buy_ticket(orderType *);

		void refund_ticket(orderType *);

		~TrainManager()
		{
			delete StationBpTree , delete TrainBpTree;
			delete StationFile , delete TrainFile;
		}
	};
}

#endif //TICKETSYSTEM_TRAINMANAGER_HPP
