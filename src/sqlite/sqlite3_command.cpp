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
#include <string>
#include <vector>
#include <stdexcept>
using namespace std;

#include "sqlite3_plus.h"
#include "sqlite3_internal.h"

namespace sqlite3 {	
	reader command::executereader() {
		reader r;
		r.con=&this->con;

		int ret=_sqlite3_prepare(this->con.db, this->cmdstr.c_str(), &r.vm);
		if(ret!=SQLITE_OK) throw runtime_error(string("executereader: ")+_sqlite3_errmsg(this->con.db));

		for(vector<parameter>::size_type i=0; i<this->parameters.size(); i++) {
			switch(this->parameters[i].type) {
				case parameter::dt_blob:
					_sqlite3_bind_blob(r.vm, this->parameters[i].index, this->parameters[i].data_blob, this->parameters[i].data_length);
					break;
				case parameter::dt_double:
					_sqlite3_bind_double(r.vm, this->parameters[i].index, this->parameters[i].data_double);
					break;
				case parameter::dt_int32:
					_sqlite3_bind_int(r.vm, this->parameters[i].index, this->parameters[i].data_int32);
					break;
				case parameter::dt_int64:
					_sqlite3_bind_int64(r.vm, this->parameters[i].index, this->parameters[i].data_int64);
					break;
				case parameter::dt_null:
					_sqlite3_bind_null(r.vm, this->parameters[i].index);
					break;
				case parameter::dt_text:
					_sqlite3_bind_text(r.vm, this->parameters[i].index, this->parameters[i].data_text, this->parameters[i].data_length);
					break;
			}
		}

		r.argc=_sqlite3_column_count(r.vm);

		return r;
	}

	int command::executenonquery() {
		reader r=this->executereader();

		r.read();
		r.close();

		return _sqlite3_changes(this->con.db);
	}
	
	int command::executeint32() {
		reader r=this->executereader();

		int ret=0;
		if(r.read()) ret=r.getint32(0);
		r.close();

		return ret;
	}

	__int64 command::executeint64() {
		reader r=this->executereader();

		__int64 ret=0;
		if(r.read()) ret=r.getint64(0);
		r.close();

		return ret;
	}

	double command::executedouble() {
		reader r=this->executereader();

		double ret=0.0;
		if(r.read()) ret=r.getdouble(0);
		r.close();

		return ret;
	}

	string command::executestring() {
		reader r=this->executereader();

		string ret;
		if(r.read()) ret=r.getstring(0);
		r.close();

		return ret;
	}

	string command::executeblob() {
		reader r=this->executereader();

		string ret;
		if(r.read()) ret=r.getblob(0);
		r.close();

		return ret;
	}
}
