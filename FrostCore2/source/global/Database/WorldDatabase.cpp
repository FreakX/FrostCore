#include "WorldDatabase.h"

WorldDatabase& WorldDatabase::Instance()
{
    static WorldDatabase pDatabase;
    return pDatabase;
};