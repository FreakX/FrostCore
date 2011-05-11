#include "AuthServer.h"
#include "boost/asio.hpp"
#include "AuthSession.h"

using boost::asio::ip::tcp;
_AuthServer::_AuthServer()
{
}
void _AuthServer::NewSession(tcp::socket* sock)
{
    AuthSession* new_session = new AuthSession(sock, m_session_count);
    m_session_count++;
    m_socketmap.insert(std::pair<tcp::socket*, AuthSession*>(sock, new_session));

    new_session->StartNetworking();
}
