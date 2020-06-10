//
// Created by crazy_cloud on 2020/5/25.
//

#ifndef TICKETSYSTEM_FILEMANAGER_HPP
#define TICKETSYSTEM_FILEMANAGER_HPP

#include "TypesAndHeaders.hpp"
#include "LRUCache.hpp"

namespace sjtu
{
	template <class valueType>
	class FileManager_Base
	{
	protected:
		std::fstream file;
	public:
		FileManager_Base(){}

		virtual void init(std::string , bool = false) = 0;
		virtual std::pair<valueType * , locType> newspace() = 0;
		virtual valueType *read(const locType &) = 0;
		virtual void save(const locType &) = 0;

		virtual ~FileManager_Base(){}
	};

	template <class valueType>
	class FileManager : public FileManager_Base<valueType>
	{
	private:
		LRUCache<valueType> *cache;

		using FileManager_Base<valueType>::file;
	public:
		typedef valueType blockType;

		bool is_newfile;

		FileManager(std::string filepath) {cache = nullptr , init(filepath);}

		virtual void init(std::string filepath , bool is_reset = false) override
		{
			if (cache) delete cache;
			cache = new LRUCache<valueType>(1024 , file);
			std::ifstream infile(filepath , std::ios_base::in);
			if (!infile.is_open() || is_reset)
			{
				is_newfile = true;
				std::ofstream outfile(filepath , std::ios_base::out | std::ios_base::binary);
				outfile.close();
			}else is_newfile = false;
			infile.close();
			file.open(filepath , std::ios_base::in | std::ios_base::out | std::ios_base::binary);
		}

		virtual std::pair<valueType * , locType> newspace() override
		{
			file.seekp(0 , std::ios_base::end) , file.clear();
			valueType tmp;locType offset = file.tellp();
			file.write(reinterpret_cast<char *> (&tmp) , sizeof (valueType));
			return std::make_pair(cache -> load(offset) , offset);
		}

		virtual valueType *read(const locType &offset) override {return cache -> load(offset);}

		virtual void save(const locType &offset) override {cache -> dirty_page_set(offset);}

		virtual ~FileManager() {delete cache , file.close();}
	};

	template <class valueType>
	class DynamicFileManager : public FileManager_Base<valueType>
	{
	private:
		union dataType
		{
			valueType value;
			locType nxt;

			dataType() {memset(this , 0 , sizeof (dataType));}
			dataType(const valueType &value_) : value(value_) {}
			dataType(const locType &nxt_) : nxt(nxt_) {}
		};

		LRUCache<dataType> *cache;

		using FileManager_Base<valueType>::file;
	public:
		typedef dataType blockType;

		bool is_newfile;

		DynamicFileManager(std::string filepath) {cache = nullptr , init(filepath);}

		virtual void init(std::string filepath , bool is_reset = false) override
		{
			if (cache) delete cache;
			cache = new LRUCache<dataType>(1024 , file);
			std::ifstream infile(filepath , std::ios_base::in);
			if (!infile.is_open() || is_reset)
			{
				is_newfile = true;
				std::ofstream outfile(filepath , std::ios_base::out | std::ios_base::binary);
				dataType tmp(locType(sizeof (dataType)));
				outfile.write(reinterpret_cast<char *> (&tmp) , sizeof (dataType)) , outfile.close();
			}else is_newfile = false;
			infile.close();
			file.open(filepath , std::ios_base::in | std::ios_base::out | std::ios_base::binary);
		}

		virtual std::pair<valueType * , locType> newspace() override
		{
			dataType tmp;
			file.seekg(0 , std::ios_base::beg) , file.read(reinterpret_cast<char *>(&tmp) , sizeof (dataType));
			locType offset = tmp.nxt;
			file.seekp(0 , std::ios_base::end) , file.clear();
			if (file.tellp() == offset)
			{
				file.write(reinterpret_cast<char *> (&tmp) , sizeof (dataType));
				tmp.nxt = offset + (sizeof (dataType)) , file.seekp(0 , std::ios_base::beg);
				file.write(reinterpret_cast<char *> (&tmp) , sizeof (dataType));
			}
			else
			{
				file.seekg(offset) , file.read(reinterpret_cast<char *> (&tmp) , sizeof (dataType));
				locType offset_ = tmp.nxt;tmp = dataType();
				file.seekp(offset) , file.write(reinterpret_cast<char *> (&tmp) , sizeof (dataType));
				tmp.nxt = offset_ , file.seekp(0 , std::ios_base::beg) , file.write(reinterpret_cast<char *> (&tmp) , sizeof (dataType));
			}
			return std::make_pair(&(cache -> load(offset) -> value) , offset);
		}

		void release(const locType &offset)
		{
			dataType tmp;
			cache -> remove(offset) , file.seekg(0 , std::ios_base::beg) , file.read(reinterpret_cast<char *> (&tmp) , sizeof (dataType));
			locType offset_ = tmp.nxt;
			tmp.nxt = offset , file.seekp(0 , std::ios_base::beg) , file.write(reinterpret_cast<char *> (&tmp) , sizeof (dataType));
			tmp.nxt = offset_ , file.seekp(offset) , file.write(reinterpret_cast<char *> (&tmp) , sizeof (dataType));
		}

		virtual valueType *read(const locType &offset) override {return &(cache -> load(offset) -> value);}

		virtual void save(const locType &offset) override {cache -> dirty_page_set(offset);}

		virtual ~DynamicFileManager() {delete cache , file.close();}
	};
}
#endif //TICKETSYSTEM_FILEMANAGER_HPP
