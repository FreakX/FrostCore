#include "CharacterDatabase.h"

CharacterDatabase& CharacterDatabase::Instance()
{
    static CharacterDatabase pDatabase;
    return pDatabase;
};