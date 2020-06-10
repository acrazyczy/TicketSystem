//
// Created by crazy_cloud on 2020/6/10.
//

#ifndef TICKETSYSTEM_STRINGHASHER_HPP
#define TICKETSYSTEM_STRINGHASHER_HPP

#include <cstring>
#include <string>

namespace sjtu
{
	class StringHasher
	{
		static const int MOD0 = 998244353;
		static const int MOD1 = 1000000007;
		static const int P0 = 569;
		static const int P1 = 769;

	public:
		typedef unsigned long long hashType;

		hashType operator()(const std::string str)
		{
			int ret0 = 0 , ret1 = 0;
			for (std::string::iterator it = str.begin();it != str.end();++ it)
				ret0 = (1ll * ret0 * P0 + (*it)) % MOD0 , ret1 = (1ll * ret1 * P1 + (*it)) % MOD1;
			return static_cast<hashType>(ret0) << 32 | static_cast<hashType>(ret1);
		}
	};

	const int StringHasher::MOD0;
	const int StringHasher::MOD1;
	const int StringHasher::P0;
	const int StringHasher::P1;
}

#endif //TICKETSYSTEM_STRINGHASHER_HPP
