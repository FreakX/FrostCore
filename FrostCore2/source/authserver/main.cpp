#include <ctime>
#include <iostream>
#include <string>

#include "BoostFix.h"
#include "boost/asio.hpp"
#include "boost/thread.hpp"
#include "boost/progress.hpp"
#include "AuthServer.h"
#include "LogonDatabase.h"
#include "config.h"
#include "Types.h"


using boost::asio::ip::tcp;

void ServiceLoop(boost::asio::io_service* io_service)
{
    for(;;)
    {
        io_service->run();
    }
}

int main()
{
    try
    {
        std::cout << "===============================================================================" << std::endl;
        std::cout << "======= FrostCore === Authentication Server === (c) dani11795@googlemail ======" << std::endl;
        std::cout << "===============================================================================" << std::endl;
        sLogonDatabase.SetDatabaseName("Logon");
        sLogonDatabase.InitDB(FC_MYSQL_LOGON_HOST, 3306, FC_MYSQL_LOGON_USER, FC_MYSQL_LOGON_PASS, FC_MYSQL_LOGON_DATA, 5, 2048);
        
        boost::asio::io_service io_service;

        boost::thread workerThread(ServiceLoop, &io_service);
        
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 3724));
        for (;;)
        {
            tcp::socket* socket = new tcp::socket(io_service);
            acceptor.accept(*socket);
            
            sAuthServer.NewSession(socket);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}


// Use of Progress Bar :
/*
boost::progress_display show_progress( 1000 ,std::cout , std::string(""), std::string(""), std::string(""));
for(uint32 i = 0; i != 1000; i++)
{
    ++show_progress;
    boost::asio::deadline_timer t(io_service, boost::posix_time::millisec(10));
    t.wait();
}
*/