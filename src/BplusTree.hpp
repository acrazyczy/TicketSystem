#ifndef BplusTree_HPP
#define BplusTree_HPP
#include<cstdio>
#include<iostream>
#include<cstring>
#include<fstream>
#include<vector>
#include "FileManager.hpp"
#include "vector.hpp"
namespace sjtu {
    template<
            class Key,
            class T,
            int degree = 100,
            class Compare = std::less<Key>
    >
    class BplusTree {
        typedef std::pair<const Key, T> value_type;
    public:
        int depth;
        locType root;

        struct node {
            union {
                T val[degree - 1];
                locType pointer[degree];
            };
            locType nxt;
            Key keyvalue[degree - 1];
            int sz;

            node() {
                memset(this, 0, sizeof *this);
                sz = 0;
            }

            /*         void printnonleaf() {
                         int i;
                         for (i = 0; i < sz; ++i)std::cout << keyvalue[i] << ' ';
                         std::cout << std::endl;
                         for (i = 0; i <= sz; ++i)std::cout << pointer[i] << ' ';
                         std::cout << std::endl;
                     }

                     void printleaf() {
                         int i;
                         for (i = 0; i < sz; ++i)std::cout << keyvalue[i] << ' ';
                         std::cout << std::endl;
                         for (i = 0; i < sz; ++i)std::cout << val[i] << ' ';
                         std::cout << nxt << std::endl;
                     }*/
        };

        DynamicFileManager<node> *file;

        /*      void print2(locType pos, int cur, std::ifstream &fin) {
                  fin.seekg(pos);
                  node x;
                  fin.read(reinterpret_cast<char *>(&x), sizeof(node));
                  std::cout << pos << ':' << std::endl;
                  if (cur < depth) {
                      x.printnonleaf();
                      for (int i = 0; i <= x.sz; ++i)print2(x.pointer[i], cur + 1, fin);
                  } else {
                      x.printleaf();
                  }
              }

              void print(locType pos) {
                  locType rt;
                  std::ifstream fin("file.txt");
                  fin.read(reinterpret_cast<char *>(&rt), sizeof(locType));
                  if (rt == -1)std::cout << "empty" << std::endl;
                  else print2(rt, 1, fin);
                  std::cout << std::endl;
                  fin.close();
                  return;
              }*/

        bool empty() {
            return root == -1;
        }

        bool equal(Key x, Key y) {
            return !Compare()(x, y) && !Compare()(y, x);
        }

        void writeroot(locType _root) {
            node *x = file->read(sizeof(DynamicFileManager<node>::blockType));
            x->nxt = _root;
            file->save(sizeof(DynamicFileManager<node>::blockType));
        }

        locType readroot() {
            return file->read(sizeof(DynamicFileManager<node>::blockType))->nxt;
        }

        BplusTree(std::string str, bool is_reset = false) {
            depth = 0;
            if (is_reset) {
                file->init(str, true);
                root = -1;
            } else {
                file = new DynamicFileManager<node>(str);
                if (file->is_newfile)
                    root = -1;
                else
                    root = readroot();
            }
        }

        ~BplusTree() {
            writeroot(root);
        }

        std::pair<T, bool> find(const Key &key) {
            T ret;
            if (!depth)return std::make_pair(ret, 0);
            locType pos = root;
            node *x;
            int i, j;
            for (i = 0; i < depth; ++i) {
                x = file->read(pos);
                for (j = 0; j < x->sz && Compare()(x->keyvalue[j], key); ++j);
                if (i < depth - 1) {
                    if (j < x->sz && !Compare()(key, x->keyvalue[j]))pos = x->pointer[j + 1];
                    else pos = x->pointer[j];
                }
            }
            if (j == x->sz || Compare()(key, x->keyvalue[j]))
                return std::make_pair(ret, false);
            ret = x->val[j];
            return std::make_pair(ret, true);
        }

        vector<value_type> range_query(const Key &keyl, const Key &keyr) {
            std::vector<value_type> ret;
            if (!depth)return ret;
            locType pos = root;
            node *x;
            int i, j;
            for (i = 0; i < depth; ++i) {
                x = file->read(pos);
                for (j = 0; j < x->sz && Compare()(x->keyvalue[j], keyl); ++j);
                if (i < depth - 1) {
                    if (j < x->sz && !Compare()(keyl, x->keyvalue[j]))pos = x->pointer[j + 1];
                    else pos = x->pointer[j];
                }
            }
            while (1) {
                if (j == x->sz) {
                    if (x->nxt == -1)break;
                    x = file->read(x->nxt);
                    j = 0;
                }
                if (Compare()(keyr, x->keyvalue[j]))break;
                ret.push_back(std::make_pair(x->keyvalue[j], x->val[j]));
                ++j;
            }
            return ret;
        }

