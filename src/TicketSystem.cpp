//
// Created by hanchong on 2020/6/7.
//

#include "UserManager.hpp"
#include "TrainManager.hpp"
#include "OrderManager.hpp"
#include <iostream>
#include <cstring>
#include <string>

int main()
{
    std::string argv[100];
    int argc = 0, l, r;
    sjtu::UserManager *user_manager;
    sjtu::TrainManager *train_manager;
    sjtu::OrderManager *order_manager;
    user_manager = new sjtu::UserManager;
    train_manager = new sjtu::TrainManager;
    order_manager = new sjtu::OrderManager;
    while(1) {
        std::string tmp, ord;
        getline(std::cin, tmp);
        argc = 0;
        for (l = 0; l < tmp.length(); l = r + 1) {
            for (r = l; r < tmp.length() && tmp[r] != ' '; ++r) {
                if (l == 0) ord = std::string(tmp, l, r - l + 1);
            }
            if (l != 0) argv[argc++] = std::string(tmp, l, r - l + 1);
        }
        if(ord == "add_user") user_manager->add_user(argc, argv);
        else if (ord == "login") user_manager->login(argc, argv);
        else if (ord == "logout") user_manager->logout(argc, argv);
        else if (ord == "query_profile") user_manager->query_profile(argc, argv);
        else if (ord == "modify_profile") user_manager->modify_profile(argc, argv);
        else if (ord == "add_train") train_manager->add_train(argc, argv);
        else if (ord == "release_train") train_manager->release_train(argc, argv);
        else if (ord == "query_train") train_manager->query_train(argc, argv);
        else if (ord == "delete_train") train_manager->delete_train(argc, argv);
        else if (ord == "query_ticket") train_manager->query_ticket(argc, argv);
        else if (ord == "query_transfer") train_manager->query_transfer(argc, argv);
        else if (ord == "buy_ticket") order_manager->buy_ticket(user_manager, train_manager, argc, argv);
        else if (ord == "query_order") user_manager->query_order(order_manager, argc, argv);
        else if (ord == "refund_ticket") order_manager->refund_ticket(user_manager, train_manager, argc, argv);
        else if (ord == "clean") {
            user_manager->init(true);
            train_manager->init(true);
            order_manager->init(true);
        }
        else if (ord == "exit") {
            delete user_manager;
            delete train_manager;
            delete order_manager;
        }
    }
    return 0;
}