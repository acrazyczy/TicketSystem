//
// Created by crazy_cloud on 2020/5/25.
//

#ifndef TICKETSYSTEM_FILEMANAGER_HPP
#define TICKETSYSTEM_FILEMANAGER_HPP

#include "TypesAndHeaders.hpp"
#include "LRUCache.hpp"

namespace sjtu
{
	template <class T>
	class FileManager_Base
	{
	protected:
		std::fstream file;
		LRUCache<T> *cache;
	public:
		bool is_newfile;
		FileManager_Base(){}

		virtual void init(std::string , bool = false) = 0;
		virtual std::pair<T * , locType> newspace() = 0;
		virtual T *read(const locType &) = 0;
		virtual void save(const locType &) = 0;

		virtual ~FileManager_Base(){}
	};

	template <class T>
	class FileManager : public FileManager_Base<T>
	{
	private:
		using FileManager_Base<T>::file;
		using FileManager_Base<T>::cache;
	public:
		typedef T valueType;

		using FileManager_Base<T>::is_newfile;

		FileManager(std::string filepath) {cache = nullptr , init(filepath);}

		virtual void init(std::string filepath , bool is_reset = false) override
		{
			if (cache) delete cache;
			cache = new LRUCache<T>(1024 , file);
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

		virtual std::pair<T * , locType> newspace() override
		{
			file.seekp(0 , std::ios_base::end) , file.clear();
			T tmp;locType offset = file.tellp();
			file.write(reinterpret_cast<char *> (&tmp) , sizeof (T));
			return std::make_pair(cache -> load(offset) , offset);
		}

		virtual T *read(const locType &offset) override {return cache -> load(offset);}

		virtual void save(const locType &offset) override {cache -> dirty_page_set(offset);}

		virtual ~FileManager() {delete cache , file.close();}
	};

	template <class T>
	class DynamicFileManager : public FileManager_Base<T>
	{
	private:
		using FileManager_Base<T>::file;
		using FileManager_Base<T>::cache;
	public:
		typedef T valueType;

		using FileManager_Base<T>::is_newfile;

		DynamicFileManager(std::string filepath) {cache = nullptr , init(filepath);}

		virtual void init(std::string filepath , bool is_reset = false) override
		{
			if (cache) delete cache;
			cache = new LRUCache<T>(1024 , file);
			std::ifstream infile(filepath , std::ios_base::in);
			if (!infile.is_open() || is_reset)
			{
				is_newfile = true;
				std::ofstream outfile(filepath , std::ios_base::out | std::ios_base::binary);
				T tmp;
				outfile.write(reinterpret_cast<char *> (&tmp) , sizeof (T));
				locType loc = sizeof (T);
				outfile.seekp(- sizeof (T) , std::ios_base::cur) , outfile.write(reinterpret_cast<char *> (&loc) , sizeof (locType));
				outfile.close();
			}else is_newfile = false;
			infile.close();
			file.open(filepath , std::ios_base::in | std::ios_base::out | std::ios_base::binary);
		}

		virtual std::pair<T * , locType> newspace() override
		{
			locType offset , offset_;T tmp;
			file.seekg(0 , std::ios_base::beg) , file.read(reinterpret_cast<char *> (&offset) , sizeof (locType));
			file.seekp(0 , std::ios_base::end) , file.clear();
			if (file.tellp() == offset)
			{
				file.write(reinterpret_cast<char *> (&tmp) , sizeof (T));
				offset += sizeof (T) , file.seekp(0 , std::ios_base::beg);
				file.write(reinterpret_cast<char *> (&tmp) , sizeof (T));
				file.seekp(- sizeof (T) , std::ios_base::cur) , file.write(reinterpret_cast<char *> (&offset) , sizeof (locType));
			}
			else
			{
				locType offset_;
				file.seekg(offset) , file.read(reinterpret_cast<char *> (&offset_) , sizeof (locType));
				//file.seekp(offset) , file.write(reinterpret_cast<char *> (&tmp) , sizeof (T));
				file.seekp(0 , std::ios_base::beg) , file.write(reinterpret_cast<char *> (&offset_) , sizeof (locType));
			}
			return std::make_pair(cache -> load(offset) , offset);
		}

		void release(locType offset)
		{
			locType offset_;
			cache -> remove(offset);
			file.seekg(0 , std::ios_base::beg) , file.read(reinterpret_cast<char *> (&offset_) , sizeof (locType));
			file.seekp(0 , std::ios_base::beg) , file.write(reinterpret_cast<char *> (&offset) , sizeof (locType));
			file.seekp(offset) , file.write(reinterpret_cast<char *> (&offset_) , sizeof (locType));
		}

		virtual T *read(const locType &offset) override {return cache -> load(offset);}

		virtual void save(const locType &offset) override {cache -> dirty_page_set(offset);}

		virtual ~DynamicFileManager() {delete cache , file.close();}
	};
}
#endif //TICKETSYSTEM_FILEMANAGER_HPP
