//
// Created by crazy_cloud on 2020/5/31.
//

#include "FileManager.hpp"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <ctime>
#include <string>

const int N = 10000;
const int T = 10;

int offset[N] , value[N] , per[N];

int main()
{
	/*sjtu::FileManager<int> *file = new sjtu::FileManager<int> (std::string("iotest.dat"));
	for (int i = 0;i < N;++ i)
	{
		value[i] = i , per[i] = i;
		auto ret = file -> newspace();
		offset[i] = ret.second , *(ret.first) = i , file -> save(offset[i]);
	}
	for (int t = 0;t < T;++ t)
	{
		std::random_shuffle(per , per + N);
		for (int i = 0;i < N;++ i)
		{
			int *val = file -> read(offset[per[i]]);
			assert((*val) == value[per[i]]) , (*val) = value[per[i]] = i , file -> save(offset[per[i]]);
		}
	}
	delete file;*/
	/*auto ret = file -> newspace();
	*(ret.first) = 19260817 , file -> save(ret.second);
	int *val = file -> read(ret.second);
	std::cout << *val << std::endl;*/
	/*std::ofstream outfile("iotest.dat" , std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
	outfile.close();
	std::fstream file("iotest.dat" , std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	int x = 123 , y;
	file.seekp(0 , std::ios_base::beg) , file.write(reinterpret_cast<char*> (&x) , sizeof (int));
	file.seekg(0 , std::ios_base::beg) , file.read(reinterpret_cast<char *> (&y) , sizeof (int));
	assert(x == y);
	file.seekp(0 , std::ios_base::end) , file.clear() , std::cout << file.tellp() << std::endl;
	file.close();*/
	sjtu::DynamicFileManager<int> *file = new sjtu::DynamicFileManager<int> (std::string("iotest.dat"));
	for (int i = 0;i < N;++ i)
	{
		value[i] = i , per[i] = i;
		auto ret = file -> newspace();
		offset[i] = ret.second , *(ret.first) = i , file -> save(offset[i]);
	}
	for (int i = 0;i < (N >> 1);++ i)
	{
		int *val = file -> read(offset[i]);
		assert((*val) == value[i]) , file -> release(offset[i]) , offset[i] = -1;
	}
	srand(time(0));
	for (int t = 0;t < N * T;++ t)
	{
		int x = rand() % N;
		if (~offset[x])
		{
			int *val = file -> read(offset[x]);
			assert((*val) == value[x]) , file -> release(offset[x]) , offset[x] = -1;
		}
		else
		{
			auto ret = file -> newspace();
			offset[x] = ret.second , *(ret.first) = (value[x] *= -1) , file -> save(offset[x]);
		}
	}
	delete file;
	std::cout << "IO test pass." << std::endl;
	return 0;
}