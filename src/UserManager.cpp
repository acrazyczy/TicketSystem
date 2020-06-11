//
// Created by crazy_cloud on 2020/6/11.
//
#include "UserManager.hpp"
#include "OrderManager.hpp"

namespace sjtu
{
        void UserManager::add_order(OrderManager *order_manager, orderType *order) {
            auto ret = order_manager->OrderFile->newspace();          //std::pair<std::pair<*orderType , locType> , locType>
            ret.first -> first = *order;
            order->offset = ret.second;
            std::pair<locType, bool> tmp = UserBpTree->find(hasher(order->username));
            userType *cur = UserFile->read(tmp.first);
            ret.first -> second = cur->head;
            cur->head = ret.second;
            ++cur->order_count;
            order_manager->OrderFile->save(order->offset);
            UserFile->save(cur->offset);
        }
        bool UserManager::refund_order(OrderManager *order_manager, int n, orderType *order) {
            std::pair<locType, bool> tmp = UserBpTree->find(hasher(order->username));
            userType *cur = UserFile->read(tmp.first);
            if (cur->order_count < n) return false;
            else {
                locType tmpticket = cur->head;
                while(-- n) {
                    auto ret = order_manager->OrderFile->read(tmpticket);   //std::pair<*orderType , locType>
                    tmpticket = ret -> second;
                }
                auto now = order_manager->OrderFile->read(tmpticket);
                now -> first.status = refunded;
                order_manager->OrderFile->save(now -> first.offset);
            }
        }
        void UserManager::query_order(OrderManager *order_manager, int argc, std::string *argv) {
            std::string uname = argv[1];
            std::pair<locType, bool> tmp = UserBpTree->find(hasher(uname));
            if (tmp.second == false) std::cout << -1 << std::endl;
            else {
                userType *cur = UserFile->read(tmp.first);
                if (cur->is_online != online_flag) std::cout << -1 << std::endl;
                else {
                    std::cout << cur->order_count << std::endl;
                    locType tmpticket = cur->head;
                    while (tmpticket != -1) {
                        auto ret = order_manager->OrderFile->read(tmpticket);
                        std::cout << '[' << ret -> first.status << "] " << ret->first.trainID << ' ' << ret->first.station[0] << ' ' << ret->first.date[0] << " -> " << ret->first.station[1] << ' ' << ret->first.date[1] << ' ' << ret->first.price << ' ' << ret->first.num << std::endl;
                        tmpticket = ret -> second;
                    }
                }
            }
        }
}