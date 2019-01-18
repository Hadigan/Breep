
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                               //
// Copyright 2017 Lucas Lazare.                                                                  //
// This file is part of Breep project which is released under the                                //
// European Union Public License v1.1. If a copy of the EUPL was                                 //
// not distributed with this software, you can obtain one at :                                   //
// https://joinup.ec.europa.eu/community/eupl/og_page/european-union-public-licence-eupl-v11     //
//                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * @file chat/main.cpp
 * @author Lucas Lazare
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <boost/uuid/uuid_io.hpp>
#include <map>
#include <breep/network/tcp.hpp>
#include <string>

std::map<std::string, breep::tcp::peer> nodes;
const char* getAddr(char* hostName, char* str, int len) {
    char   *ptr, **pptr;
    struct hostent *hptr;
    ptr = hostName;

    if((hptr = gethostbyname(ptr)) == NULL)
    {
        printf(" gethostbyname error for host:%s\n", ptr);
        return NULL;
    } else 
    {
        return inet_ntop(hptr->h_addrtype, hptr->h_addr, str, len);
    }
}

class timed_message {
public:
	timed_message(): m_starting_time{time(0)} {}

	/*
	 * Using an operator() overload allows you to pass objects as listeners.
	 */
	void operator()(breep::tcp::peer_manager& /* peer_manager */, const breep::tcp::peer& source, breep::cuint8_random_iterator data,
	                      size_t data_size, bool /* sent_only_to_me */) {

		// print the time and the name of the buddy that sent me something
		time_t now = time(0) - m_starting_time;
		std::cout << '[' << std::string(ctime(&now)).substr(14,5) << "] " << source.id_as_string().substr(0, 4) << ": ";

		// prints what he sent.
		//ABCDEname
		if (data_size >= 5)
		{
			if ((int)data[0] == 65 &&
				(int)data[1] == 66 &&
				(int)data[2] == 67 &&
				(int)data[3] == 68 &&
				(int)data[4] == 69)
			{

				data = data + 5;
				std::cout << "recieve name: ";
				data_size -= 5;
				char *nodeNameT = (char *)malloc(data_size * sizeof(char));
				size_t dataSizeT = data_size;
				for (; data_size > 0; --data_size)
				{
					nodeNameT[dataSizeT - data_size] =  static_cast<char>(*data++);
				}
				std::cout<< std::endl;
				nodes.insert(std::pair<std::string, breep::tcp::peer>(std::string(nodeNameT, dataSizeT), source));
                
				// nodes[std::string(nodeNameT, dataSizeT)] = source;
			}
			else
			{
				for (; data_size > 0; --data_size)
				{
					std::cout << static_cast<char>(*data++);
				}
			}
		}
		std::cout << std::endl;
		// we could reply directly here by using the peer_manager passed as parameter.
		//ie : peer_manager.send_to_all("reply"); or peer_manager.send_to(source, "reply");
	}

private:
	const time_t m_starting_time;
};

/*
 * This method will get called whenever a peer connects // disconnects
 * (connection listeners can be used as disconnection listeners and vice-versa)
 */
void connection_disconnection(breep::tcp::peer_manager &peer_manager, const breep::tcp::peer &peer)
{
    if (peer.is_connected())
    {
        // someone connected
        std::cout << peer.id_as_string().substr(0, 4) << " connected!" << std::endl;
        peer_manager.send_to(peer, std::string("ABCDE") + std::string(peer_manager.nodeName));
    }
    else
    {
        // someone disconnected
        //这里需要根据peer的id找到node中的索引  删除。
        std::cout << peer.id_as_string().substr(0, 4) << " disconnected" << std::endl;
        for (auto &x : nodes)
        {
            if (x.second.id_as_string().compare(peer.id_as_string()) == 0)
            {
                nodes.erase(x.first);
                break;
            }
        }
    }
}

void sendTo(breep::tcp::peer_manager& peer_manager, std::string nodeName, const std::string data)
{
    for (auto &x : nodes)
    {
        if (x.first.compare(nodeName) == 0)
        {
            peer_manager.send_to(x.second, data);
        }
    }
}

int main(int argc, char* argv[]) {

	if (argc != 3 && argc != 5) {
		std::cout << "Usage: " << argv[0] << " <hosting port> [<target ip> <target port>]" << std::endl;
		return 1;
	}
	char *str = (char*)malloc(32*sizeof(char));
	char *nodeName = (char*)malloc(32*sizeof(char));
	// taking the local hosting port as parameter.
	breep::tcp::peer_manager peer_manager(static_cast<unsigned short>(atoi(argv[1])));
	
	// disabling logging.
	peer_manager.set_log_level(breep::log_level::none);

	std::cout << "you are " << peer_manager.self().id_as_string() << "." << std::endl;

	// adding listeners. Of course, more listeners could be added.
	breep::listener_id da_listener_id = peer_manager.add_data_listener(timed_message());
	breep::listener_id co_listener_id = peer_manager.add_connection_listener(&connection_disconnection);
	breep::listener_id dc_listener_id = peer_manager.add_disconnection_listener(&connection_disconnection);


	if (argc == 3) {
		// only hosting
		peer_manager.run();
		nodeName = argv[2];
		peer_manager.nodeName = nodeName;
	} else {
		nodeName = argv[4];
		peer_manager.nodeName = nodeName;
		// connecting to a remote peer.                                           v− address in string format (v4 or v6)
		sleep(3);
		getAddr(argv[2], str, 32);
		std::cout<<str<<std::endl;
		boost::asio::ip::address address = boost::asio::ip::address::from_string(str);
		//                                                    target port -v
		if (!peer_manager.connect(address, static_cast<unsigned short>(atoi(argv[3])))) {
			std::cout << "Connection failed" << std::endl;
			return 1;
		}
	}


	std::string data;
	std::string node1 = std::string("node2");
	data = std::string(nodeName);
	while(true) {
		// std::getline(std::cin, ans);

		// if (ans == "/q") {
		// 	std::cout << "Leaving..." << std::endl;
		// 	peer_manager.disconnect();
		// 	break;
		// } else {
		// 	peer_manager.send_to_all(nodeName);
		// }
		sleep(10);
		if (std::string(peer_manager.nodeName).compare(node1) != 0)
		{
			// for (auto &x : nodes)
			// {
			// 	if (x.first.compare(node1) == 0) {
			// 		peer_manager.send_to(x.second, data);
			// 	}
			// }
            sendTo(peer_manager, node1, data);
		}
		// peer_manager.send_to_all(data);
	}

	// this is not obligatory, as the peer_manager is going out of scope anyway
	peer_manager.remove_data_listener(da_listener_id);
	peer_manager.remove_connection_listener(co_listener_id);
	peer_manager.remove_disconnection_listener(dc_listener_id);
	free(str);
	free(nodeName);
}