        void insertnonleaf(const Key &key, const int &cur, node **p, int *pp, locType *pos, locType wz) {
            if (cur == -1) {
                auto lnk = file->newspace();
                root = lnk.second;
                node *x = lnk.first;
                ++depth;
                x->sz = 1;
                x->keyvalue[0] = key;
                x->pointer[0] = pos[0];
                x->pointer[1] = wz;
                file->save(root);
                return;
            }
            node *&x = p[cur];
            int i, j = pp[cur];
            if (x->sz < degree - 1) {
                for (i = x->sz; i > j; --i)x->keyvalue[i] = x->keyvalue[i - 1], x->pointer[i + 1] = x->pointer[i];
                x->keyvalue[j] = key;
                x->pointer[j + 1] = wz;
                ++x->sz;
                file->save(pos[cur]);
            } else {
                locType tmpp[degree + 1];
                Key tmpk[degree];
                tmpp[0] = x->pointer[0];
                for (i = 0; i < j; ++i)tmpp[i + 1] = x->pointer[i + 1], tmpk[i] = x->keyvalue[i];
                tmpp[j + 1] = wz;
                tmpk[j] = key;
                for (i = j + 1; i < degree; ++i)tmpp[i + 1] = x->pointer[i], tmpk[i] = x->keyvalue[i - 1];
                x->sz = degree >> 1;
                for (i = 0; i < x->sz; ++i)x->pointer[i] = tmpp[i], x->keyvalue[i] = tmpk[i];
                x->pointer[x->sz] = tmpp[i];
                auto lnk = file->newspace();
                node *y = lnk.first;
                for (i = x->sz + 1; i < degree; ++i)
                    y->pointer[i - x->sz - 1] = tmpp[i], y->keyvalue[i - x->sz - 1] = tmpk[i];
                y->sz = degree - 1 - x->sz;
                y->pointer[y->sz] = tmpp[degree];
                file->save(lnk.second);
                file->save(pos[cur]);
                insertnonleaf(tmpk[x->sz], cur - 1, p, pp, pos, lnk.second);
            }
        }

        bool insert(const value_type &value) {
            Key vkey = value.first;
            T vt = value.second;
            int i, j;
            if (!depth) {
                auto lnk = file->newspace();
                node *x = lnk.first;
                root = lnk.second;
                depth = 1;
                x->sz = 1;
                x->keyvalue[0] = vkey;
                x->val[0] = vt;
                x->nxt = -1;
                file->save(root);
                return 1;
            }
            locType *pos;
            node **p;
            int *pp;
            pos = new locType[depth];
            memset(pos, 0, sizeof pos);
            p = new node *[depth];
            pp = new int[depth];
            pos[0] = root;
            for (i = 0; i < depth; ++i) {
                p[i] = file->read(pos[i]);
                for (j = 0; j < p[i]->sz && Compare()(p[i]->keyvalue[j], vkey); ++j);
                if (i < depth - 1) {
                    if (j < p[i]->sz && !Compare()(vkey, p[i]->keyvalue[j]))++j;
                    pos[i + 1] = p[i]->pointer[j];
                }
                pp[i] = j;
            }
            if (j < p[depth - 1]->sz && !Compare()(vkey, p[depth - 1]->keyvalue[j])) {
                delete[]p;
                delete[]pp;
                delete[]pos;
                return 0;
            }
            node *&x = p[depth - 1];
            if (!j) {
                for (i = depth - 2; i >= 0; --i)
                    if (pp[i] && equal(x->keyvalue[0], p[i]->keyvalue[pp[i] - 1])) {
                        p[i]->keyvalue[pp[i] - 1] = vkey;
                        file->save(pos[i]);
                        break;
                    }
            }
            if (x->sz < degree - 1) {
                for (i = x->sz; i > j; --i)x->keyvalue[i] = x->keyvalue[i - 1], x->val[i] = x->val[i - 1];
                x->keyvalue[j] = vkey;
                x->val[j] = vt;
                ++x->sz;
                file->save(pos[depth - 1]);
            } else {
                T tmpv[degree];
                Key tmpk[degree];
                auto lnk = file->newspace();
                node *y = lnk.first;
                for (i = 0; i < j; ++i)tmpv[i] = x->val[i], tmpk[i] = x->keyvalue[i];
                tmpv[j] = vt;
                tmpk[j] = vkey;
                for (i = j + 1; i < degree; ++i)tmpv[i] = x->val[i - 1], tmpk[i] = x->keyvalue[i - 1];
                y->nxt = x->nxt;
                x->sz = degree >> 1;
                for (i = 0; i < x->sz; ++i)x->val[i] = tmpv[i], x->keyvalue[i] = tmpk[i];
                x->nxt = lnk.second;
                for (i = x->sz; i < degree; ++i)y->val[i - x->sz] = tmpv[i], y->keyvalue[i - x->sz] = tmpk[i];
                y->sz = degree - x->sz;
                file->save(lnk.second);
                file->save(pos[depth - 1]);
                insertnonleaf(y->keyvalue[0], depth - 2, p, pp, pos, lnk.second);
            }
            delete[]p;
            delete[]pp;
            delete[]pos;
            return 1;
        }

