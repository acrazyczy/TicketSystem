#ifndef BplusTree_HPP
#define BplusTree_HPP
#include<cstdio>
#include<iostream>
#include<cstring>
#include<fstream>
#include<vector>
template<
        class Key,
        class T,
        int degree,
        class Compare = std::less<Key>
> class BplusTree {
    typedef std::pair<const Key, T> value_type;
public:
    int depth;

    struct node {
        union{
            T val[degree-1];
            long pointer[degree];
        };
        long nxt;
        Key keyvalue[degree - 1];
        int sz;

        node() {
            memset(this, 0, sizeof *this);
            sz = 0;
        }

        void printnonleaf() {
            int i;
            for (i = 0; i < sz; ++i)std::cout << keyvalue[i] << ' ';
            std::cout << std::endl;
            for (i = 0; i <= sz; ++i)std::cout << pointer[i] << ' ';
            std::cout << std::endl;
        }
        void printleaf(){
            int i;
            for (i = 0; i < sz; ++i)std::cout << keyvalue[i] << ' ';
            std::cout << std::endl;
            for (i = 0; i < sz; ++i)std::cout << val[i] << ' ';
            std::cout <<nxt<< std::endl;
        }
    };

    void print2(long pos, int cur, std::ifstream &fin) {
        fin.seekg(pos);
        node x;
        fin.read(reinterpret_cast<char *>(&x), sizeof(node));
        std::cout << pos << ':' << std::endl;
        if (cur < depth) {
            x.printnonleaf();
            for (int i = 0; i <= x.sz; ++i)print2(x.pointer[i], cur + 1, fin);
        }
        else{
            x.printleaf();
        }
    }

    void print(long pos) {
        long rt;
        std::ifstream fin("file.txt");
        fin.read(reinterpret_cast<char *>(&rt), sizeof(long));
        if (rt == -1)std::cout << "empty" << std::endl;
        else print2(rt, 1, fin);
        std::cout << std::endl;
        fin.close();
        return;
    }

    bool equal(Key x, Key y) {
        return !Compare()(x, y) && !Compare()(y, x);
    }

    BplusTree() {
        depth = 0;
        std::ofstream fout("file.txt");
        long rt = -1;
        fout.write(reinterpret_cast<char *>(&rt), sizeof(long));
        fout.close();
    }

    std::pair<T, bool> find(const Key &key) {
        T ret;
        if (!depth)return std::make_pair(ret, 0);
        std::ifstream fin("file.txt");
        long pos;
        node x;
        int i, j;
        fin.read(reinterpret_cast<char *>(&pos), sizeof(long));
        for (i = 0; i < depth; ++i) {
            fin.seekg(pos);
            fin.read(reinterpret_cast<char *>(&x), sizeof(node));
            for (j = 0; j < x.sz && Compare()(x.keyvalue[j], key); ++j);
            if(i<depth-1){
                if(j < x.sz && !Compare()(key, x.keyvalue[j]))pos = x.pointer[j+1];
                else pos=x.pointer[j];
            }
        }
        if (j == x.sz || Compare()(key, x.keyvalue[j]))
            return std::make_pair(ret, false);
        ret=x.val[j];
        fin.close();
        return std::make_pair(ret, true);
    }
    std::vector<T> range_query(const Key &keyl,const Key &keyr){
        std::vector<T>ret;
        if(!depth)return ret;
        std::ifstream fin("file.txt");
        long pos;
        node x;
        int i, j;
        fin.read(reinterpret_cast<char *>(&pos), sizeof(long));
        for (i = 0; i < depth; ++i) {
            fin.seekg(pos);
            fin.read(reinterpret_cast<char *>(&x), sizeof(node));
            for (j = 0; j < x.sz && Compare()(x.keyvalue[j], keyl); ++j);
            if(i<depth-1){
                if(j < x.sz && !Compare()(keyl, x.keyvalue[j]))pos = x.pointer[j+1];
                else pos=x.pointer[j];
            }
        }
        while(1){
            if(j==x.sz){
                if(x.nxt==-1)break;
                fin.seekg(x.nxt);
                fin.read(reinterpret_cast<char *>(&x),sizeof(node));
                j=0;
            }
            if(Compare()(keyr,x.keyvalue[j]))break;
            ret.push_back(x.val[j++]);
        }
        return ret;
    }
    void insertnonleaf(const Key &key, const int &cur, node *p, int *pp, long *pos, long wz, std::fstream &fio) {
        if (cur == -1) {
            node x;
            ++depth;
            fio.seekg(0, std::ios::end);
            long rt = fio.tellg();
            fio.seekp(0);
            fio.write(reinterpret_cast<char *>(&rt), sizeof(long));
            x.sz = 1;
            x.keyvalue[0] = key;
            x.pointer[0] = pos[0];
            x.pointer[1] = wz;
            fio.seekp(rt);
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            return;
        }
        node &x = p[cur];
        int i, j = pp[cur];
        if (x.sz < degree - 1) {
            for (i = x.sz; i > j; --i)x.keyvalue[i] = x.keyvalue[i - 1], x.pointer[i + 1] = x.pointer[i];
            x.keyvalue[j] = key;
            x.pointer[j + 1] = wz;
            ++x.sz;
            fio.seekp(pos[cur]);
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
        } else {
            long tmpp[degree + 1];
            Key tmpk[degree];
            tmpp[0] = x.pointer[0];
            for (i = 0; i < j; ++i)tmpp[i + 1] = x.pointer[i + 1], tmpk[i] = x.keyvalue[i];
            tmpp[j + 1] = wz;
            tmpk[j] = key;
            for (i = j + 1; i < degree; ++i)tmpp[i + 1] = x.pointer[i], tmpk[i] = x.keyvalue[i - 1];
            x.sz = degree >> 1;
            for (i = 0; i < x.sz; ++i)x.pointer[i] = tmpp[i], x.keyvalue[i] = tmpk[i];
            x.pointer[x.sz] = tmpp[i];
            node y;
            for (i = x.sz + 1; i < degree; ++i)y.pointer[i - x.sz - 1] = tmpp[i], y.keyvalue[i - x.sz - 1] = tmpk[i];
            y.sz = degree - 1 - x.sz;
            y.pointer[y.sz] = tmpp[degree];
            fio.seekp(0, std::ios::end);
            wz = fio.tellp();
            fio.write(reinterpret_cast<char *>(&y), sizeof(node));
            fio.seekp(pos[cur]);
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            insertnonleaf(tmpk[x.sz], cur - 1, p, pp, pos, wz, fio);
        }
    }

    bool insert(const value_type &value) {
        Key vkey = value.first;
        T vt = value.second;
        std::fstream fio("file.txt");
        int i, j;
        if (!depth) {
            node x;
            depth = 1;
            long pos = sizeof(long);
            fio.write(reinterpret_cast<char *>(&pos), sizeof(long));
            x.sz = 1;
            x.keyvalue[0] = vkey;
            x.val[0] = vt;
            x.nxt = -1;
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            fio.close();
            return 1;
        }
        long *pos;
        node *p;
        int *pp;
        pos = new long[depth];memset(pos,0,sizeof pos);
        p = new node[depth];
        pp = new int[depth];
        fio.read(reinterpret_cast<char *>(&pos[0]), sizeof(long));
        for (i = 0; i < depth; ++i) {
            fio.seekg(pos[i]);
            fio.read(reinterpret_cast<char *>(&p[i]), sizeof(node));
            for (j = 0; j < p[i].sz && Compare()(p[i].keyvalue[j], vkey); ++j);
            if(i<depth-1){
                if(j < p[i].sz && !Compare()(vkey, p[i].keyvalue[j]))++j;
                pos[i+1]= p[i].pointer[j];
            }
            pp[i] = j;
        }
        if (j < p[depth - 1].sz && !Compare()(vkey, p[depth - 1].keyvalue[j])) {
            delete[]p;
            delete[]pp;
            delete[]pos;
            return 0;
        }
        node &x = p[depth - 1];
        if (!j) {
            for (i = depth - 2; i >= 0; --i)
                if (pp[i]&&equal(x.keyvalue[0], p[i].keyvalue[pp[i]-1])) {
                    p[i].keyvalue[pp[i]-1] = vkey;
                    break;
                }
            if (i >= 0) {
                fio.seekp(pos[i]);
                fio.write(reinterpret_cast<char *>(&p[i]), sizeof(node));
            }
        }
        long wz;
    //    fio.seekp(0, std::ios::end);
     //   fio.write(reinterpret_cast<char *>(&vt), sizeof(T));
        if (x.sz < degree - 1) {
            for (i = x.sz; i > j; --i)x.keyvalue[i] = x.keyvalue[i - 1], x.val[i] = x.val[i - 1];
            x.keyvalue[j] = vkey;
            x.val[j] = vt;
            ++x.sz;
            fio.seekp(pos[depth - 1]);
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
        } else {
            T tmpv[degree];
            Key tmpk[degree];
            node y;
            for (i = 0; i < j; ++i)tmpv[i] = x.val[i], tmpk[i] = x.keyvalue[i];
            tmpv[j] = vt;
            tmpk[j] = vkey;
            for (i = j + 1; i < degree; ++i)tmpv[i] = x.val[i - 1], tmpk[i] = x.keyvalue[i - 1];
            y.nxt=x.nxt;
            x.sz = degree >> 1;
            for (i = 0; i < x.sz; ++i)x.val[i] = tmpv[i], x.keyvalue[i] = tmpk[i];
            fio.seekp(0,std::ios::end);
            x.nxt = fio.tellp();
            for (i = x.sz; i < degree; ++i)y.val[i - x.sz] = tmpv[i], y.keyvalue[i - x.sz] = tmpk[i];
            y.sz = degree - x.sz;
            wz = fio.tellp();
            fio.write(reinterpret_cast<char *>(&y), sizeof(node));
            fio.seekp(pos[depth - 1]);
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            insertnonleaf(y.keyvalue[0], depth - 2, p, pp, pos, wz, fio);
        }
        fio.close();
        delete[]p;
        delete[]pp;
        delete[]pos;
        return 1;
    }

    void erasenonleaf(const int &cur, node *p, int *pp, long *pos, std::fstream &fio) {
        node &x = p[cur];
        int i, j = pp[cur];
        for (i = j + 1; i < x.sz; ++i)x.keyvalue[i - 1] = x.keyvalue[i], x.pointer[i] = x.pointer[i + 1];
        --x.sz;
        if (!cur) {
            if (!x.sz) {
                --depth;
                fio.seekp(0);
                fio.write(reinterpret_cast<char *>(&pos[1]), sizeof(long));
                //(del rt in file.)
            } else {
                fio.seekp(pos[0]);
                fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            }
            return;
        }
        if (x.sz >= degree - 1 >> 1) {
            fio.seekp(pos[cur]);
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            return;
        }
        if (pp[cur - 1] == p[cur - 1].sz) {
            p[cur + 1] = p[cur];
            pp[cur + 1] = pp[cur - 1];
            pos[cur + 1] = pos[cur];
            pp[cur - 1] = p[cur - 1].sz - 1;
            pos[cur] = p[cur - 1].pointer[pp[cur - 1]];
            fio.seekg(pos[cur]);
            fio.read(reinterpret_cast<char *>(&p[cur]), sizeof(node));
        } else {
            pp[cur + 1] = pp[cur - 1] + 1;
            pos[cur + 1] = p[cur - 1].pointer[pp[cur + 1]];
            fio.seekg(pos[cur + 1]);
            fio.read(reinterpret_cast<char *>(&p[cur + 1]), sizeof(node));
        }
        node &y = p[cur + 1];
        if (x.sz + y.sz < degree - 1) {
            x.keyvalue[x.sz] = p[cur - 1].keyvalue[pp[cur - 1]];
            for (i = 0; i < y.sz; ++i)x.pointer[x.sz + 1 + i] = y.pointer[i], x.keyvalue[x.sz + 1 + i] = y.keyvalue[i];
            x.pointer[x.sz += 1 + y.sz] = y.pointer[y.sz];
            fio.seekp(pos[cur]);
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            //(del y in file)
            erasenonleaf(cur - 1, p, pp, pos, fio);
        } else {
            int newsz = x.sz + y.sz + 1;
            long tmpp[newsz + 1];
            Key tmpk[newsz], old = y.keyvalue[0];
            for (i = 0; i < x.sz; ++i)tmpp[i] = x.pointer[i], tmpk[i] = x.keyvalue[i];
            tmpp[x.sz] = x.pointer[x.sz];
            tmpk[x.sz] = p[cur - 1].keyvalue[pp[cur - 1]];
            for (i = x.sz + 1; i < newsz; ++i)tmpp[i] = y.pointer[i - x.sz - 1], tmpk[i] = y.keyvalue[i - x.sz - 1];
            tmpp[newsz] = y.pointer[y.sz];
            x.sz = newsz >> 1;
            y.sz = newsz - x.sz-1;
            for (i = 0; i < x.sz; ++i)x.pointer[i] = tmpp[i], x.keyvalue[i] = tmpk[i];
            x.pointer[x.sz] = tmpp[x.sz];
            p[cur - 1].keyvalue[pp[cur - 1]] = tmpk[x.sz];
            for (i = 0; i < y.sz; ++i)y.pointer[i] = tmpp[i + x.sz + 1], y.keyvalue[i] = tmpk[i + x.sz + 1];
            y.pointer[y.sz] = tmpp[newsz];
            fio.seekp(pos[cur]);
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            fio.seekp(pos[cur + 1]);
            fio.write(reinterpret_cast<char *>(&y), sizeof(node));
            fio.seekp(pos[cur - 1]);
            fio.write(reinterpret_cast<char *>(&p[cur - 1]), sizeof(node));
        }
    }

    bool erase(const Key &key) {
        if (!depth)return 0;
        std::fstream fio("file.txt");
        int i, j;
        long *pos;
        node *p;
        int *pp;
        pos = new long[depth + 1];memset(pos,0,sizeof pos);
        p = new node[depth + 1];
        pp = new int[depth + 1];
        fio.read(reinterpret_cast<char *>(&pos[0]), sizeof(int));
        for (i = 0; i < depth; ++i) {
            fio.seekg(pos[i]);
            fio.read(reinterpret_cast<char *>(&p[i]), sizeof(node));
            for (j = 0; j < p[i].sz && Compare()(p[i].keyvalue[j], key); ++j);
            if(i<depth-1){
                if(j < p[i].sz && !Compare()(key, p[i].keyvalue[j]))++j;
                pos[i+1]= p[i].pointer[j];
            }
            pp[i] = j;
        }
        if (j == p[depth - 1].sz || Compare()(key, p[depth - 1].keyvalue[j])) {
            delete[]p;
            delete[]pp;
            delete[]pos;
            return 0;
        }
        node &x = p[depth - 1];
        if (!j) {
            for (i = depth - 2; i >= 0; --i)
                if (pp[i]&&equal(x.keyvalue[0], p[i].keyvalue[pp[i]-1])) {
                    p[i].keyvalue[pp[i]-1] = x.keyvalue[1];
                    break;
                }
            if (i >= 0) {
                fio.seekp(pos[i]);
                fio.write(reinterpret_cast<char *>(&p[i]), sizeof(node));
            }
        }
        //(del x.pointer[j] in file.)
        for (i = j + 1; i < x.sz; ++i)x.keyvalue[i - 1] = x.keyvalue[i], x.val[i - 1] = x.val[i];
        --x.sz;
        if (depth == 1) {
            if (!x.sz) {
                depth = 0;
                //(del rt in file.)
                fio.seekp(0);
                long rt = -1;
                fio.write(reinterpret_cast<char *>(&rt), sizeof(long));
            } else {
                fio.seekp(pos[0]);
                fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            }
            delete[]p;
            delete[]pp;
            delete[]pos;
            return 1;
        }
        if (x.sz >= degree >> 1) {
            fio.seekp(pos[depth - 1]);
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            delete[]p;
            delete[]pp;
            delete[]pos;
            return 1;
        }
        if (pp[depth - 2] == p[depth - 2].sz) {
            p[depth] = p[depth - 1];
            pp[depth] = pp[depth - 2];
            pos[depth] = pos[depth - 1];
            pp[depth - 2] = p[depth - 2].sz - 1;
            pos[depth - 1] = p[depth - 2].pointer[pp[depth - 2]];
            fio.seekg(pos[depth - 1]);
            fio.read(reinterpret_cast<char *>(&p[depth - 1]), sizeof(node));
        } else {
            pp[depth] = pp[depth - 2] + 1;
            pos[depth] = p[depth - 2].pointer[pp[depth]];
            fio.seekg(pos[depth]);
            fio.read(reinterpret_cast<char *>(&p[depth]), sizeof(node));
        }
        node &y = p[depth];
        if (x.sz + y.sz < degree) {
            for (i = 0; i < y.sz; ++i)x.val[x.sz + i] = y.val[i], x.keyvalue[x.sz + i] = y.keyvalue[i];
            x.nxt=y.nxt;x.sz+=y.sz;
            fio.seekp(pos[depth - 1]);
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            //(del y in file)
            erasenonleaf(depth - 2, p, pp, pos, fio);
        } else {
            int newsz = x.sz + y.sz;
            T tmpv[newsz + 1];
            Key tmpk[newsz], old = y.keyvalue[0];
            for (i = 0; i < x.sz; ++i)tmpv[i] = x.val[i], tmpk[i] = x.keyvalue[i];
            for (i = x.sz; i < newsz; ++i)tmpv[i] = y.val[i - x.sz], tmpk[i] = y.keyvalue[i - x.sz];
            x.sz = newsz + 1 >> 1;
            y.sz = newsz - x.sz;
            for (i = 0; i < x.sz; ++i)x.val[i] = tmpv[i], x.keyvalue[i] = tmpk[i];
            for (i = 0; i < y.sz; ++i)y.val[i] = tmpv[i + x.sz], y.keyvalue[i] = tmpk[i + x.sz];
            fio.seekp(pos[depth - 1]);
            fio.write(reinterpret_cast<char *>(&x), sizeof(node));
            fio.seekp(pos[depth]);
            fio.write(reinterpret_cast<char *>(&y), sizeof(node));
            for (i = depth - 2; i >= 0; --i)
                if (equal(old, p[i].keyvalue[pp[i]])) {
                    p[i].keyvalue[pp[i]] = y.keyvalue[0];
                    break;
                }
            fio.seekp(pos[i]);
            fio.write(reinterpret_cast<char *>(&p[i]), sizeof(node));
        }
        fio.close();
        delete[]p;
        delete[]pp;
        delete[]pos;
        return 1;
    }
};
#endif