//
// Created by crazy_cloud on 2020/6/11.
//
#include "UserManager.hpp"
#include "OrderManager.hpp"

namespace sjtu
{
        void UserManager::add_order(OrderManager *order_manager, orderType *order) {
            auto ret = order_manager->OrderFile->newspace();          //std::pair<std::pair<orderType , locType>* , locType>
            ret.first -> first = *order;
            ret.first->first.offset = order -> offset = ret.second;
            std::pair<locType, bool> tmp = UserBpTree->find(hasher(order->username));
            userType *cur = UserFile->read(tmp.first);
            ret.first -> second = cur->head;
            cur->head = ret.second;
            ++cur->order_count;
            order_manager->OrderFile->save(ret.first->first.offset);
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
                if (now -> first.status == refunded) return false;
                now -> first.status = refunded;
                order -> date[0] = now -> first.date[0] , order -> date[1] = now -> first.date[1];
                order -> num = now -> first.num , order -> price = now -> first.price , order -> offset = now -> first.offset;
                strcpy(order -> trainID , now -> first.trainID);
                strcpy(order -> station[0] , now -> first.station[0]) , strcpy(order -> station[1] , now -> first.station[1]);
                order_manager->OrderFile->save(now -> first.offset);
                return true;
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
                        std::cout << '[' << (ret -> first.status ? (ret -> first.status == pending ? "pending" : "refunded") : "success" ) << "] " << ret->first.trainID << ' ' << ret->first.station[0] << ' ' << ret->first.date[0] << " -> " << ret->first.station[1] << ' ' << ret->first.date[1] << ' ' << ret->first.price << ' ' << ret->first.num << std::endl;
                        tmpticket = ret -> second;
                    }
                }
            }
        }
}