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
    	static const int StationBpTree_cache_size = 256;
    	static const int TrainBpTree_cache_size = 256;
    	static const int StationFile_cache_size = 32;
    	static const int TrainFile_cache_size = 16;

	static const int max_train_num = 16384;
	static const int W = 32;
	static const int WS = max_train_num / W;

	class orderType;

	struct recordType
	{
		std::string trainID , station[2];
		timeType time[2];
		int price , seat , duration;

		recordType &operator=(const recordType &other)
		{
			if (&other == this) return *this;
			trainID = other.trainID;
			for (int t = 0;t < 2;++ t) station[t] = other.station[t] , time[t] = other.time[t];
			price = other.price , seat = other.seat , duration = other.duration;
			return *this;
		}

		friend std::ostream &operator<<(std::ostream &os , const recordType &rec)
		{
			os << rec.trainID << " " << rec.station[0] << " " << rec.time[0] << " -> " << rec.station[1] << " " << rec.time[1] << " " << rec.price << " " << rec.seat;
		}
	};

	struct cmp_cost
	{
		bool operator()(const recordType &lhs , const recordType &rhs) const {return lhs.price < rhs.price || lhs.price == rhs.price && lhs.trainID < rhs.trainID;}
		bool operator()(const std::pair<recordType , recordType> &lhs , const std::pair<recordType , recordType> &rhs) const
		{
			return lhs.first.price + lhs.second.price < rhs.first.price + rhs.second.price ||
			lhs.first.price + lhs.second.price == rhs.first.price + rhs.second.price &&
			(
				lhs.first.trainID < rhs.first.trainID ||
				lhs.first.trainID == rhs.first.trainID && lhs.first.duration < rhs.first.duration
			);
		}
	};

	struct cmp_time
	{
		bool operator()(const recordType &lhs , const recordType &rhs) const {return lhs.duration < rhs.duration || lhs.duration == rhs.duration && lhs.trainID < rhs.trainID;}
		bool operator()(const std::pair<recordType , recordType> &lhs , const std::pair<recordType , recordType> &rhs) const
		{
			return lhs.first.duration + lhs.second.duration + (lhs.second.time[0] - lhs.first.time[1]) < rhs.first.price + rhs.second.price + (rhs.second.time[0] - rhs.first.time[1])||
			lhs.first.duration + lhs.second.duration + (lhs.second.time[0] - lhs.first.time[1]) == rhs.first.price + rhs.second.price + (rhs.second.time[0] - rhs.first.time[1]) &&
			(
				lhs.first.trainID < rhs.first.trainID ||
				lhs.first.trainID == rhs.first.trainID && lhs.first.duration < rhs.first.duration
			);
		}
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
		int stationNum , seatNum;
		stationType stations[101];
		timeType saleDate[2] , startTime;
		char type;
		bool is_released;
		locType offset;

		void display_single(int day_id , const timeType &date , int start , int end)
		{
			timeType current_time = startTime;
			current_time.month = date.month , current_time.day = date.day;
			int price_sum = 0;
			for (int i = 0;i < end;++ i)
			{
				if (i >= start) std::cout << stations[i].stationName << " ";
				if (i) current_time = current_time + stations[i].travelTime;
				if (i >= start)
					if (i) std::cout << current_time << " -> " , price_sum += stations[i].price;
					else std::cout << "xx-xx xx:xx -> ";
				if (i) current_time = current_time + stations[i].stopoverTime;
				if (i >= start)
				{
					if (i == stationNum - 1) std::cout << "xx-xx xx:xx " << price_sum << " x" << std::endl;
					else std::cout << current_time << " " << price_sum << " " << stations[i + 1].seatNum[day_id] << std::endl;
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
				if (i) current_time = current_time + stations[i].travelTime + stations[i].stopoverTime;
				if (stationName == stations[i].stationName) break;
			}
			return 1 - current_time.day;
		}

		bool getrecord(recordType *record , bool flag = false)
		{
			if (!is_released) return false;
			timeType actual_time(startTime);
			actual_time.month = saleDate[0].month , actual_time.day = saleDate[0].day;
			bool found = false , end_flag = false;
			for (int i = 0 , day_id;i < stationNum;++ i)
			{
				if (i) actual_time = actual_time + stations[i].travelTime;
				if (found)
				{
					record -> seat = record -> seat < stations[i].seatNum[day_id] ? record -> seat : stations[i].seatNum[day_id];
					record -> price += stations[i].price , record -> duration += stations[i].travelTime;
					if (stations[i].stationName == record -> station[1])
					{
						record -> time[1] = actual_time , end_flag = true;
						break;
					}
					record -> duration += stations[i].stopoverTime;
				}
				else if (stations[i].stationName == record -> station[1]) break;
				if (i) actual_time = actual_time + stations[i].stopoverTime;
				if (stations[i].stationName == record -> station[0])
				{
					record -> price = 0 , record -> duration = 0 , record -> seat = 100000;
					if (!flag || actual_time < record -> time[0]) actual_time.month = record -> time[0].month , actual_time.day = record -> time[0].day;
					if (flag && actual_time < record -> time[0]) actual_time = actual_time + 24 * 60;
					day_id = timeType::dateminus(actual_time , saleDate[0]) + get_Delta_date(record -> station[0]);
					if (day_id < 0 || day_id > timeType::dateminus(saleDate[1] , saleDate[0])) break;
					record -> time[0] = actual_time;
					found = true;
				}
			}
			if (!found || !end_flag) return false;
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
				StationBpTree -> init("StationBpTree.dat" , StationBpTree_cache_size , true);
				TrainBpTree -> init("TrainBpTree.dat" , TrainBpTree_cache_size , true);
				TrainFile -> init("TrainFile.dat" , TrainFile_cache_size , true);
				StationFile -> init("StationFile.dat" , StationFile_cache_size , true);
			}
			else
			{
				StationBpTree = new BplusTree<StringHasher::hashType , unsigned int> ("StationBpTree.dat" , StationBpTree_cache_size);
				TrainBpTree = new BplusTree<StringHasher::hashType , locType> ("TrainBpTree.dat" , TrainBpTree_cache_size);
				TrainFile = new DynamicFileManager<trainType> ("TrainFile.dat" , TrainFile_cache_size);
				StationFile = new FileManager<stationNameType> ("StationFile.dat" , StationFile_cache_size);
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
				train -> stationNum = stoi(stationNum) , train -> seatNum = stoi(seatNum);
				for (int i = 0;i < train -> stationNum;++ i)
					for (int j = 0;j < 92;++ j)
						train -> stations[i].seatNum[j] = train -> seatNum;
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
				for (int i = 1 , l = 0 , r , len = stopoverTimes.size();i < train -> stationNum - 1;++ i , l = r + 1)
				{
					for (r = l;r < len && stopoverTimes[r] != '|';++ r);
					train -> stations[i].stopoverTime = stoi(stopoverTimes.substr(l , r - l));
				}
				train -> saleDate[0] = timeType(saleDate.substr(0 , 5)) , train -> saleDate[1] = timeType(saleDate.substr(6 , 5));
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
						if (!w) continue;
						int id = -1;
						for (unsigned int w_ = w;w_;w_ >>= 1) ++ id;
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

		template <class Compare>
		void query_transfer_cmp(std::string station[] , timeType date , Compare comp)
		{
			auto ret_s = StationBpTree -> find(hasher(station[0])) , ret_t = StationBpTree -> find(hasher(station[1]));
			if (ret_s.second == false || ret_t.second == false) std::cout << 0 << std::endl;
			else
			{
				unsigned int bits[3][WS];		
				bitset_query(ret_s.first , bits[0]) , bitset_query(ret_t.first , bits[1]);
				std::pair<recordType , recordType> ans;bool found = false;
				for (int i = 0;i < WS;++ i)
					for (unsigned int w;bits[0][i];bits[0][i] ^= w)
					{
						w = bits[0][i] & (-bits[0][i]);
						if (!w) continue;
						int id = -1;
						for (unsigned int w_ = w;w_;w_ >>= 1) ++ id;
						locType offset = (i * W + id + 1) * sizeof (DynamicFileManager<trainType>::valueType);
						trainType *train = TrainFile -> read(offset);
						int day_id = timeType::dateminus(date , train -> saleDate[0]) + train -> get_Delta_date(station[0]);
						if (train -> is_released == false || day_id < 0 || day_id > timeType::dateminus(train -> saleDate[1] , train -> saleDate[0])) continue;
						std::pair<recordType , recordType> cur;
						cur.first.trainID = train -> trainID , cur.first.station[0] = station[0] , cur.first.time[0] = cur.first.time[1] = train -> startTime;
						cur.first.seat = 100000 , cur.first.price = 0 , cur.first.duration = 0;
						bool flag = false;
						for (int j = 0 , stationNum = train -> stationNum;j < train -> stationNum;++ j)
						{
							train = TrainFile -> read(offset);
							if (j) cur.first.time[1] = cur.first.time[1] + train -> stations[j].travelTime , cur.first.duration += flag * (train -> stations[j].travelTime);
							if (flag)
							{
								cur.first.station[1] = train -> stations[j].stationName;
								cur.first.seat = min(cur.first.seat , train -> stations[j].seatNum[day_id]) , cur.first.price += train -> stations[j].price;
								bitset_query(StationBpTree -> find(hasher(train -> stations[j].stationName)).first , bits[2]);
								for (int k = 0;k < WS;++ k)
								{
									bits[2][k] &= bits[1][k];
									for (unsigned int v;bits[2][k];bits[2][k] ^= v)
									{
										v = bits[2][k] & (-bits[2][k]);
										if (!v) continue;
										int id_ = -1;
										for (unsigned int v_ = v;v_;v_ >>= 1) ++ id_;
										trainType *train_ = TrainFile -> read((k * W + id_ + 1) * sizeof (DynamicFileManager<trainType>::valueType));
										if (train_ -> trainID == cur.first.trainID) continue;
										cur.second.station[0] = cur.first.station[1] , cur.second.station[1] = station[1];
										cur.second.trainID = train_ -> trainID , cur.second.time[0] = cur.first.time[1];
										if (train_ -> getrecord(&cur.second , true)) if (!found || comp(cur , ans)) ans = cur , found = true;
									}
								}
							}
							train = TrainFile -> read(offset);
							if (j) cur.first.time[1] = cur.first.time[1] + train -> stations[j].stopoverTime , cur.first.duration += flag * (train -> stations[j].travelTime);
							if (train -> stations[j].stationName == station[0])
							{
								flag = true;
								cur.first.time[1].month = cur.first.time[0].month = date.month;
								cur.first.time[0].hour = cur.first.time[1].hour , cur.first.time[0].minute = cur.first.time[1].minute;
								cur.first.time[1].day = cur.first.time[0].day = date.day;
							}
						}
					}
				if (!found) std::cout << 0 << std::endl;
				else std::cout << ans.first << std::endl << ans.second << std::endl;
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
			if (keyword == "time") query_transfer_cmp(station , date , cmp_cost());
			else query_transfer_cmp(station , date , cmp_time());
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
				if (train -> is_released == true) std::cout << -1 << std::endl;
				else
				{
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
			else
			{
				trainType *train = TrainFile -> read(ret.first);
				if (train -> is_released == true) std::cout << -1 << std::endl;
				else
				{
					for (int i = 0 , train_id = train -> offset / sizeof (DynamicFileManager<trainType>::valueType) - 1;i < train -> stationNum;++ i)
					{
						int id = StationBpTree -> find(hasher(train -> stations[i].stationName)).first;
						bitset_set(id , train_id , 0);
					}
					TrainBpTree -> erase(hasher(trainID)) , TrainFile -> release(ret.first);
					std::cout << 0 << std::endl;
				}
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
