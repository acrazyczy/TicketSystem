//
// Created by hanchong on 2020/5/23.
//

#ifndef TICKET_ORDERMANAGER_HPP
#define TICKET_ORDERMANAGER_HPP

namespace sjtu {

#include "FileManager.hpp"
#include "BplusTree.hpp"
#include "StringHaser.hpp"
#include "timeType.hpp"
#include "UserManager.hpp"
#include "TrainManager.hpp"
#include "TypesAndHeaders.hpp"
#include <iostream>
#include <cstring>
#include <string>

    enum orderstatus {
        success, pending, refunded
    };

    class orderType {
    public:
        char username[21];
        char trainID[21];
        timeType date[2];
        char station[2][41];
        int num;
        int price;
        orderstatus status;
        locType offset;
    };

    class OrderManager {
    private:
        StringHasher hasher;
        int pending_id = 0;
        BpTree <std::pair<StringHasher::hashType, int>, locType> *PendingBpTree;

    public:
        FileManager<std::pair<orderType, locType>> *OrderFile;

        void init(bool obj = false) {
            if (obj == false) {
                OrderFile = new FileManager<std::pair<orderType, locType>>("OrderFile.dat");
                PeningBpTree = new BpTree("PendingBptree.dat");
                if (OrderFile->is_newfile) {
                    auto ret = OrderFile.newspace();
                    orderType *order = ret.first;
                    order->offset = ret.second;
                    order->num = 0;
                    OrderFile->save(order->offset);
                } else {
                    orderType *tmp = OrderFile->read(0);
                    pending_id = tmp->num;
                }
            } else {
                OrderFile->init("OrderFile.dat", true);
                PendingBpTree->init("PendingBptree.dat", true);
                auto ret = OrderFile.newspace();
                orderType *order = ret.first;
                order->offset = ret.second;
                order->num = 0;
                OrderFile->save(order->offset);
            }
        }

        OrderManager() {
            init();
        }

        void refund_ticket(UserManager *user_manager, TrainManager *train_manager, int argc, std::string *argv) {
            std::string uname;
            int number = 1;
            for (int i = 0; i < argc; i += 2) {
                if (argv[i] == "-u") uname = argv[i + 1];
                else if (argv[i] == "-n") number = stoi(argv[i + 1]);
            }
            if (user_manager->is_login(uname) == false) std::cout << -1 << std::endl;
            else {
                orderType *tmp = new orderType;
                strcpy(tmp->username, uname.c_str());
                bool if_ok = user_manager->refund_order(this, number, tmp);
                if (if_ok) {
                    train_manager->refund_ticket(tmp);
                    vector <std::pair<StringHasher::hashType, int>, locType> pendinglist = PendingBpTree->range(
                            std::pair<hasher(tmp->trainID), 0>, std::pair<hasher(tmp->trainID), 0x7fffffff>);
                    for (int i = 0; i < pendinglist.size(); ++i) {
                        orderType *order = OrderFile->read(pendinglist[i].second);
                        if (order->status != refunded && train_manager->buy_ticket(order)) {
                            order->status = success;
                            OrderFile->save(order->offset);
                            PendingBpTree->earse(pendinglist[i]);
                            break;
                        }
                    }
                    std::cout << 0 << std::endl;
                } else std::cout << -1 << std::endl;
                delete tmp;
            }
        }

        void buy_ticket(UserManager *user_manager, TrainManager *train_manager, int argc, std::string *argv) {
            std::string uname, utrain, ufrom, uto;
            int unum;
            bool ifpending;
            for (int i = 0; i < argc; i += 2) {
                if (argv[i] == "-u") uname = argv[i + 1];
                else if (argv[i] == "-i") utrain = argv[i + 1];
                else if (argv[i] == "-d") timeType uday(argv[i + 1]);
                else if (argv[i] == "-n") unum = stoi(argv[i + 1]);
                else if (argv[i] == "-f") ufrom = argv[i + 1];
                else if (argv[i] == "-t") uto = argv[i + 1];
                else if (argv[i] == "-q") {
                    if (argv[i + 1] == "true") ifpending = true;
                    else ifpending = false;
                }
            }
            if (user_manager->is_login(uname) == false) std::cout << -1 << std::endl;
            else {
                orderType *tmp = new orderType;
                strcpy(tmp->username, uname.c_str());
                strcpy(tmp->trainID, utrain.c_str());
                strcpy(tmp->station[0], ufrom.c_str());
                strcpy(tmp->station[1], uto.c_str());
                tmp->date[0] = uday;
                if (train_manager->buy_ticket(tmp) == false) std::cout << -1 << std::endl;
                else {
                    if (tmp->status == success) {
                        user_manager->add_order(this, tmp);
                        std::cout << tmp->price << std::endl;
                    }
                    else {
                        if (ifpending == false) std::cout << -1 << std::endl;
                        else {
                            tmp->status = pending;
                            user_manager->add_order(this, tmp);
                            PendingBpTree->insert(std::pair<hasher(order->trainID), pending_id>, order->offset);
                            std::cout << "queue" << std::endl;
                        }
                    }
                }
                delete tmp;
            }
        }

        ~OrderManager() {
            orderType *tmp = OrderFile->read(0);
            tmp->num = pending_id;
            OrderFile->save(0);
            delete PendingBpTree;
            delete OrderFile;
        }
    };

};

#endif //TICKET_ORDERMANAGER_HPP



