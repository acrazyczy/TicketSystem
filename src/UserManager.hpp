//
// Created by hanchong on 2020/5/23.
//

#ifndef TICKET_USERMANAGER_HPP
#define TICKET_USERMANAGER_HPP

#include "FileManager.hpp"
#include "BpTree.hpp"
#include "StringHaser.hpp"
#include <iostream>
#include <cstring>
#include <string>

class UserManager {
private:
    class userType {
    public:
        char username[21];
        char password[31];
        char name[21];
        char mailAddr[31];
        int privilege;
        bool is_online;
        locType head;
        locType offset;
    };
    StringHasher hasher;
    static FineManager<userType> *UserFile;
    static BpTree<StringHasher::hashType, locType> *UserBpTree;

public:
    static void init(bool &obj) {
        if (obj == false) {
            UserBpTree = new BpTree<StringHasher::hashType, locType>(std::string("UserFile.dat"));
            UserFile new FileManager(std::string("UserFile.dat"));
        }
        else {
            UserBpTree->init(std::string("UserFile.dat"), true);
            UserFile->init(std::string("UserFile.dat"), true);
        }
    }
    static void add_user(int argc, std::string *argv) {
        if (UserBpTree.empty()) {
            auto ret = UserFile.newspace();
            userType *user = ret.first;
            user -> offset = ret.second;
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
        }
        else {
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
                user -> offset = ret.second;
                strcpy(user->username, usname.c_str());
                strcpy(user->password, uspass.c_str());
                strcpy(user->name, uname.c_str());
                strcpy(user->mailAddr, umail.c_str());
                user->privilege = upri;
                user->is_online = false;
                UserFile->save(user->offset);
                UserBpTree->insert(Bptree::value_type(hasher(user->username), user->offset));
                std::cout << 0 << std::endl;
            }
            else std::cout << -1 << std::endl;
        }
    }
    static void login(int argc, std::string *argv) {
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
            }
            else std::cout << -1 << std::endl;
        }
        else std::cout << -1 << std::endl;
    }
    static void logout(int argc, std::string *argv) {
        std::pair<locType, bool> tmp = UserBpTree->find(hasher(argv[1]));
        if (tmp.second == true) {
            userType *user = UserFile->read(tmp.first);
            if (user->is_online == true) {
                user->is_online = false;
                UserFile->save(user->offset);
                std::cout << 0 << std::endl;
            }
            else std::cout << -1 << std::endl;
        }
        else std::cout << -1 << std::endl;

    }
    static void query_profile(int argc, std::string *argv) {
        std::string curname, usname;
        for (int i = 0; i < argc; i += 2) {
            if (argv[i] == "-c") curname = argv[i + 1];
            else if (argv[i] == "-u") usname = argv[i + 1];
        }
        std::pair<locType, bool> tmp1 = UserBpTree->find(hasher(curname));
        std::pair<locType, bool> tmp2 = UserBpTree->find(hasher(usname));
        if (tmp1.second == true && tmp2.second == true) {
            userType *cur = UserFile->read(tmp1.first);
            userType *user = UserFile->read(tmp2.first);
            if ((cur->privilege > user->privilege || curname == usname) && cur->is_online == true) {
                std::cout << user->username << " " << user->name << " " << user->mailAddr << " " << user->privilege << std::endl;
            }
            else std::cout << -1 <<std::endl;
        }
        else std::cout << -1 << std::endl;
    }
    static void modify_profile(int argc, std::string *argv) {
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
            userType *user = UserFile->read(tmp2.first);
            if ((cur->privilege > user->privilege || curname == usname) && cur->is_online == true && cur->privilege > upri) {
                if (uspass != "") strcpy(user->password, uspass.c_str());
                if (uname != "") strcpy(user->name, uname.c_str());;
                if (umail != "") strcpy(user->mailAddr, umail.c_str());;
                if (upri != -1) user->privilege = upri;
                UserFile->save(user->offset);
                std::cout << user->username << " " << user->name << " " << user->mailAddr << " " << user->privilege << std::endl;
            }
            else std::cout << -1 <<std::endl;
        }
        else std::cout << -1 << std::endl;
    }
    static unsigned long long &access_head(std::string obj1, bool obj2) {
        userType *user = UserFile.read(UserBpTree->find(hasher(user->username)).first);
        if (obj2 == true) UserFile.save(user->offset);
        return &user -> head;
    }
    ~UserManager() {
        delete UserFile;
        delete UserBpTree;
    }
};
static FineManager<UserManager::userType> *UserManager::UserFile;
static BpTree<StringHasher::hashType, locType> *UserManager::UserBpTree;

#endif //TICKET_USERMANAGER_HPP
