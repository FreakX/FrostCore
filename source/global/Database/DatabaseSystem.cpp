#include <string>
#include <stdio.h>
#include <stdarg.h>
#include "DatabaseSystem.h"
#include "boost/thread.hpp"

Database::Database()
{
	Count = 0;
	m_connections = NULL;
	ConnectionCount = -1;   // Not connected.
    m_qbuff = new QueryBuffer;

    boost::thread workerThread(*this);
}

Database::~Database()
{
	for(int32 i = 0; i < ConnectionCount; ++i)
	{
		if( m_connections[i].conn != NULL )
			mysql_close(m_connections[i].conn);
	}

    delete m_qbuff;
	delete [] m_connections;
}

bool
Database::InitDB(std::string Hostname, unsigned int port, std::string Username, std::string Password, std::string DatabaseName, uint32 ConnectionCount, uint32 BufferSize)
{   
    uint32 i;
    MYSQL * temp, * temp2;
    my_bool my_true = true;

    Hostname = Hostname;
    ConnectionCount = ConnectionCount;
    Username = Username;
    Password = Password;
    DatabaseName = DatabaseName;

    printf( "%s> Connecting to %s database %s\n", __FUNCTION__, Hostname.c_str(), DatabaseLogName.c_str());

    m_connections = new DatabaseConnection[ConnectionCount];
    for (i = 0; i < ConnectionCount; ++i)
    {
        temp = mysql_init( NULL );
        if (mysql_options(temp, MYSQL_SET_CHARSET_NAME, "utf8"))
            printf("%s> MySQLDatabase\n", "Could not set utf8 character set.", __FUNCTION__);

        if (mysql_options(temp, MYSQL_OPT_RECONNECT, &my_true))
            printf("%s> MYSQL_OPT_RECONNECT could not be set, connection drops may occur but will be counteracted.", __FUNCTION__);

        temp2 = mysql_real_connect(temp, Hostname.c_str(), Username.c_str(), Password.c_str(), DatabaseName.c_str(), port, NULL, 0);
        if (temp2 == NULL)
        {
            printf("MySQLDatabase","Connection failed due to: `%s`", mysql_error(temp));
            return false;
        }

        m_connections[i].conn = temp2;
		m_connections[i].IsHandled = false;
    }
    return true;
}

void Database::SetDatabaseName(std::string new_name)
{
	DatabaseLogName = new_name;
}

DatabaseConnection* Database::GetFreeConnection()
{
	uint32 i = 0;
	for(;;)
	{
		DatabaseConnection * con = &m_connections[ ((++i) % ConnectionCount) ];
		if(!con->IsHandled)
			con->IsHandled = true;
			return con;
	}

	// shouldn't be reached
	return NULL;
}

QueryResult* Database::Query(const char* QueryString, ...)
{	
	char sql[16384];
	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(sql, 16384, QueryString, vlist);
	va_end(vlist);

	// Send the query
	QueryResult * qResult = NULL;
	DatabaseConnection * con = GetFreeConnection();

	if(_SendQuery(con, sql, false))
		qResult = _StoreQueryResult(con);
	
	con->IsHandled = false;
	return qResult;
}

void QueryBuffer::AddQuery(const char * format, ...)
{
	char query[16384];
	va_list vlist;
	va_start(vlist, format);
	vsnprintf(query, 16384, format, vlist);
	va_end(vlist);

	size_t len = strlen(query);
	char * pBuffer = new char[len+1];
	memcpy(pBuffer, query, len + 1);

	queries.push_back(pBuffer);
}

bool Database::Execute(const char* QueryString, ...)
{
	char query[16384];

	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(query, 16384, QueryString, vlist);
	va_end(vlist);

	size_t len = strlen(query);
	char * pBuffer = new char[len+1];
	memcpy(pBuffer, query, len + 1);

	return true;
}

bool Database::ExecuteNA(const char* QueryString)
{
	size_t len = strlen(QueryString);
	char * pBuffer = new char[len+1];
	memcpy(pBuffer, QueryString, len + 1);

	return true;
}

bool Database::WaitExecuteNA(const char* QueryString)
{
	DatabaseConnection * con = GetFreeConnection();
	bool Result = _SendQuery(con, QueryString, false);
	con->IsHandled = false;
	return Result;
}

void Database::FreeQueryResult(QueryResult * p)
{
	delete p;
}

std::string Database::EscapeString(std::string Escape)
{
	char a2[16384] = {0};

	DatabaseConnection * con = GetFreeConnection();
	const char * ret;
	if (mysql_real_escape_string(con->conn, a2, Escape.c_str(), (unsigned long)Escape.length()) == 0)
		ret = Escape.c_str();
	else
		ret = a2;

	return std::string(ret);
}

