//
// Created by crazy_cloud on 2020/5/25.
//

#ifndef TICKETSYSTEM_FILEMANAGER_HPP
#define TICKETSYSTEM_FILEMANAGER_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include "LRUCache.hpp"

template <class valueType>
class FileManager
{
private:
	LRUCache *cache;
	fstream file;
public:
	FileManager(std::string filepath) {init(filepath);}

	virtual void init(std::string , bool is_reset = false)
	{
		if (is_reset)
		{

		}
		else
		{
		}
	}

	virtual locType newspace();

	valueType *read(const locType &offset);

	void save(const locType &offset);

	virtual ~FileManager();
};

template <class valueType>
class DynamicFileManager
{
private:
public:
};

#endif //TICKETSYSTEM_FILEMANAGER_HPP
