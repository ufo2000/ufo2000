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

#ifndef __SQLITE3_PLUS_H__
#define __SQLITE3_PLUS_H__

#include <string>
#include <vector>
#include <cstdarg>

namespace sqlite3 {
	class connection {
		friend class reader;
		friend class command;
	private:
		void *db;

		class reader _executereader(const char *fmt, va_list args);
	
	public:
		connection();
		connection(const char *db);
		~connection();

		void open(const char *db);
		void close();

		__int64 insertid() const;

		int executenonquery(const char *fmt, ...);
		class reader executereader(const char *fmt, ...);
		
		int executeint32(const char *fmt, ...);
		__int64 executeint64(const char *fmt, ...);
		double executedouble(const char *fmt, ...);
		std::string executestring(const char *fmt, ...);
		std::string executeblob(const char *fmt, ...);
	};

	struct parameter {
		typedef enum {
			dt_blob, dt_double, dt_int32, dt_int64, dt_null, dt_text
		} datatype;

		int index;
		datatype type;
		union {
			struct {
				union {
					const void *data_blob;
					const char *data_text;
				};
				int data_length;
			};
			double data_double;
			int data_int32;
			__int64 data_int64;
		};

		parameter() {};
		parameter(int index) : index(index),type(dt_null) {};
		parameter(int index, const void *data, int length) : index(index),type(dt_blob),data_blob(data),data_length(length) {};
		parameter(int index, const char *data, int length) : index(index),type(dt_text),data_text(data),data_length(length) {};
		parameter(int index, double data) : index(index),type(dt_double),data_double(data) {};
		parameter(int index, int data) : index(index),type(dt_int32),data_int32(data) {};
		parameter(int index, __int64 data) : index(index),type(dt_int64),data_int64(data) {};
	};

	class command {
	private:
		class connection &con;

	public:
		std::string cmdstr;
		std::vector<parameter> parameters;

		command(connection &c) : con(c) {};
		command(connection &c, const char *sql) : con(c),cmdstr(sql) {};

		int executenonquery();
		class reader executereader();
		
		int executeint32();
		__int64 executeint64();
		double executedouble();
		std::string executestring();
		std::string executeblob();
	};

	class reader {
		friend class connection;
		friend class command;

	private:
		class connection *con;
		void *vm;
		int argc;

	public:
		reader();
		bool read();
		void close();
		void reset();

		int getint32(int index) const;
		__int64 getint64(int index) const;
		double getdouble(int index) const;
		std::string getstring(int index) const;
		std::string getblob(int index) const;
		std::string getcolname(int index) const;

		std::string operator[](int index) const;
	};
};

#endif
