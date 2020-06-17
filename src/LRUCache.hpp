//
// Created by crazy_cloud on 2020/5/25.
//

#ifndef TICKETSYSTEM_LRUCACHE_HPP
#define TICKETSYSTEM_LRUCACHE_HPP

#include "TypesAndHeaders.hpp"
#include "map.hpp"

namespace sjtu
{
	template <class valueType>
	class LRUCache
	{
	private:
		std::fstream &file;

		std::size_t block_lim;

		unsigned int attempt_count , miss_count;

		valueType f_read(const locType &offset)
		{
			valueType ret;
			file.seekg(offset) , file.read(reinterpret_cast<char *> (&ret) , sizeof (valueType));
			return ret;
		}

		void f_write(const locType &offset , valueType &value){file.seekp(offset) , file.write(reinterpret_cast<char *> (&value) , sizeof (valueType));}

		struct CacheNode
		{
			valueType value;
			locType offset;
			bool is_dirty_page;
			CacheNode *prec , *succ;
			LRUCache<valueType> *bel;

			CacheNode(const valueType &value_ , const locType &offset_ , LRUCache<valueType> *bel_) : value(value_) , offset(offset_) , bel(bel_) , is_dirty_page(false) , prec(nullptr) , succ(nullptr)
			{
				bel -> table.insert(std::make_pair(offset , this));
			}

			~CacheNode()
			{
				if (is_dirty_page) bel -> f_write(offset , value);
				bel -> table.erase(bel -> table.find(offset));
			}
		}*head , *tail;

		map<locType , CacheNode*> table;
	public:
		LRUCache(std::size_t block_lim_ , std::fstream &file_) : block_lim(block_lim_) , file(file_) , head(nullptr) , tail(nullptr) , attempt_count(0) , miss_count(0) {}
		LRUCache(const LRUCache &) = delete;

		LRUCache &operator=(const LRUCache &) = delete;

		void remove(const locType &offset)
		{
			typename map<locType , CacheNode*>::iterator it = table.find(offset);
			if (it != table.end())
			{
				CacheNode *node = it -> second;
				if (node -> prec) node -> prec -> succ = node -> succ;
				else head = node -> succ;
				if (node -> succ) node -> succ -> prec = node -> prec;
				else tail = node -> prec;
				delete node;
			}
		}

		valueType *load(const locType &offset)
		{
			++ attempt_count;
			typename map<locType , CacheNode*>::iterator it = table.find(offset);
			CacheNode *node;
			if (it == table.end())
			{
				++ miss_count;
				node = new CacheNode (f_read(offset) , offset , this);
				if (block_lim == table.size())
				{
					CacheNode *tmp = tail;
					if (tail = tail -> prec) tail -> succ = nullptr;
					else  head = nullptr;
					delete tmp;
				}
				if (head == nullptr) head = tail = node;
				else (node -> succ = head) -> prec = node , head = node;
			}
			else
			{
				node = it -> second;
				if (node != head)
				{
					node -> prec -> succ = node -> succ;
					if (node -> succ) node -> succ -> prec = node -> prec;
					else tail = node -> prec;
					node -> prec = nullptr , (node -> succ = head) -> prec = node , head = node;
				}
			}
			return &(node -> value);
		}

		void dirty_page_set(const locType &offset) {table.find(offset) -> second -> is_dirty_page = true;}

		~LRUCache()
		{
			for (;head;head = tail) tail = head -> succ , delete head;
			std::cerr << "Attempt count: " << attempt_count << std::endl;
			std::cerr << "Miss count: " << miss_count << std::endl;
			std::cerr << "Miss rate: " << static_cast<long double> (miss_count) / attempt_count * 100 << "%" << std::endl;
		}
	};
}

#endif //TICKETSYSTEM_LRUCACHE_HPP
