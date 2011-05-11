#ifndef TYPES_H_
#define TYPES_H_
#ifdef COMPILER_MSVC
typedef unsigned __int64 uint64;
typedef __int64 int64;
#else
typedef unsigned long long uint64;
typedef long long int64;
#endif
typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef long int32;
typedef short int16;
typedef char int8;


#ifdef COMPILER_MSVC
#include <float.h>
#define I64FMT "%016I64X"
#define I64FMTD "%I64u"
#define SI64FMTD "%I64d"
#define snprintf _snprintf
#define sscanf sscanf_s
#define atoll __atoi64
#define vsnprintf _vsnprintf
#define strdup _strdup
#define finite(X) _finite(X)
#else
#define stricmp strcasecmp
#define strnicmp strncasecmp
#define I64FMT "%016llX"
#define I64FMTD "%llu"
#define SI64FMTD "%lld"
#endif

enum
{
    FT_NA='x',                                              //not used or unknown, 4 byte size
    FT_NA_BYTE='X',                                         //not used or unknown, byte
    FT_STRING='s',                                          //char*
    FT_FLOAT='f',                                           //float
    FT_INT='i',                                             //uint32
    FT_BYTE='b',                                            //uint8
    FT_SORT='d',                                            //sorted by this field, field is not included
    FT_IND='n',                                             //the same,but parsed to data
    FT_LOGIC='l'                                            //Logical (boolean)
};

#endif