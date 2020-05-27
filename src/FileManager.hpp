//
// Created by crazy_cloud on 2020/5/25.
//

#ifndef TICKETSYSTEM_FILEMANAGER_HPP
#define TICKETSYSTEM_FILEMANAGER_HPP

#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
#include "LRUCache.hpp"

template <class valueType>
class FileManager_Base
{
private:
	std::fstream file;
public:
	FileManager_Base(){}

	virtual void init(std::string , bool = false) = 0;
	virtual std::pair<valueType * , locType> newspace() = 0;
	virtual valueType *read(const locType &) = 0;
	virtual void save(const locType &) = 0;

	virtual ~FileManager_Base() = 0;
};

template <class valueType>
class FileManager : FileManager_Base<valueType>
{
private:
	LRUCache<valueType> *cache;

	valueType f_read(const locType &offset)
	{
		valueType ret;
		file.seekg(offset) , file.read(reinterpret_cast<char *> (&ret) , sizeof valueType);
		return ret;
	}

	void f_write(const locType &offset , const valueType &value) {file.seekp(offset) , file.write(reinterpret_cast<char *> (&value) , sizeof valueType);}
public:
	FileManager(std::string filepath) {cache = nullptr , init(filepath);}

	virtual void init(std::string filepath , bool is_reset = false) override
	{
		if (!is_reset) file.open(filepath , ios_base::in | ios_base::binary);
		if (is_reset || !file.is_open())
		{
			std::ofstream outfile(filepath , ios_base::out);outfile.close();
			file.open(filepath , ios_base::in | ios_base::binary);
		}
		if (cache) delete cache;
		cache = new LRUCache<valueType>(1024 , f_read , f_write);
	}

	virtual std::pair<valueType * , locType> newspace() override
	{
		file.seekp(0 , ios_base::end);
		valueType tmp;locType offset = file.tellp();
		file.clear() , file.write(reinterpret_cast<char *> (&tmp) , sizeof valueType);
		return std::make_pair(cache -> load(offset) , offset);
	}

	virtual valueType *read(const locType &offset) override {return cache -> load(offset);}

	virtual void save(const locType &offset) override {cache -> dirty_page_set(offset);}

	virtual ~FileManager() {delete cache , file.close();}
};

template <class valueType>
class DynamicFileManager : FileManager_Base<valueType>
{
private:
	union dataType
	{
		valueType value;
		locType nxt;

		dataType() {memset(this , 0 , sizeof dataType);}
		dataType(const valueType &value_) : value(value_) {}
		dataType(const locType &nxt_) : nxt(nxt_) {}
	};

	LRUCache<dataType> *cache;
	std::fstream file;

	void f_write(const locType &offset , const dataType * &value) {}
public:
	DynamicFileManager(std::string filepath) {cache = nullptr , init(filepath);}

	virtual void init(std::string filepath , bool is_reset = false) override
	{
		if (!is_reset) file.open(filepath , ios_base::in | ios_base::binary);
		if (is_reset || !file.is_open())
		{
			std::ofstream outfile(filepath , ios_base::out);
			f_write(0 , dataType(sizeof dataType)) , outfile.close();
			file.open(filepath , ios_base::in | ios_base::binary);
		}
		if (cache) delete cache;
		cache = new LRUCache<valueType>(1024 , f_read , f_write);
	}

	virtual std::pair<valueType * , locType> newspace() override
	{
		dataType tmp = f_read(0);locType offset = tmp.nxt;
		file.seekp(0 , ios_base::end);
		if (file.tellp() == offset)
		{
			file.clear() , file.write(reinterpret_cast<char *> (&tmp) , sizeof valueType);
			f_write(0 , dataType(offset + (sizeof dataType)));
			return std::make_pair(cache -> load(offset) , offset);
		}
		else
		{
			locType offset_ = f_read(offset);
			file.seekp(offset) , file.write(reinterpret_cast<char *> (&tmp) , sizeof valueType);
			f_write(0 , dataType(offset_));
			return std::make_pair(cache -> load(offset) , offset);
		}
	}

	void release(const locType &offset)
	{
		cache -> remove(offset);dataType tmp = f_read(0);
		f_write(0 , dataType(offset)) , f_write(offset , tmp);
	}

	virtual valueType *read(const locType &offset) override {return &(cache -> load(offset) -> value);}

	virtual void save(const locType &offset) override {cache -> dirty_page_set(offset);}

	virtual ~DynamicFileManager() {delete cache , file.close();}
};

#endif //TICKETSYSTEM_FILEMANAGER_HPP