        void erasenonleaf(const int &cur, node **p, int *pp, locType *pos) {
            node *&x = p[cur];
            int i, j = pp[cur];
            for (i = j + 1; i < x->sz; ++i)x->keyvalue[i - 1] = x->keyvalue[i], x->pointer[i] = x->pointer[i + 1];
            --x->sz;
            if (!cur) {
                if (!x->sz) {
                    --depth;
                    file->release(root);
                    root = pos[1];
                } else {
                    file->save(pos[0]);
                }
                return;
            }
            if (x->sz >= degree - 1 >> 1) {
                file->save(pos[cur]);
                return;
            }
            if (pp[cur - 1] == p[cur - 1]->sz) {
                p[cur + 1] = p[cur];
                pp[cur + 1] = pp[cur - 1];
                pos[cur + 1] = pos[cur];
                pp[cur - 1] = p[cur - 1]->sz - 1;
                pos[cur] = p[cur - 1]->pointer[pp[cur - 1]];
                p[cur] = file->read(pos[cur]);
            } else {
                pp[cur + 1] = pp[cur - 1] + 1;
                pos[cur + 1] = p[cur - 1]->pointer[pp[cur + 1]];
                p[cur + 1] = file->read(pos[cur + 1]);
            }
            node *&y = p[cur + 1];
            if (x->sz + y->sz < degree - 1) {
                x->keyvalue[x->sz] = p[cur - 1]->keyvalue[pp[cur - 1]];
                for (i = 0; i < y->sz; ++i)
                    x->pointer[x->sz + 1 + i] = y->pointer[i], x->keyvalue[x->sz + 1 + i] = y->keyvalue[i];
                x->pointer[x->sz += 1 + y->sz] = y->pointer[y->sz];
                file->save(pos[cur]);
                file->release(pos[cur + 1]);
                erasenonleaf(cur - 1, p, pp, pos);
            } else {
                int newsz = x->sz + y->sz + 1;
                locType tmpp[newsz + 1];
                Key tmpk[newsz], old = y->keyvalue[0];
                for (i = 0; i < x->sz; ++i)tmpp[i] = x->pointer[i], tmpk[i] = x->keyvalue[i];
                tmpp[x->sz] = x->pointer[x->sz];
                tmpk[x->sz] = p[cur - 1]->keyvalue[pp[cur - 1]];
                for (i = x->sz + 1; i < newsz; ++i)
                    tmpp[i] = y->pointer[i - x->sz - 1], tmpk[i] = y->keyvalue[i - x->sz - 1];
                tmpp[newsz] = y->pointer[y->sz];
                x->sz = newsz >> 1;
                y->sz = newsz - x->sz - 1;
                for (i = 0; i < x->sz; ++i)x->pointer[i] = tmpp[i], x->keyvalue[i] = tmpk[i];
                x->pointer[x->sz] = tmpp[x->sz];
                p[cur - 1]->keyvalue[pp[cur - 1]] = tmpk[x->sz];
                for (i = 0; i < y->sz; ++i)y->pointer[i] = tmpp[i + x->sz + 1], y->keyvalue[i] = tmpk[i + x->sz + 1];
                y->pointer[y->sz] = tmpp[newsz];
                file->save(pos[cur]);
                file->save(pos[cur + 1]);
                file->save(pos[cur - 1]);
            }
        }

