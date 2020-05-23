#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu {

class exception {
protected:
	const std::string variant = "";
	std::string detail = "";
public:
	exception() {}
	exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
	virtual std::string what() {
		return variant + " " + detail;
	}
};

class index_out_of_bound : public exception {
public:
	std::string wro;
	index_out_of_bound() {
	    wro = "index_out_of_bound";
	}

};

class runtime_error : public exception {
public:
	std::string wro;
	runtime_error() {
	    wro = "runtime_error";
	}
};

class invalid_iterator : public exception {
public:
	std::string wro;
	invalid_iterator() {
	    wro = "invalid_iterator";
	}
};

class container_is_empty : public exception {
public:
	std::string wro;
	container_is_empty() {
	    wro = "container_is_empty";
	}
};
}

#endif
