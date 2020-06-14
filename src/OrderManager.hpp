//
// Created by hanchong on 2020/5/23.
//

#ifndef TICKET_ORDERMANAGER_HPP
#define TICKET_ORDERMANAGER_HPP


#include "FileManager.hpp"
#include "BplusTree.hpp"
#include "StringHasher.hpp"
#include "UserManager.hpp"
#include "TrainManager.hpp"
#include "TypesAndHeaders.hpp"

namespace sjtu {

    enum orderstatus {
        success , pending, refunded
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
        BplusTree <std::pair<StringHasher::hashType, int>, locType> *PendingBpTree;

    public:
        FileManager<std::pair<orderType, locType>> *OrderFile;

        void init(bool obj = false) {
            if (obj == false) {
                OrderFile = new FileManager<std::pair<orderType, locType>>("OrderFile.dat");
                PendingBpTree = new BplusTree<std::pair<StringHasher::hashType, int>, locType>("PendingBptree.dat");
                if (OrderFile->is_newfile) {
                    auto ret = OrderFile -> newspace();
                    ret.first -> first.offset = ret.second;
                    ret.first -> first.num = 0;
                    OrderFile->save(ret.first -> first.offset);
                } else {
                    auto tmp = OrderFile->read(0);
                    pending_id = tmp->first.num;
                }
            } else {
                OrderFile->init("OrderFile.dat", true);
                PendingBpTree->init("PendingBptree.dat", true);
                    auto ret = OrderFile -> newspace();
                    ret.first -> first.offset = ret.second;
                    ret.first -> first.num = 0;
                    OrderFile->save(ret.first -> first.offset);
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
                if (user_manager->refund_order(this, number, tmp)) {
                    train_manager->refund_ticket(tmp);
                    auto pendinglist = PendingBpTree->range_query(
                            std::make_pair(hasher(tmp->trainID), 0), std::make_pair(hasher(tmp->trainID), 0x7fffffff));
                    for (int i = 0; i < pendinglist.size(); ++i) {
                        auto ret = OrderFile->read(pendinglist[i].second);
                        if (ret -> first.status != refunded && train_manager->buy_ticket(&(ret -> first))) {
                            if (ret -> first.status != pending)
                            {
                                OrderFile->save(ret -> first.offset);
                                PendingBpTree->erase(pendinglist[i].first);
                            }
                            break;
                        }
                    }
                    std::cout << 0 << std::endl;
                } else std::cout << -1 << std::endl;
                delete tmp;
            }
        }

        void buy_ticket(UserManager *user_manager, TrainManager *train_manager, int argc, std::string *argv) {
            std::string uname, utrain, ufrom, uto;timeType uday;
            int unum;
            bool ifpending;
            for (int i = 0; i < argc; i += 2) {
                if (argv[i] == "-u") uname = argv[i + 1];
                else if (argv[i] == "-i") utrain = argv[i + 1];
                else if (argv[i] == "-d") uday = timeType(argv[i + 1]);
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
                tmp -> num = unum;
                tmp->date[0] = uday;
                if (train_manager->buy_ticket(tmp) == false) std::cout << -1 << std::endl;
                else {
                    if (tmp->status == success) {
                        user_manager->add_order(this, tmp);
                        std::cout << tmp->price * tmp->num << std::endl;
                    }
                    else {
                        if (ifpending == false) std::cout << -1 << std::endl;
                        else {
                            tmp->status = pending;
                            user_manager->add_order(this, tmp);
                            PendingBpTree->insert(std::make_pair(std::make_pair(hasher(tmp->trainID), pending_id), tmp->offset));
                            std::cout << "queue" << std::endl;
                        }
                    }
                }
                delete tmp;
            }
        }

        ~OrderManager() {
            auto tmp = OrderFile->read(0);
            tmp -> first.num = pending_id;
            OrderFile-> save(0);
            delete PendingBpTree;
            delete OrderFile;
        }
    };

};

#endif //TICKET_ORDERMANAGER_HPP
