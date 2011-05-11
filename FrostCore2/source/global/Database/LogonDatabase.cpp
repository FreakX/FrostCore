#include "LogonDatabase.h"

LogonDatabase& LogonDatabase::Instance()
{
    static LogonDatabase pDatabase;
    return pDatabase;
};