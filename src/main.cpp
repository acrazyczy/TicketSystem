//
// Created by crazy_cloud on 2020/5/31.
//

#include "FileManager.hpp"
#include <algorithm>
#include <iostream>
#include <cassert>
#include <string>

const int N = 4;
const int T = 3;

int offset[N] , value[N] , per[N];

int main()
{
	sjtu::FileManager<int> *file = new sjtu::FileManager<int> (std::string("iotest.dat"));
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
			assert((*val) == value[per[i]]) , (*val) = value[per[i]] = i , file -> save(offset[i]);
		}
	}
	delete file;
	std::cout << "IO test pass." << std::endl;
	return 0;
}