#ifndef AUTHSERVER_H_
#define AUTHSERVER_H_

#include <map>

#include "boost/asio.hpp"

#include "AuthSession.h"

using boost::asio::ip::tcp;

class _AuthServer
{
public:
	_AuthServer();
	~_AuthServer(){};

	void NewSession(tcp::socket* sock);

	static _AuthServer& instance(){
		static _AuthServer server;
		return server;
	}

    std::map<tcp::socket*, AuthSession*> m_socketmap;
    uint64 m_session_count;
};

#endif

#define sAuthServer _AuthServer::instance()