        bool erase(const Key &key) {
            if (!depth)return 0;
            int i, j;
            locType *pos;
            node **p;
            int *pp;
            pos = new locType[depth + 1];
            memset(pos, 0, sizeof pos);
            p = new node *[depth + 1];
            pp = new int[depth + 1];
            pos[0] = root;
            for (i = 0; i < depth; ++i) {
                p[i] = file->read(pos[i]);
                for (j = 0; j < p[i]->sz && Compare()(p[i]->keyvalue[j], key); ++j);
                if (i < depth - 1) {
                    if (j < p[i]->sz && !Compare()(key, p[i]->keyvalue[j]))++j;
                    pos[i + 1] = p[i]->pointer[j];
                }
                pp[i] = j;
            }
            if (j == p[depth - 1]->sz || Compare()(key, p[depth - 1]->keyvalue[j])) {
                delete[]p;
                delete[]pp;
                delete[]pos;
                return 0;
            }
            node *&x = p[depth - 1];
            if (!j) {
                for (i = depth - 2; i >= 0; --i)
                    if (pp[i] && equal(x->keyvalue[0], p[i]->keyvalue[pp[i] - 1])) {
                        p[i]->keyvalue[pp[i] - 1] = x->keyvalue[1];
                        file->save(pos[i]);
                        break;
                    }
            }
            //file->release(x->pointer[j]);
            for (i = j + 1; i < x->sz; ++i)x->keyvalue[i - 1] = x->keyvalue[i], x->val[i - 1] = x->val[i];
            --x->sz;
            if (depth == 1) {
                if (!x->sz) {
                    depth = 0;
                    file->release(root);
                    root = -1;
                } else {
                    file->save(root);
                }
                delete[]p;
                delete[]pp;
                delete[]pos;
                return 1;
            }
            if (x->sz >= degree >> 1) {
                file->save(pos[depth - 1]);
                delete[]p;
                delete[]pp;
                delete[]pos;
                return 1;
            }
            if (pp[depth - 2] == p[depth - 2]->sz) {
                p[depth] = p[depth - 1];
                pp[depth] = pp[depth - 2];
                pos[depth] = pos[depth - 1];
                pp[depth - 2] = p[depth - 2]->sz - 1;
                pos[depth - 1] = p[depth - 2]->pointer[pp[depth - 2]];
                p[depth - 1] = file->read(pos[depth - 1]);
            } else {
                pp[depth] = pp[depth - 2] + 1;
                pos[depth] = p[depth - 2]->pointer[pp[depth]];
                p[depth] = file->read(pos[depth]);
            }
            node *&y = p[depth];
            if (x->sz + y->sz < degree) {
                for (i = 0; i < y->sz; ++i)x->val[x->sz + i] = y->val[i], x->keyvalue[x->sz + i] = y->keyvalue[i];
                x->nxt = y->nxt;
                x->sz += y->sz;
                file->save(pos[depth - 1]);
                file->release(pos[depth]);
                erasenonleaf(depth - 2, p, pp, pos);
            } else {
                int newsz = x->sz + y->sz;
                T tmpv[newsz + 1];
                Key tmpk[newsz], old = y->keyvalue[0];
                for (i = 0; i < x->sz; ++i)tmpv[i] = x->val[i], tmpk[i] = x->keyvalue[i];
                for (i = x->sz; i < newsz; ++i)tmpv[i] = y->val[i - x->sz], tmpk[i] = y->keyvalue[i - x->sz];
                x->sz = newsz + 1 >> 1;
                y->sz = newsz - x->sz;
                for (i = 0; i < x->sz; ++i)x->val[i] = tmpv[i], x->keyvalue[i] = tmpk[i];
                for (i = 0; i < y->sz; ++i)y->val[i] = tmpv[i + x->sz], y->keyvalue[i] = tmpk[i + x->sz];
                file->save(pos[depth - 1]);
                file->save(pos[depth]);
                for (i = depth - 2; i >= 0; --i)
                    if (equal(old, p[i]->keyvalue[pp[i]])) {
                        p[i]->keyvalue[pp[i]] = y->keyvalue[0];
                        file->save(pos[i]);
                        break;
                    }
            }
            delete[]p;
            delete[]pp;
            delete[]pos;
            return 1;
        }
    };
}
#endif