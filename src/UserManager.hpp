//
// Created by hanchong on 2020/5/23.
//

#ifndef TICKET_USERMANAGER_HPP
#define TICKET_USERMANAGER_HPP

namespace sjtu {

#include "FileManager.hpp"
#include "OrderManager.hpp"
#include "BplusTree.hpp"
#include "StringHaser.hpp"
#include "TypesAndHeaders.hpp"
#include <iostream>
#include <cstring>
#include <string>

    enum trainstatus {
        success, pending, refunded
    };

    class userType {
    public:
        char username[21];
        char password[31];
        char name[21];
        char mailAddr[31];
        int privilege;
        bool is_online = false;
        locType head = -1;
        locType offset;
        int order_count = 0;
    };

    class UserManager {
    private:
        StringHasher hasher;
        BpTree <StringHasher::hashType, locType> *UserBpTree;

    public:
        FileManager<userType> *UserFile;

        void init(bool obj = false) {
            if (obj == false) {
                UserBpTree = new BpTree<StringHasher::hashType, locType>(std::string("UserBpTree.dat"));
                UserFile = new FileManager<userType>(std::string("UserFile.dat"));
            } else {
                UserBpTree->init(std::string("UserBpTree.dat"), true);
                UserFile->init(std::string("UserFile.dat"), true);
            }
        }

        UserManager() {
            init();
        }

        void add_user(int argc, std::string *argv) {
            if (UserBpTree->empty()) {
                auto ret = UserFile->newspace();
                userType *user = ret.first;
                user->offset = ret.second;
                user->privilege = 10;
                for (int i = 0; i < argc; i += 2) {
                    if (argv[i] == "-u") strcpy(user->username, argv[i + 1].c_str());
                    else if (argv[i] == "-p") strcpy(user->password, argv[i + 1].c_str());
                    else if (argv[i] == "-n") strcpy(user->name, argv[i + 1].c_str());
                    else if (argv[i] == "-m") strcpy(user->mailAddr, argv[i + 1].c_str());
                }
                user->is_online = false;
                UserFile->save(user->offset);
                UserBpTree->insert(Bptree::value_type(hasher(user->username), user->offset));
                std::cout << 0 << std::endl;
            } else {
                std::string usname, uspass, uname, umail, current;
                int upri;
                for (int i = 0; i < argc; i += 2) {
                    if (argv[i] == "-u") usname = argv[i + 1];
                    else if (argv[i] == "-p") uspass = argv[i + 1];
                    else if (argv[i] == "-n") uname = argv[i + 1];
                    else if (argv[i] == "-m") umail = argv[i + 1];
                    else if (argv[i] == "-c") current = argv[i + 1];
                    else if (argv[i] == "-g") upri = stoi(argv[i + 1]);
                }
                std::pair<locType, bool> tmp = UserBpTree->find(hasher(current));
                userType *curuser = UserFile->read(tmp.first);
                if (curuser->privilege > upri && tmp.second == false) {
                    auto ret = UserFile.newspace();
                    userType *user = ret.first;
                    user->offset = ret.second;
                    strcpy(user->username, usname.c_str());
                    strcpy(user->password, uspass.c_str());
                    strcpy(user->name, uname.c_str());
                    strcpy(user->mailAddr, umail.c_str());
                    user->privilege = upri;
                    user->is_online = false;
                    UserFile->save(user->offset);
                    UserBpTree->insert(Bptree::value_type(hasher(user->username), user->offset));
                    std::cout << 0 << std::endl;
                } else std::cout << -1 << std::endl;
            }
        }

        void login(int argc, std::string *argv) {
            std::string usname, uspass;
            for (int i = 0; i < argc; i += 2) {
                if (argv[i] == "-u") usname = argv[i + 1];
                else if (argv[i] == "-p") uspass = argv[i + 1];
            }
            std::pair<locType, bool> tmp = UserBpTree->find(hasher(usname));
            if (tmp.second == true) {
                userType *user = UserFile->read(tmp.first);
                if (uspass == user->password && user->is_online == false) {
                    user->is_online = true;
                    UserFile->save(user->offset);
                    std::cout << 0 << std::endl;
                } else std::cout << -1 << std::endl;
            } else std::cout << -1 << std::endl;
        }

        void logout(int argc, std::string *argv) {
            std::pair<locType, bool> tmp = UserBpTree->find(hasher(argv[1]));
            if (tmp.second == true) {
                userType *user = UserFile->read(tmp.first);
                if (user->is_online == true) {
                    user->is_online = false;
                    UserFile->save(user->offset);
                    std::cout << 0 << std::endl;
                } else std::cout << -1 << std::endl;
            } else std::cout << -1 << std::endl;

        }

