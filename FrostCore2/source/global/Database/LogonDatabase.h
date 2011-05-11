#ifndef LOGONDATABASE_H_
#define LOGONDATABASE_H_

#include "DatabaseSystem.h"

class LogonDatabase : public Database
{
public:
    static LogonDatabase& Instance();
};

#define sLogonDatabase LogonDatabase::Instance()
#endif