#ifndef WORLDDATABASE_H_
#define WORLDDATABASE_H_

#include "DatabaseSystem.h"

class WorldDatabase : public Database
{
public:
    static WorldDatabase& Instance();

};

#define sWorldDatabase WorldDatabase::Instance()

#endif