        void query_profile(int argc, std::string *argv) {
            std::string curname, usname;
            for (int i = 0; i < argc; i += 2) {
                if (argv[i] == "-c") curname = argv[i + 1];
                else if (argv[i] == "-u") usname = argv[i + 1];
            }
            std::pair<locType, bool> tmp1 = UserBpTree->find(hasher(curname));
            std::pair<locType, bool> tmp2 = UserBpTree->find(hasher(usname));
            if (tmp1.second == true && tmp2.second == true) {
                userType *cur = UserFile->read(tmp1.first);
                int cur_privilege = cur->privilege;
                bool cur_is_online = cur->is_online;//cur may be disabled after next UserFile -> read()
                userType *user = UserFile->read(tmp2.first);
                if ((cur_privilege > user->privilege || curname == usname) && cur_is_online == true) {
                    std::cout << user->username << " " << user->name << " " << user->mailAddr << " " << user->privilege
                              << std::endl;
                } else std::cout << -1 << std::endl;
            } else std::cout << -1 << std::endl;
        }

        void modify_profile(int argc, std::string *argv) {
            std::string usname, uspass = "", uname = "", umail = "", curname;
            int upri = -1;
            for (int i = 0; i < argc; i += 2) {
                if (argv[i] == "-u") usname = argv[i + 1];
                else if (argv[i] == "-p") uspass = argv[i + 1];
                else if (argv[i] == "-n") uname = argv[i + 1];
                else if (argv[i] == "-m") umail = argv[i + 1];
                else if (argv[i] == "-c") curname = argv[i + 1];
                else if (argv[i] == "-g") upri = stoi(argv[i + 1]);
            }
            std::pair<locType, bool> tmp1 = UserBpTree->find(hasher(curname));
            std::pair<locType, bool> tmp2 = UserBpTree->find(hasher(usname));
            if (tmp1.second == true && tmp2.second == true) {
                userType *cur = UserFile->read(tmp1.first);
                int cur_privilege = cur->privilege;
                bool cur_is_online = cur->is_online;//cur may be disabled after next UserFile -> read()
                userType *user = UserFile->read(tmp2.first);
                if ((cur_privilege > user->privilege || curname == usname) && cur_is_online == true &&
                    cur_privilege > upri) {
                    if (uspass != "") strcpy(user->password, uspass.c_str());
                    if (uname != "") strcpy(user->name, uname.c_str());;
                    if (umail != "") strcpy(user->mailAddr, umail.c_str());;
                    if (upri != -1) user->privilege = upri;
                    UserFile->save(user->offset);
                    std::cout << user->username << " " << user->name << " " << user->mailAddr << " " << user->privilege
                              << std::endl;
                } else std::cout << -1 << std::endl;
            } else std::cout << -1 << std::endl;
        }
        bool is_login(std::string username) {
            std::pair<locType, bool> tmp = UserBpTree->find(hasher(username));
            if (tmp.second == false) return false;
            else {
                userType *cur = UserFile->read(tmp.first);
                return cur->is_online;
            }
        }
        void add_order(OrderManager *order_manager, orderType *order) {
            auto ret = order_manager->OrderFile->newspace();          //std::pair<std::pair<*orderType , locType> , locType>
            ret.first.first = order;
            order->offset = ret.second;
            std::pair<locType, bool> tmp = UserBpTree->find(hasher(order->username));
            userType *cur = UserFile->read(tmp.first);
            ret.first.second = cur->head;
            cur->head = ret.second;
            ++cur->order_count;
            order_manager->OrderFile->save(order->offset);
            UserFile->save(cur->offset);
        }
        bool refund_order(OrderManager *order_manager, int n, orderType *order) {
            std::pair<locType, bool> tmp = UserBpTree->find(hasher(order->username));
            userType *cur = UserFile->read(tmp.first);
            if (cur->order_count < n) return false;
            else {
                locType tmpticket = cur->head;
                --n;
                while(n--) {
                    auto ret = order_manager->OrderFile->read(tmpticket);   //std::pair<*orderType , locType>
                    tmpticket = ret.second;
                }
                auto now = order_manager->OrderFile->read(tmpticket);
                orderType *nowticket = now.first;
                nowticket->status = refunded;
                order_manager->OrderFile->save(nowticket->offset);
            }
        }
        void query_order(OrderManager *order_manager, int argc, std::string *argv) {
            std::string uname = argv[1];
            std::pair<locType, bool> tmp = UserBpTree->find(hasher(uname));
            if (tmp.second == false) std::cout << -1 << std::endl;
            else {
                userType *cur = UserFile->read(tmp.first);
                if (!cur->is_online) std::cout << -1 << std::endl;
                else {
                    std::cout << cur->order_count << std::endl;
                    locType tmpticket = cur->head;
                    while (tmpticket != -1) {
                        auto ret = order_manager->OrderFile->read(tmpticket);
                        orderType *nowticket = now.first;
                        std::cout << '[' << nowticket->status << '] ' << nowticket->trainID << ' ' << nowticket->station[0] << ' ' << nowticket->date[0] << " -> " << nowticket->station[1] << ' ' << nowticket->date[1] << ' ' << nowticket->price << ' ' << nowticket->num << std::endl;
                        tmpticket = ret.second;
                    }
                }
            }
        }
        ~UserManager() {
            delete UserFile;
            delete UserBpTree;
        }
    };

};
#endif //TICKET_USERMANAGER_HPP


