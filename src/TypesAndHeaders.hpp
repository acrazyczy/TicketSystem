//
// Created by crazy_cloud on 2020/5/31.
//

#ifndef TICKETSYSTEM_TYPESANDHEADERS_HPP
#define TICKETSYSTEM_TYPESANDHEADERS_HPP

#include "StringHasher.hpp"
#include "timeType.hpp"
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <cstring>
#include <utility>
#include <string>
#include <cstdio>

namespace sjtu
{
	typedef long locType;

	template <typename T>
	T min(const T &lhs , const T &rhs){return lhs < rhs ? lhs : rhs;}

	template <typename T>
	T max(const T &lhs , const T &rhs){return lhs > rhs ? lhs : rhs;}
};

#endif //TICKETSYSTEM_TYPESANDHEADERS_HPP
