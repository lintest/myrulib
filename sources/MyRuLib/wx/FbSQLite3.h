#ifndef __FBSQLITE3_H__
#define __FBSQLITE3_H__

#include <wx/wxsqlite3.h>
#include <wx/log.h>

#define FBSQLITE3_BEGIN \
	try {

#define FBSQLITE3_END(result) \
	} catch (wxSQLite3Exception e) { \
		if (e.GetErrorCode() != 9) wxLogError(e.GetMessage()); \
		return result; \
	}

class FbSQLite3ResultSet: private wxSQLite3ResultSet
{
public:
	FbSQLite3ResultSet()
		: wxSQLite3ResultSet() {}

	FbSQLite3ResultSet(const wxSQLite3ResultSet& resultSet)
		: wxSQLite3ResultSet(resultSet) {}

	FbSQLite3ResultSet& operator=(const wxSQLite3ResultSet& resultSet) {
		wxSQLite3ResultSet::operator=(resultSet);
		return *this;
	}

	FbSQLite3ResultSet& operator=(const FbSQLite3ResultSet& resultSet) {
		wxSQLite3ResultSet::operator=(resultSet);
		return *this;
	}

	bool NextRow() {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::NextRow();
		FBSQLITE3_END(false)
	}

	bool IsOk() {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::IsOk();
		FBSQLITE3_END(false)
	}

	bool Eof() {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::Eof();
		FBSQLITE3_END(false)
	}

	int GetColumnCount() {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::GetColumnCount();
		FBSQLITE3_END(0)
	}

	int GetInt(int columnIndex, int nullValue = 0) {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::GetInt(columnIndex, nullValue);
		FBSQLITE3_END(nullValue)
	}

	int GetInt(const wxString& columnName, int nullValue = 0) {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::GetInt(columnName, nullValue);
		FBSQLITE3_END(nullValue)
	}

	wxLongLong GetInt64(int columnIndex, wxLongLong nullValue = 0) {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::GetInt64(columnIndex, nullValue);
		FBSQLITE3_END(nullValue)
	}

	wxLongLong GetInt64(const wxString& columnName, wxLongLong nullValue = 0) {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::GetInt64(columnName, nullValue);
		FBSQLITE3_END(nullValue)
	}

	wxString GetString(int columnIndex, const wxString& nullValue = wxEmptyString) {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::GetString(columnIndex, nullValue);
		FBSQLITE3_END(nullValue)
	}

	wxString GetString(const wxString& columnName, const wxString& nullValue = wxEmptyString) {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::GetString(columnName, nullValue);
		FBSQLITE3_END(nullValue)
	}

	wxString GetAsString(int columnIndex) {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::GetAsString(columnIndex);
		FBSQLITE3_END(wxEmptyString)
	}

	wxString GetAsString(const wxString& columnName) {
		FBSQLITE3_BEGIN
		return wxSQLite3ResultSet::GetAsString(columnName);
		FBSQLITE3_END(wxEmptyString)
	}

	friend class FbSQLite3Statement;
};

class FbSQLite3Statement: private wxSQLite3Statement
{
public:
	FbSQLite3Statement() {}

	FbSQLite3Statement(const wxSQLite3Statement& statement)
		: wxSQLite3Statement(statement) {}

	FbSQLite3Statement(const FbSQLite3Statement& statement)
		: wxSQLite3Statement(statement) {}

	void FTS(int index, const wxString& value);

	void Bind(int paramIndex, const wxString& stringValue) {
		FBSQLITE3_BEGIN
		return wxSQLite3Statement::Bind(paramIndex, stringValue);
		FBSQLITE3_END()
	}

	void Bind(int paramIndex, int intValue) {
		FBSQLITE3_BEGIN
		return wxSQLite3Statement::Bind(paramIndex, intValue);
		FBSQLITE3_END()
	}

	void Bind(int paramIndex, wxLongLong int64Value) {
		FBSQLITE3_BEGIN
		return wxSQLite3Statement::Bind(paramIndex, int64Value);
		FBSQLITE3_END()
	}

	void Bind(int paramIndex, double doubleValue) {
		FBSQLITE3_BEGIN
		return wxSQLite3Statement::Bind(paramIndex, doubleValue);
		FBSQLITE3_END()
	}

	void Bind(int paramIndex, const char* charValue) {
		FBSQLITE3_BEGIN
		return wxSQLite3Statement::Bind(paramIndex, charValue);
		FBSQLITE3_END()
	}

