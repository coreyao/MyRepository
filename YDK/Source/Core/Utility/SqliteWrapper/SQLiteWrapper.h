/* 
SQLiteWrapper.h

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

#ifndef SQLITE_WRAPPER_H__
#define SQLITE_WRAPPER_H__

#include <string>
#include <vector>

#include "sqlite3.h"
#include "BasicDefines.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CSQLiteStatement {
private:
	// SQLiteStatement's ctor only to be called by SQLiteWrapper
	friend class CSQLiteWrapper;
	CSQLiteStatement(std::string const& statement, sqlite3* db);

public:
	CSQLiteStatement();

	enum dataType {
		INT = SQLITE_INTEGER,
		FLT = SQLITE_FLOAT  ,
		TXT = SQLITE_TEXT   ,
		BLB = SQLITE_BLOB   ,
		NUL = SQLITE_NULL   ,
	};

	dataType DataType(int pos_zero_indexed);

	int         ValueInt   (int pos_zero_indexed);
	std::string ValueString(int pos_zero_indexed);
	const char*	ValueBlob  (int pos_zero_indexed);

	//    SQLiteStatement(const SQLiteStatement&);
	~CSQLiteStatement();

	//SQLiteStatement& operator=(SQLiteStatement const&);

	bool BindString    (int pos_zero_indexed, std::string const& value);
	bool BindDouble    (int pos_zero_indexed, double             value);
	bool BindInt	   (int pos_zero_indexed, int                value);
	bool BindNull(int pos_zero_indexed);
	bool BindBlob(int pos_zero_indexed, const char*        value, int len);

	bool Execute();

	bool NextRow();

	/*   Call Reset if not depending on the NextRow cleaning up.
	For example for select count(*) statements*/
	bool Reset();

	bool RestartSelect();

private:
	//void DecreaseRefCounter();

	//int* ref_counter_; // not yet used...
	sqlite3_stmt* stmt_;
};

class CSQLiteWrapper {
public:
	CSQLiteWrapper();

	bool Open(std::string const& db_file);
	bool Close();

	class ResultRecord {
	public:
		std::vector<std::string> fields_;
	};

	class ResultTable {
		friend class CSQLiteWrapper;
	public:
		ResultTable() : ptr_cur_record_(0) {}

		std::vector<ResultRecord> records_;

		ResultRecord* next() {
			if (ptr_cur_record_ < records_.size()) {
				return &(records_[ptr_cur_record_++]);
			}
			return 0;
		}

	private:
		void reset() {
			records_.clear();
			ptr_cur_record_=0;
		}

	private:
		unsigned int ptr_cur_record_;
	};

	bool SelectStmt           (std::string const& stmt, ResultTable& res);
	bool DirectStatement      (std::string const& stmt);
	CSQLiteStatement* Statement(std::string const& stmt);
	int GetLastInsertRowID();

	std::string LastError();

	// Transaction related
	bool Begin   ();
	bool Commit  ();
	bool Rollback();

private:

	static int SelectCallback(void *p_data, int num_fields, char **p_fields, char **p_col_names);

	sqlite3* db_;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif