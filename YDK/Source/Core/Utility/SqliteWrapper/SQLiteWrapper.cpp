/* 
SQLiteWrapper.cpp

Copyright (C) 2004 Ren¨¦ Nyffenegger

This source code is provided 'as-is', without any express or implied
warranty. In no event will the author be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this source code must not be misrepresented; you must not
claim that you wrote the original source code. If you use this source code
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original source code.

3. This notice may not be removed or altered from any source distribution.

Ren¨¦ Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

#include "SQLiteWrapper.h"
// TODO: raus
#include <windows.h>

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CSQLiteWrapper::CSQLiteWrapper() : db_(0) {
}

bool CSQLiteWrapper::Open(std::string const& db_file) {
	if (sqlite3_open(db_file.c_str(), &db_) != SQLITE_OK) {
		return false;
	}
	return true;
} 

bool CSQLiteWrapper::Close()
{
	if (sqlite3_close(db_) != SQLITE_OK)
	{
		return false;
	}

	return true;
}

bool CSQLiteWrapper::SelectStmt(std::string const& stmt, ResultTable& res) {
	char *errmsg;
	int   ret;

	res.reset();

	ret = sqlite3_exec(db_, stmt.c_str(), SelectCallback, static_cast<void*> (&res), &errmsg);

	if (ret != SQLITE_OK) {
		return false;
	}
	return true;
	//  if (ret != SQLITE_OK) {
	//    std::cout << stmt << " [" << errmsg << "]" << std::endl;
	//  }
}

// TODO parameter p_col_names
int CSQLiteWrapper::SelectCallback(void *p_data, int num_fields, char **p_fields, char** p_col_names) {
	ResultTable* res = reinterpret_cast<ResultTable*>(p_data);

	ResultRecord record;

#ifdef SQLITE_WRAPPER_REPORT_COLUMN_NAMES
	// Hubert Castelain: column names in the first row of res if res is empty

	if(res->records_.size()==0) {
		ResultRecord col_names;

		for(int i=0; i < num_fields; i++) {
			if(p_fields[i]) col_names.fields_.push_back (p_col_names[i]);
			else
				col_names.fields_.push_back("(null)"); // or what else ?
		}
		res->records_.push_back(col_names);
	}
#endif

	for(int i=0; i < num_fields; i++) {
		// Hubert Castelain: special treatment if null
		if (p_fields[i]) record.fields_.push_back(p_fields[i]);
		else             record.fields_.push_back("<null>");
	}

	res->records_.push_back(record);

	return 0;
}

CSQLiteStatement* CSQLiteWrapper::Statement(std::string const& statement) {
	CSQLiteStatement* stmt;
	try {
		stmt = new CSQLiteStatement(statement, db_);
		return stmt;
	}
	catch (const char* e) {
		e;
		return 0;
	}
}

CSQLiteStatement::CSQLiteStatement(std::string const& statement, sqlite3* db) {
	if ( sqlite3_prepare(
		db, 
		statement.c_str(),  // stmt
		-1,                  // If than zero, then stmt is read up to the first nul terminator
		&stmt_,
		0                   // Pointer to unused portion of stmt
		)
		!= SQLITE_OK) {
			throw sqlite3_errmsg(db);
	}

	if (!stmt_) {
		throw "stmt_ is 0";
	}
}

CSQLiteStatement::~CSQLiteStatement() {
	// Hubert Castelain 28/8/2005
	// Prevent the database remaining locked after some statement.
	// syntax: int sqlite3_finalize(sqlite3_stmt *pStmt);
	if(stmt_) sqlite3_finalize(stmt_);
}

CSQLiteStatement::CSQLiteStatement() :
stmt_       (0)
{
}

bool CSQLiteStatement::BindString(int pos_zero_indexed, std::string const& value) {
	if (sqlite3_bind_text (
		stmt_,
		pos_zero_indexed+1,  // Index of wildcard
		value.c_str(),
		value.length(),      // length of text
		SQLITE_TRANSIENT     // SQLITE_TRANSIENT: SQLite makes its own copy
		)
		!= SQLITE_OK) {
			return false;
	}
	return true;
}

bool CSQLiteStatement::BindDouble(int pos_zero_indexed, double value) {
	if (sqlite3_bind_double(
		stmt_,
		pos_zero_indexed+1,  // Index of wildcard
		value
		)
		!= SQLITE_OK) {
			return false;
	}
	return true;
}

bool CSQLiteStatement::BindInt(int pos_zero_indexed, int value) {
	if (sqlite3_bind_int(
		stmt_,
		pos_zero_indexed+1,  // Index of wildcard
		value 
		)
		!= SQLITE_OK) {
			return false;
	}
	return true;
}

bool CSQLiteStatement::BindNull(int pos_zero_indexed) {
	if (sqlite3_bind_null(
		stmt_,
		pos_zero_indexed+1  // Index of wildcard
		)
		!= SQLITE_OK) {
			return false;
	}
	return true;
}

bool CSQLiteStatement::BindBlob(int pos_zero_indexed, const char* value, int len)
{
	if (sqlite3_bind_blob(
		stmt_,
		pos_zero_indexed+1,  // Index of wildcard
		value,
		len,
		SQLITE_TRANSIENT
		)
		!= SQLITE_OK) {
			return false;
	}
	return true;
}

bool CSQLiteStatement::Execute() {
	int rc = sqlite3_step(stmt_);
	if (rc == SQLITE_BUSY) {
		::MessageBox(0, "SQLITE_BUSY", 0, 0); 
		return false;
	}
	if (rc == SQLITE_ERROR) {
		::MessageBox(0, "SQLITE_ERROR", 0, 0); 
		return false;
	}
	if (rc == SQLITE_MISUSE) {
		::MessageBox(0, "SQLITE_ERROR", 0, 0); 
		return false;
	}
	if (rc != SQLITE_DONE) {
		//sqlite3_reset(stmt_);
		return false;
	}
	sqlite3_reset(stmt_);
	return true;
}

CSQLiteStatement::dataType CSQLiteStatement::DataType(int pos_zero_indexed) {
	return dataType(sqlite3_column_type(stmt_, pos_zero_indexed));
}

int CSQLiteStatement::ValueInt(int pos_zero_indexed) {
	return sqlite3_column_int(stmt_, pos_zero_indexed);
}

std::string CSQLiteStatement::ValueString(int pos_zero_indexed) {
	return std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt_, pos_zero_indexed)));
}

const char* CSQLiteStatement::ValueBlob(int pos_zero_indexed)
{
	return reinterpret_cast<const char*>(sqlite3_column_blob(stmt_, pos_zero_indexed));
}

bool CSQLiteStatement::RestartSelect() {
	sqlite3_reset(stmt_);
	return true;
}

bool CSQLiteStatement::Reset() {
	int rc = sqlite3_step(stmt_);

	sqlite3_reset(stmt_);

	if (rc == SQLITE_ROW) return true;
	return false;
}

bool CSQLiteStatement::NextRow() {
	int rc = sqlite3_step(stmt_);

	if (rc == SQLITE_ROW   ) {
		return true;
	}
	if (rc == SQLITE_DONE  ) {
		sqlite3_reset(stmt_);
		return false;
	} 
	else if (rc == SQLITE_MISUSE) {
		::MessageBox(0,  "SQLiteStatement::NextRow SQLITE_MISUSE", 0, 0);
	} 
	else if (rc == SQLITE_BUSY  ) {
		::MessageBox(0, "SQLiteStatement::NextRow SQLITE_BUSY", 0, 0);
	} 
	else if (rc == SQLITE_ERROR ) {
		::MessageBox(0, "SQLiteStatement::NextRow SQLITE_ERROR", 0, 0);
	}
	return false;
}

bool CSQLiteWrapper::DirectStatement(std::string const& stmt) {
	char *errmsg;
	int   ret;

	ret = sqlite3_exec(db_, stmt.c_str(), 0, 0, &errmsg);

	if(ret != SQLITE_OK) {
		return false;
	}
	return true;

	//if(ret != SQLITE_OK) {
	//  std::cout << stmt << " [" << errmsg << "]" << std::endl;
	//}
}

std::string CSQLiteWrapper::LastError() {
	return sqlite3_errmsg(db_);
}

bool CSQLiteWrapper::Begin() {
	return DirectStatement("begin");
}

bool CSQLiteWrapper::Commit() {
	return DirectStatement("commit");
}

bool CSQLiteWrapper::Rollback() {
	return DirectStatement("rollback");
}

int CSQLiteWrapper::GetLastInsertRowID()
{
	return static_cast<int>(sqlite3_last_insert_rowid(db_));
}
END_NAMESPACE(Utility)
END_NAMESPACE(Core)
