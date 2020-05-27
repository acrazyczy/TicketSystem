//
// Created by crazy_cloud on 2020/5/25.
//

#ifndef TICKETSYSTEM_LRUCACHE_HPP
#define TICKETSYSTEM_LRUCACHE_HPP

#include <functional>
#include "map.h"

namespace sjtu
{
	typedef unsigned long long locType;

	template <class valueType>
	class LRUCache
	{
	private:
		typedef std::function<valueType(const locType &)> freadType;
		typedef std::function<void(const locType & , const valueType &)> fwriteType;

		freadType f_read;
		fwriteType f_write;

		std::size_t block_lim;

		map<locType , *CacheNode> table;

		struct CacheNode
		{
			valueType value;
			locType offset;
			bool is_dirty_page;
			CacheNode *prec , *succ;

			CacheNode(const valueType &value_ , const locType &offset_) : value(value_) , offset(offset_) , is_dirty_page(false) , prec(nullptr) , succ(nullptr)
			{
				table.insert(std::pair(locType , this));
			}

			~CacheNode()
			{
				if (dirty_page_set) f_write(offset , value);
				table.erase(table.find(offset));
			}
		}*head , *tail;

	public:
		LRUCache(std::size_t block_lim_ , freadType f_read_ , fwriteType f_write_) : block_lim(block_lim_) , block_cnt(0) , f_read(f_read_) , f_write(f_write_) , head(nullptr) , tail(nullptr) {}
		LRUCache(const LRUCache &) = delete;

		LRUCache &operator=(const LRUCache &) = delete;

		void remove(const locType &offset)
		{
			map<locType , *CacheNode>::iterator it = table.find(offset);
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
			map<locType , *CacheNode>::iterator it = table.find(offset);
			CacheNode *node;
			if (it == table.end())
			{
				node = new CacheNode (f_read(offset) , offset);
				if (block_lim == table.size())
				{
					CacheNode *tmp = tail;
					if (tail = tail -> prec) tail -> succ = nullptr;
					if (!tail) head = nullptr;
					delete tmp;
				}
				if (head == nullptr) head = tail = node;
				else (node -> prec = tail) -> succ = node , tail = node;
			}
			else
			{
				node = it -> second;
				if (node != head)
				{
					node -> prec -> succ = node -> succ;
					if (node -> succ) node -> succ -> prec = node -> prec;
					node -> prec = nullptr , node -> succ = head , head = node;
				}
			}
			return node -> value;
		}

		void dirty_page_set(const locType &offset) {table.find(offset) -> second -> is_dirty_page = true;}

		~LRUCache() {for (;head;head = tail) tail = head -> succ , delete head;}
	};
}

#endif //TICKETSYSTEM_LRUCACHE_HPP