void Database::EscapeLongString(const char * str, uint32 len, std::stringstream& out)
{
	char a2[65536*3] = {0};

	DatabaseConnection * con = GetFreeConnection();
	const char * ret;
	if (mysql_real_escape_string(con->conn, a2, str, (unsigned long)len) == 0)
		ret = str;
	else
		ret = a2;
}

std::string Database::EscapeString(const char * esc, DatabaseConnection * con)
{
	char a2[16384] = {0};
	const char * ret;
	if (mysql_real_escape_string(con->conn, a2, (char*)esc, (unsigned long)strlen(esc)) == 0)
		ret = esc;
	else
		ret = a2;

	return std::string(ret);
}

bool Database::_SendQuery(DatabaseConnection *con, const char* Sql, bool Self)
{
	//dunno what it does ...leaving untouched 
	int result = mysql_query(con->conn, Sql);
	if (result > 0)
	{
		if (Self == false && _HandleError(con, mysql_errno(con->conn)))
		{
			// Re-send the query, the connection was successful.
			// The true on the end will prevent an endless loop here, as it will
			// stop after sending the query twice.
			result = _SendQuery(con, Sql, true);
		}
		else
		{
			const char * error = mysql_error( con->conn );
			FILE * fp = fopen("SqlError.txt","ab+");
			fprintf(fp,"Sql query failed due to [%s], Query: [%s]\n",error,Sql);
			fprintf(fp,"\n");
			fclose(fp);

			//sLog.Out(LOG_ERROR,"%s> Sql query failed due to [%s], Query: [%s]\n", __FUNCTION__,  error, Sql);
		}
	}

	return (result == 0 ? true : false);
}

bool Database::_HandleError(DatabaseConnection * con, uint32 ErrorNumber)
{
	// Handle errors that should cause a reconnect to the Database.
	switch(ErrorNumber)
	{
	    case 2006:  // Mysql server has gone away
	    case 2008:  // Client ran out of memory
	    case 2013:  // Lost connection to sql server during query
	    case 2055:  // Lost connection to sql server - system error
		{
			// Let's instruct a reconnect to the db when we encounter these errors.
			return _Reconnect(con);
		}
        break;
	}

	return false;
}

QueryResult::QueryResult(MYSQL_RES *res, uint32 fields, uint32 rows) : mResult(res), mFieldCount(fields), mRowCount(rows)
{
	mCurrentRow = new Fields[fields];
}

QueryResult::~QueryResult()
{
	mysql_free_result(mResult);
	delete [] mCurrentRow;
}

bool QueryResult::NextRow()
{
	MYSQL_ROW row = mysql_fetch_row(mResult);
	if (row == NULL)
		return false;

	for (uint32 i = 0; i < mFieldCount; ++i)
		mCurrentRow[i].SetValue(row[i]);

	return true;
}

QueryResult * Database::_StoreQueryResult(DatabaseConnection * con)
{
	QueryResult *res;
	MYSQL_RES * pRes = mysql_store_result(con->conn);
	uint32 uRows = (uint32)mysql_affected_rows(con->conn);
	uint32 uFields = (uint32)mysql_field_count(con->conn);

	if (uRows == 0 || uFields == 0 || pRes == 0)
	{
		if (pRes != NULL)
			mysql_free_result( pRes );

		return NULL;
	}

	res = new QueryResult( pRes, uFields, uRows );
	res->NextRow();

	return res;
}

bool Database::_Reconnect(DatabaseConnection * conn)
{
	MYSQL * temp, *temp2;

	temp = mysql_init( NULL );
	temp2 = mysql_real_connect(temp, HostName.c_str(), Username.c_str(), Password.c_str(), DatabaseName.c_str(), Port, NULL, 0);
	if (temp2 == NULL)
	{
		//sLog.Out(LOG_ERROR,"%s> Could not reconnect to database because of `%s`", __FUNCTION__, mysql_error( temp ) );
		mysql_close(temp);
		return false;
	}

	if (conn->conn != NULL)
		mysql_close(conn->conn);

	conn->conn = temp;
	return true;
}

void Database::CleanupLibs()
{
    mysql_library_end();
}

Database *Database::Create()
{
    return new Database();
}

void Database::Shutdown()
{
	for (int32 i = 0; i < ConnectionCount; ++i)
	{
		if (m_connections[i].conn != NULL)
		{
			mysql_close(m_connections[i].conn);
			m_connections[i].conn = NULL;
		}
	}
}

void Database::operator()()
{
    for(;;)
    {
        if(!m_qbuff->queries.size())
            continue;

        DatabaseConnection * con = GetFreeConnection();

        for(std::vector<std::string>::iterator itr = m_qbuff->queries.begin(); itr != m_qbuff->queries.end(); ++itr)
        {
            _SendQuery(con, (*itr).c_str(), false);
            m_qbuff->queries.erase(itr);
        }

        con->IsHandled = false;
    }
}