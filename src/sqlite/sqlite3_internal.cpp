/*
	Copyright (c) 2004 Cory Nelson

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <cstdlib>
#include <cstdarg>
#include <sqlite3.h>
#include "sqlite3_internal.h"

const char *_sqlite3_errmsg(void *db) {
	return sqlite3_errmsg((sqlite3*)db);
}

int _sqlite3_open(const char *filename, void **db) {
	return sqlite3_open(filename, (sqlite3**)db);
}

int _sqlite3_close(void *db) {
	return sqlite3_close((sqlite3*)db);
}

int _sqlite3_prepare(void *db, const char *sql, void **vm) {
	const char *tail=NULL;
	return sqlite3_prepare((sqlite3*)db, sql, -1, (sqlite3_stmt**)vm, &tail);
}

int _sqlite3_step(void *vm) {
	return sqlite3_step((sqlite3_stmt*)vm);
}

int _sqlite3_finalize(void *vm) {
	return sqlite3_finalize((sqlite3_stmt*)vm);
}

int _sqlite3_reset(void *vm) {
	return sqlite3_reset((sqlite3_stmt*)vm);
}

int _sqlite3_column_count(void *vm) {
	return sqlite3_column_count((sqlite3_stmt*)vm);
}

int _sqlite3_column_bytes(void *vm, int index) {
	return sqlite3_column_bytes((sqlite3_stmt*)vm, index);
}

int _sqlite3_column_int(void *vm, int index) {
	return sqlite3_column_int((sqlite3_stmt*)vm, index);
}

__int64 _sqlite3_column_int64(void *vm, int index) {
	return sqlite3_column_int64((sqlite3_stmt*)vm, index);
}

double _sqlite3_column_double(void *vm, int index) {
	return sqlite3_column_double((sqlite3_stmt*)vm, index);
}

const char *_sqlite3_column_text(void *vm, int index) {
	return (const char*)sqlite3_column_text((sqlite3_stmt*)vm, index);
}

const void *_sqlite3_column_blob(void *vm, int index) {
	return sqlite3_column_blob((sqlite3_stmt*)vm, index);
}

const char *_sqlite3_column_name(void *vm, int index) {
	return sqlite3_column_name((sqlite3_stmt*)vm, index);
}

int _sqlite3_bind_blob(void *vm, int index, const void *data, int datalen) {
	return sqlite3_bind_blob((sqlite3_stmt*)vm, index, data, datalen, SQLITE_TRANSIENT);
}

int _sqlite3_bind_double(void *vm, int index, double data) {
	return sqlite3_bind_double((sqlite3_stmt*)vm, index, data);
}

int _sqlite3_bind_int(void *vm, int index, int data) {
	return sqlite3_bind_int((sqlite3_stmt*)vm, index, data);
}

int _sqlite3_bind_int64(void *vm, int index, __int64 data) {
	return sqlite3_bind_int64((sqlite3_stmt*)vm, index, data);
}

int _sqlite3_bind_null(void *vm, int index) {
	return sqlite3_bind_null((sqlite3_stmt*)vm, index);
}

int _sqlite3_bind_text(void *vm, int index, const char *data, int datalen) {
	return sqlite3_bind_text((sqlite3_stmt*)vm, index, data, datalen, SQLITE_TRANSIENT);
}

__int64 _sqlite3_last_insert_rowid(void *db) {
	return sqlite3_last_insert_rowid((sqlite3*)db);
}

int _sqlite3_changes(void *db) {
	return sqlite3_changes((sqlite3*)db);
}

char *_sqlite3_vmprintf(const char *fmt, va_list val) {
	return sqlite3_vmprintf(fmt, val);
}

void _sqlite3_free(char *str) {
	sqlite3_free(str);
}
