#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <iterator>

namespace sjtu {
template<typename T>
class vector {
public:
        T *data;
        int currentLength;
        int maxSize;
        void doubleSpace() {
                T *tmp = data;
                maxSize *= 2;
                data = (T*) (operator new (maxSize * sizeof(T)));
                for (int i = 0; i < currentLength; ++i)
                        new (data + i) T(tmp[i]);
        for (T* p = tmp; p < tmp + currentLength; ++p)
            p->~T();
                operator delete (tmp);
        };
        class const_iterator;
        class iterator: public std::iterator<std::random_access_iterator_tag, T> {
        private:
                friend const_iterator;
        public:
        T *pos;
        vector *vec;
        iterator (T *obj1 = NULL, vector *obj2 = NULL) {
            pos = obj1;
            vec = obj2;
        }
                iterator operator+(const int &n) const {
                        return iterator(pos + n, vec);
                }
                iterator operator-(const int &n) const {
            return iterator(pos - n, vec);
                }
                /*int operator-(const iterator &rhs) const {
                        if (vec != rhs.vec) throw invalid_iterator();
                        return (rhs.pos - pos);
                }*/
                iterator& operator+=(const int &n) {
            pos += n;
            return *this;
                }
                iterator& operator-=(const int &n) {
            pos -= n;
            return *this;
                }
                iterator operator++(int) {
            iterator tmp = *this;
            ++pos;
            return tmp;
                }
                iterator& operator++() {
            ++pos;
            return *this;
                }
                iterator operator--(int) {
            iterator tmp = *this;
            --pos;
            return tmp;
                }
                iterator& operator--() {
            --pos;
            return *this;
                }
                T& operator*() const{
                    return *pos;
                }
                bool operator<(const iterator &rhs) const {return pos < rhs.pos;}
                typename std::iterator<std::random_access_iterator_tag, T>::difference_type operator-(const iterator &rhs) const {return pos - rhs.pos;}
                bool operator==(const iterator &rhs) const {
                    if (pos == rhs.pos) return true;
                    else return false;
                }
                bool operator==(const const_iterator &rhs) const {
            if (pos == rhs.pos) return true;
            else return false;
                }
                bool operator!=(const iterator &rhs) const {
            if (pos == rhs.pos) return false;
            else return true;
                }
                bool operator!=(const const_iterator &rhs) const {
            if (pos == rhs.pos) return false;
            else return true;
                }
        };
        class const_iterator: public std::iterator<std::random_access_iterator_tag, T> {
    private:
        friend iterator;
    public:
        const T *pos;
        const vector *vec;
        const_iterator (T *obj1 = NULL, const vector *obj2 = NULL) {
            pos = obj1;
            vec = obj2;
        }
        const_iterator operator+(const int &n) const {
            return iterator(pos + n, vec);
        }
        const_iterator operator-(const int &n) const {
            return iterator(pos - n, vec);
        }
        /*int operator-(const iterator &rhs) const {
            if (vec != rhs.vec) throw invalid_iterator();
            return (rhs - pos);
        }*/
        const_iterator& operator+=(const int &n) {
            pos += n;
            return *this;
        }
        const_iterator& operator-=(const int &n) {
            pos -= n;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++pos;
            return tmp;
        }
        const_iterator& operator++() {
            ++pos;
            return *this;
        }
        iterator operator--(int) {
            const_iterator tmp = *this;
            --pos;
            return tmp;
        }
        const_iterator& operator--() {
            --pos;
            return *this;
        }
        const T& operator*() const{
            return *pos;
        }
        bool operator<(const const_iterator &rhs) const {return pos < rhs.pos;}
            typename std::iterator<std::random_access_iterator_tag, T>::difference_type operator-(const const_iterator &rhs) const {return pos - rhs.pos;}
        bool operator==(const iterator &rhs) const {
            if (pos == rhs.pos) return true;
            else return false;
        }
        bool operator==(const const_iterator &rhs) const {
            if (pos == rhs.pos) return true;
            else return false;
        }
        bool operator!=(const iterator &rhs) const {
            if (pos == rhs.pos) return false;
            else return true;
        }
        bool operator!=(const const_iterator &rhs) const {
            if (pos == rhs.pos) return false;
            else return true;
        }
        };

