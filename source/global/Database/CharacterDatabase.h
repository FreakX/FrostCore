#ifndef CHARACTERDATABASE_H_
#define CHARACTERDATABASE_H_

#include "DatabaseSystem.h"

class CharacterDatabase : public Database
{
public:
    static CharacterDatabase& Instance();
};

#define sCharacterDatabase CharacterDatabase::Instance()

#endif