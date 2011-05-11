#include "AuthSession.h"

#include "boost/bind.hpp"

#include "ByteBuffer.h"
#include "AuthDefines.h"
#include "LogonDatabase.h"
#include "Fields.h"
#include "openssl/md5.h"
#include "openssl/sha.h"
#include "Sha1.h"

#define s_BYTE_SIZE 8

void AuthSession::StartNetworking()
{
    m_buff.resize(1024);
    m_socket->async_read_some(boost::asio::buffer(m_buff, 1024),
                              boost::bind(&AuthSession::HandleRead,
                                          this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    /*boost::asio::async_read(*m_socket,
                            boost::asio::buffer(m_buff),
                            boost::asio::transfer_at_least(8),
                            boost::bind(&AuthSession::HandleRead,
                                        this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));*/
}

void AuthSession::HandleRead(boost::system::error_code ec, size_t transferred)
{
    if(ec)
    {
        HandleError(ec);
        m_socket->close();
        return;
    }
    m_buff.resize(transferred);

    ByteBuffer recv_packet;
    recv_packet._storage = m_buff;

    recv_packet.hexlike();

    HandlePacket(&recv_packet);
    StartNetworking();
}
void AuthSession::HandleWrite(boost::system::error_code ec, size_t transferred)
{
    if(ec)
    {
        HandleError(ec);
        m_socket->close();
        return;
    }
}
void AuthSession::HandleError(boost::system::error_code ec)
{
    /*
    Need to Delete this Session From AuthServer socketmap,
    else -> memoryleak
    */
}
void AuthSession::HandlePacket(ByteBuffer* packet)
{
    uint8 opcode;
    *packet >> opcode;
    switch(opcode)
    {
        case AUTH_LOGON_CHALLENGE:
            HandleAuthLogonChallenge(packet);
            break;

    }
}
void AuthSession::SendPacket(ByteBuffer packet)
{
    Send(packet._storage);
}
void AuthSession::Send(std::vector<uint8> buffer)
{
    m_socket->async_write_some( boost::asio::buffer(buffer, buffer.size()),
                                boost::bind(&AuthSession::HandleWrite,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
}

void AuthSession::HandleAuthLogonChallenge(ByteBuffer* packet)
{
    printf("Entering AuthLogonChallenge\n");
    uint8   cmd;
    uint8   error;
    uint16  size;
    uint8   gamename[4];
    uint8   version1;
    uint8   version2;
    uint8   version3;
    uint16  build;
    uint8   platform[4];
    uint8   os[4];
    uint8   country[4];
    uint32  timezone_bias;
    uint32  ip;
    uint8   name_len;
    std::string name;

    *packet >> cmd;
    *packet >> error;
    *packet >> size;
    *packet >> gamename[0];
    *packet >> gamename[1];
    *packet >> gamename[2];
    *packet >> gamename[3];
    *packet >> version1;
    *packet >> version2;
    *packet >> version3;
    *packet >> build;
    *packet >> platform[0];
    *packet >> platform[1];
    *packet >> platform[2];
    *packet >> platform[3];
    *packet >> os[0];
    *packet >> os[1];
    *packet >> os[2];
    *packet >> os[3];
    *packet >> country[0];
    *packet >> country[1];
    *packet >> country[2];
    *packet >> country[3];
    *packet >> timezone_bias;
    *packet >> ip;
    *packet >> name_len;
    *packet >> name;

    ByteBuffer pkt;

    _login = name;
    _build = build;
    _expversion = 3; // 3 for Cataclysm / TODO: calc form build

    pkt << (uint8) AUTH_LOGON_CHALLENGE;
    pkt << (uint8) 0x00;

    
    if (IPBanned(std::string((char*)ip)))
    {
        pkt << (uint8)WOW_FAIL_BANNED;
    }
    else
    {
        // Get the account details from the account table
        QueryResult* res = sLogonDatabase.Query("SELECT Password, Id, locked, LastIP, GMLevel, v, s FROM Accounts WHERE Username = %s", _login.c_str());

        if (res)
        {
            Fields* fields = res->Fetch();

            // Get the password from the account table, upper it, and make the SRP6 calculation
            std::string rI = fields[0].GetString();

            // Don't calculate (v, s) if there are already some in the database
            std::string databaseV = fields[5].GetString();
            std::string databaseS = fields[6].GetString();

            printf("database authentication values: v='%s' s='%s'", databaseV.c_str(), databaseS.c_str());

            // multiply with 2, bytes are stored as hexstring
            if (databaseV.size() == s_BYTE_SIZE * 2 || databaseS.size() == s_BYTE_SIZE * 2)
                _SetVSFields(rI);
            else
            {
                s.SetHexStr(databaseS.c_str());
                v.SetHexStr(databaseV.c_str());
            }

            b.SetRand(19 * 8);
            BigNumber gmod = g.ModExp(b, N);
            B = ((v * 3) + gmod) % N;

            //ASSERT(gmod.GetNumBytes() <= 32);

            BigNumber unk3;
            unk3.SetRand(16 * 8);

            // Fill the response packet with the result
            pkt << uint8(WOW_SUCCESS);

            // B may be calculated < 32B so we force minimal length to 32B
            pkt.append(B.AsByteArray(32), 32); // 32 bytes
            pkt << uint8(1);
            pkt.append(g.AsByteArray(), 1);
            pkt << uint8(32);
            pkt.append(N.AsByteArray(32), 32);
            pkt.append(s.AsByteArray(), s.GetNumBytes()); // 32 bytes
            pkt.append(unk3.AsByteArray(16), 16);
            uint8 securityFlags = 0;
            pkt << uint8(securityFlags); // security flags (0x0...0x04)

            if (securityFlags & 0x01) // PIN input
            {
            pkt << uint32(0);
            pkt << uint64(0) << uint64(0); // 16 bytes hash?
            }

            if (securityFlags & 0x02) // Matrix input
            {
            pkt << uint8(0);
            pkt << uint8(0);
            pkt << uint8(0);
            pkt << uint8(0);
            pkt << uint64(0);
            }

            if (securityFlags & 0x04) // Security token input
            pkt << uint8(1);

            uint8 secLevel = fields[4].GetUInt8();
            _gmlevel = secLevel;

            _localizationName.resize(4);
            for (int i = 0; i < 4; ++i)
            _localizationName[i] = country[4-i-1];
        }
        else                                                //no account
            pkt<< (uint8) WOW_FAIL_UNKNOWN_ACCOUNT;
    }

    SendPacket(pkt);
}
/// Make the SRP6 calculation from hash in dB
void AuthSession::_SetVSFields(const std::string& rI)
{
    s.SetRand(s_BYTE_SIZE * 8);

    BigNumber I;
    I.SetHexStr(rI.c_str());

    // In case of leading zeros in the rI hash, restore them
    uint8 mDigest[SHA_DIGEST_LENGTH];
    memset(mDigest, 0, SHA_DIGEST_LENGTH);
    if (I.GetNumBytes() <= SHA_DIGEST_LENGTH)
        memcpy(mDigest, I.AsByteArray(), I.GetNumBytes());

    std::reverse(mDigest, mDigest + SHA_DIGEST_LENGTH);

    Sha1Hash sha;
    sha.UpdateData(s.AsByteArray(), s.GetNumBytes());
    sha.UpdateData(mDigest, SHA_DIGEST_LENGTH);
    sha.Finalize();
    BigNumber x;
    x.SetBinary(sha.GetDigest(), sha.GetLength());
    v = g.ModExp(x, N);
    // No SQL injection (username escaped)
    const char *v_hex, *s_hex;
    v_hex = v.AsHexStr();
    s_hex = s.AsHexStr();
    sLogonDatabase.Execute("UPDATE Accounts SET v = '%s', s = '%s' WHERE Username = '%s'", v_hex, s_hex, _login.c_str());
    OPENSSL_free((void*)v_hex);
    OPENSSL_free((void*)s_hex);
}
bool AuthSession::IPBanned(std::string ip)
{
    return false; // TODO Implement this Function
}