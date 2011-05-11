#ifndef AUTHSESSION_H_
#define AUTHSESSION_H_

#include <vector>
#include "boost/asio.hpp"
#include "Types.h"
#include "BigNumber.h"
#include "ByteBuffer.h"

using boost::asio::ip::tcp;

class AuthSession
{
public:
    AuthSession(tcp::socket* sock, uint64 sessionno)
    {
        m_socket = sock;
        m_id = sessionno;
        N.SetHexStr("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
        g.SetDword(7);
        _authed = false;
    };
    ~AuthSession()
    {
        m_socket->close();
    };

    void StartNetworking();

    // Common Networking Operations
    void HandleRead(boost::system::error_code ec, size_t transferred);
    void HandleWrite(boost::system::error_code ec, size_t transferred);
    void HandleError(boost::system::error_code ec);
    void HandlePacket(ByteBuffer* packet);
    void SendPacket(ByteBuffer packet);
    void Send(std::vector<uint8> buffer);

    // Packet Functions
    void HandleAuthLogonChallenge(ByteBuffer* packet);


    // Auth calc Functions
    void _SetVSFields(const std::string& rI);

    // Banned Functions
    bool IPBanned(std::string ip);

    tcp::socket* m_socket;
    std::vector<uint8> m_buff;
    uint64 m_id;



    // Used for Authentication
    BigNumber N, s, g, v;
    BigNumber b, B;
    BigNumber K;
    BigNumber _reconnectProof;

    bool _authed;
    uint16 _build;
    uint8 _expversion;
    std::string _login;
    uint8 _gmlevel;
    std::string _localizationName;


};
#endif