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

#ifndef __SQLITE3_INTERNAL_H__
#define __SQLITE3_INTERNAL_H__

#include <cstdarg>

#ifndef SQLITE_OK
#define SQLITE_OK 0
#endif

#ifndef SQLITE_ROW
#define SQLITE_ROW 100
#endif

#ifndef SQLITE_DONE
#define SQLITE_DONE 101
#endif

#ifdef LINUX
typedef long long __int64;
#endif

const char *_sqlite3_errmsg(void *db);

int _sqlite3_open(const char *filename, void **db);
int _sqlite3_close(void *db);

int _sqlite3_prepare(void *db, const char *sql, void **vm);
int _sqlite3_step(void *vm);
int _sqlite3_finalize(void *vm);
int _sqlite3_reset(void *vm);

int _sqlite3_column_count(void *vm);
int _sqlite3_column_bytes(void *vm, int index);
int _sqlite3_column_int(void *vm, int index);
__int64 _sqlite3_column_int64(void *vm, int index);
double _sqlite3_column_double(void *vm, int index);
const char *_sqlite3_column_text(void *vm, int index);
const void *_sqlite3_column_blob(void *vm, int index);
const char *_sqlite3_column_name(void *vm, int index);

int _sqlite3_bind_blob(void *vm, int index, const void *data, int datalen);
int _sqlite3_bind_double(void *vm, int index, double data);
int _sqlite3_bind_int(void *vm, int index, int data);
int _sqlite3_bind_int64(void *vm, int index, __int64 data);
int _sqlite3_bind_null(void *vm, int index);
int _sqlite3_bind_text(void *vm, int index, const char *data, int datalen);

__int64 _sqlite3_last_insert_rowid(void *db);
int _sqlite3_changes(void *db);

char *_sqlite3_vmprintf(const char *fmt, va_list val);
void _sqlite3_free(char *str);

#endif