	void Bind(int paramIndex, const unsigned char* blobValue, int blobLen) {
		FBSQLITE3_BEGIN
		return wxSQLite3Statement::Bind(paramIndex, blobValue, blobLen);
		FBSQLITE3_END()
	}

	void Bind(int paramIndex, const wxMemoryBuffer& blobValue) {
		FBSQLITE3_BEGIN
		return wxSQLite3Statement::Bind(paramIndex, blobValue);
		FBSQLITE3_END()
	}

	FbSQLite3ResultSet ExecuteQuery() {
		FBSQLITE3_BEGIN
		return wxSQLite3Statement::ExecuteQuery();
		FBSQLITE3_END(FbSQLite3ResultSet())
	}

	int ExecuteUpdate() {
		FBSQLITE3_BEGIN
		return wxSQLite3Statement::ExecuteUpdate();
		FBSQLITE3_END(0)
	}

	void Reset() {
		FBSQLITE3_BEGIN
		return wxSQLite3Statement::Reset();
		FBSQLITE3_END()
	}

	friend class FbSQLite3Database;
};

class FbSQLite3Database: private wxSQLite3Database
{
public:
	void Open(const wxString& fileName, const wxString& key = wxEmptyString, int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE)  {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::Open(fileName, key, flags);
		FBSQLITE3_END()
	}

	void Begin(wxSQLite3TransactionType transactionType = WXSQLITE_TRANSACTION_DEFAULT) {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::Begin(transactionType);
		FBSQLITE3_END()
	}

	bool IsOpen() const {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::IsOpen();
		FBSQLITE3_END(false)
	}

	void Close() {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::Close();
		FBSQLITE3_END()
	}

	void Commit() {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::Commit();
		FBSQLITE3_END()
	}

	FbSQLite3Statement PrepareStatement(const wxString& sql) {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::PrepareStatement(sql);
		FBSQLITE3_END(FbSQLite3Statement())
	}

	FbSQLite3ResultSet ExecuteQuery(const wxString& sql) {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::ExecuteQuery(sql);
		FBSQLITE3_END(FbSQLite3ResultSet())
	}

	int ExecuteSilent(const char* sql) {
		try { return wxSQLite3Database::ExecuteUpdate(sql); } catch (...) { return 0; }
	}

	int ExecuteSilent(const wxString& sql) {
		try { return wxSQLite3Database::ExecuteUpdate(sql); } catch (...) { return 0; }
	}

	int ExecuteUpdate(const char* sql) {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::ExecuteUpdate(sql);
		FBSQLITE3_END(0)
	}

	int ExecuteUpdate(const wxString& sql) {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::ExecuteUpdate(sql);
		FBSQLITE3_END(0)
	}

	int ExecuteScalar(const wxString& sql) {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::ExecuteScalar(sql);
		FBSQLITE3_END(0)
	}

	bool TableExists(const wxString& tableName, const wxString& databaseName = wxEmptyString) {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::TableExists(tableName, databaseName);
		FBSQLITE3_END(false)
	}

	bool CreateFunction(const wxString& name, int argCount, wxSQLite3ScalarFunction& function) {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::CreateFunction(name, argCount, function);
		FBSQLITE3_END(false)
	}

	bool CreateFunction(const wxString& name, int argCount, wxSQLite3AggregateFunction& function) {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::CreateFunction(name, argCount, function);
		FBSQLITE3_END(false)
	}

	void SetCollation(const wxString& name, wxSQLite3Collation* collation) {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::SetCollation(name, collation);
		FBSQLITE3_END()
	}

	void* GetDatabaseHandle() {
		FBSQLITE3_BEGIN
		return wxSQLite3Database::GetDatabaseHandle();
		FBSQLITE3_END(NULL)
	}

	friend class FbSQLite3Transaction;
};

class FbSQLite3Transaction: private wxSQLite3Transaction
{
public:
	explicit FbSQLite3Transaction(FbSQLite3Database* db, wxSQLite3TransactionType transactionType = WXSQLITE_TRANSACTION_DEFAULT)
		: wxSQLite3Transaction(db, transactionType) {}

	void Commit() {
		FBSQLITE3_BEGIN
		return wxSQLite3Transaction::Commit();
		FBSQLITE3_END()
	}

	void Rollback() {
		FBSQLITE3_BEGIN
		return wxSQLite3Transaction::Commit();
		FBSQLITE3_END()
	}
};

//class FbSQLite3Database:

#endif // __FBSQLITE3_H__


