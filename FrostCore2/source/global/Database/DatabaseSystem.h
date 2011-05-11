#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include <vector>
#include "mysql/mysql.h"
#include "Types.h"
#include "Fields.h"

class QueryResult;
class QueryThread;
class Database;

struct DatabaseConnection
{
    MYSQL *conn;
	bool IsHandled;
};

struct QueryBuffer
{
	    std::vector<std::string> queries;
    public:
	    friend class Database;
	    void AddQuery(const char * format, ...);
	    void AddQueryNA(const char * str);
	    void AddQueryStr(const std::string& str);
};

class Database
{
    public:
        Database();
        virtual ~Database();

        bool InitDB(std::string Hostname,
                    unsigned int port,
                    std::string Username,
                    std::string Password,
                    std::string DatabaseName,
                    uint32 ConnectionCount,
                    uint32 BufferSize);

        void Shutdown();
        void SetDatabaseName(std::string name);

        void operator()();

        QueryResult* Query(const char* QueryString, ...);
        bool Execute(const char* QueryString, ...);
        bool ExecuteNA(const char* QueryString);
        bool WaitExecuteNA(const char* QueryString);//Wait For Request Completion

        std::string EscapeString(std::string Escape);
        void EscapeLongString(const char * str, uint32 len, std::stringstream& out);
        std::string EscapeString(const char * esc, DatabaseConnection *con);

        DatabaseConnection *GetFreeConnection();
        static Database *Create();
        static void CleanupLibs();

        void AddQueryBuffer(QueryBuffer * b);
        void FreeQueryResult(QueryResult * p);
    protected:
        bool _SendQuery(DatabaseConnection *con, const char* Sql, bool Self);
        QueryResult* _StoreQueryResult(DatabaseConnection *con);
        bool _HandleError(DatabaseConnection *conn, uint32 ErrorNumber);
        bool _Reconnect(DatabaseConnection *conn);

        int ConnectionCount;
        uint32 Count;
        std::string HostName;
        std::string Username;
        std::string Password;
        std::string DatabaseName;
        std::string DatabaseLogName;
        uint32 Port;

        QueryBuffer* m_qbuff;

        DatabaseConnection *m_connections;
};

class QueryResult
{
    public:
	    QueryResult(MYSQL_RES *res, uint32 fields, uint32 rows);
	    ~QueryResult();

	    bool NextRow();
	    void Delete() { delete this; }

	    inline Fields* Fetch() { return mCurrentRow; }
	    inline uint32 GetFieldCount() const { return mFieldCount; }
	    inline uint32 GetRowCount() const { return mRowCount; }

    protected:
	    uint32 mFieldCount;
	    uint32 mRowCount;
        Fields *mCurrentRow;
	    MYSQL_RES *mResult;
};

#endif
