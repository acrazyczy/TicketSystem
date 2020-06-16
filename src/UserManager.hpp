//
// Created by hanchong on 2020/5/23.
//

#ifndef TICKET_USERMANAGER_HPP
#define TICKET_USERMANAGER_HPP



#include "FileManager.hpp"
#include "BplusTree.hpp"
#include "StringHasher.hpp"
#include "TypesAndHeaders.hpp"

namespace sjtu {

    class orderType;
    class OrderManager;

    class userType {
    public:
        char username[21];
        char password[31];
        char name[21];
        char mailAddr[31];
        int privilege;
        unsigned int is_online;
        locType head = -1;
        locType offset;
        int order_count = 0;
    };

    class UserManager {
    private:
        StringHasher hasher;
        BplusTree <StringHasher::hashType, locType> *UserBpTree;

    public:
        FileManager<userType> *UserFile;

        unsigned int online_flag;

        void init(bool obj = false) {
            if (obj == false) {
                UserBpTree = new BplusTree<StringHasher::hashType, locType>(std::string("UserBpTree.dat"));
                UserFile = new FileManager<userType>(std::string("UserFile.dat"));
            } else {
                UserBpTree->init(std::string("UserBpTree.dat"), true);
                UserFile->init(std::string("UserFile.dat"), true);
            }
            if (UserFile -> is_newfile)
            {
            	std::ofstream outfile("OnlineFlag.dat" , std::ios_base::out | std::ios_base::binary);
            	online_flag = 0 , outfile.write(reinterpret_cast<char *>(&online_flag) , sizeof (unsigned int));
            	outfile.close();
            }
            else
            {
            	std::ifstream infile("OnlineFlag.dat" , std::ios_base::in);
            	infile.read(reinterpret_cast<char *>(&online_flag) , sizeof (unsigned int));
            	infile.close();
            }
            ++ online_flag;
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
                user->is_online = 0;
                UserFile->save(user->offset);
                UserBpTree->insert(BplusTree<StringHasher::hashType , locType>::value_type(hasher(user->username), user->offset));
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
                if (tmp.second)
		{
			userType *curuser = UserFile->read(tmp.first);
			if (curuser -> is_online == online_flag && curuser->privilege > upri && UserBpTree -> find(hasher(usname)).second == false)
			{
				auto ret = UserFile -> newspace();
				userType *user = ret . first;
				user -> offset = ret . second;
				strcpy(user -> username , usname . c_str());
				strcpy(user -> password , uspass . c_str());
				strcpy(user -> name , uname . c_str());
				strcpy(user -> mailAddr , umail . c_str());
				user -> privilege = upri;
				user -> is_online = 0;
				UserFile -> save(user -> offset);
				UserBpTree -> insert(BplusTree<StringHasher::hashType , locType>::value_type(
					hasher(user -> username) , user -> offset));
				std::cout << 0 << std::endl;
			}else std::cout << -1 << std::endl;
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
                if (uspass == user->password && user->is_online != online_flag) {
                    user->is_online = online_flag;
                    UserFile->save(user->offset);
                    std::cout << 0 << std::endl;
                } else std::cout << -1 << std::endl;
            } else std::cout << -1 << std::endl;
        }

        void logout(int argc, std::string *argv) {
            std::pair<locType, bool> tmp = UserBpTree->find(hasher(argv[1]));
            if (tmp.second == true) {
                userType *user = UserFile->read(tmp.first);
                if (user->is_online == online_flag) {
                    user->is_online = 0;
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
                bool cur_is_online = cur->is_online == online_flag;
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
                bool cur_is_online = cur->is_online == online_flag;
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
                return cur->is_online == online_flag;
            }
        }

        void add_order(OrderManager * , orderType *);

        bool refund_order(OrderManager *, int , orderType *);

        void query_order(OrderManager *, int , std::string *);

        ~UserManager() {
            delete UserFile;
            delete UserBpTree;
            	std::ofstream outfile("OnlineFlag.dat" , std::ios_base::out | std::ios_base::binary);
            	outfile.write(reinterpret_cast<char *>(&online_flag) , sizeof (unsigned int));
            	outfile.close();
        }
    };

};
#endif //TICKET_USERMANAGER_HPP