        vector() {
                maxSize = 10;
        data = (T*) (operator new (maxSize * sizeof(T)));
                currentLength = 0;
        }
        vector(const vector &other) : maxSize(other.maxSize) , currentLength(other.currentLength) {
        	data = (T*) (operator new (maxSize * sizeof(T)));
        	for (int i = 0; i < currentLength; ++i)
			new (data + i) T(other.data[i]);
        }
        ~vector() {
            for (T* p = data; p < data + currentLength; ++p)
                    p->~T();
                operator delete (data);
        }
        vector &operator=(const vector &other) {
                if (this == &other) return *this;
        for (T* p = data; p < data + currentLength; ++p)
            p->~T();
        operator delete (data);
                maxSize = other.maxSize;
        data = (T*) (operator new (maxSize * sizeof(T)));
                currentLength = other.currentLength;
                for (int i = 0; i < currentLength; ++i)
                        data[i] = other.data[i];
                return *this;
        }
        T & at(const size_t &pos) {
        if (pos < 0 || pos >= currentLength) throw index_out_of_bound();
        return data[pos];
        }
        const T & at(const size_t &pos) const {
        if (pos < 0 || pos >= currentLength) throw index_out_of_bound();
        return data[pos];
        }

        T & operator[](const size_t &pos) {
                if (pos < 0 || pos >= currentLength) throw index_out_of_bound();
                return data[pos];
        }
        const T & operator[](const size_t &pos) const {
                if (pos < 0 || pos >= currentLength) throw index_out_of_bound();
                return data[pos];
        }
        const T & front() const {
                if (currentLength == 0) throw container_is_empty();
                return data[0];
        }
        const T & back() const {
                if (currentLength == 0) throw container_is_empty();
                return data[currentLength - 1];
        }
        iterator begin() {
            return iterator(data, this);
        }
        const_iterator cbegin() const {
            return const_iterator(data, this);
        }
        iterator end() {
            return iterator(data + currentLength, this);
        }
        const_iterator cend() const {
        return const_iterator(data + currentLength, this);
        }
        bool empty() const {
                if (currentLength == 0) return true;
                else return false;
        }
        size_t size() const {
                return currentLength;
        }
        void clear() {
                currentLength = 0;
        }
        iterator insert(iterator pos, const T &value) {
            T *q;
            if (currentLength == maxSize) {
                int offset = pos.pos - data;
                doubleSpace();
                pos.pos = data + offset;
            }
            q = data + currentLength;
            while (q > pos.pos) {
                *q = *(q - 1);
                --q;
            }
            new(pos.pos) T(value);
            ++currentLength;
            return pos;
        }
        iterator insert(const size_t &ind, const T &value) {
            if (ind > maxSize) throw index_out_of_bound();
            if (currentLength == maxSize) doubleSpace();
            for (int i = currentLength; i > ind; --i)
                data[i] = data[i - 1];
        new(data + ind) T(value);
        ++currentLength;
        return iterator(data + ind, this);
        }
        iterator erase(iterator pos) {
            T *q = pos.pos;
            --currentLength;
            while (q < data + currentLength) {
            *q = *(q + 1);
            ++q;
            }
            T *p = data + currentLength + 1;
            p->~T();
            return pos;
        }
        iterator erase(const size_t &ind) {
            if (ind >= maxSize) throw index_out_of_bound();
            for (int i = ind; i < currentLength - 1; ++i)
                data[i] = data[i + 1];
            --currentLength;
        T *p = data + currentLength + 1;
        p->~T();
            return iterator(data + ind, this);
        }
        void push_back(const T &value) {
                if (currentLength == maxSize) doubleSpace();
        new(data + currentLength) T(value);
                currentLength++;
        }
        void pop_back() {
                if (currentLength == 0) throw container_is_empty();
                currentLength--;
        T *p = data + currentLength + 1;
        p->~T();
        }
};

}

#endif
