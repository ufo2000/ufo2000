/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002       ufo2000 development team

This file contains slightly modified code for C++ objects 
persistance taken from CommonC++ library v1.9.7
WWW: http://cplusplus.sourceforge.net/
     http://www.gnu.org/software/CommonC++
Copyright (C) 1999-2001 Open Source Telecom Corporation.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef	CCXX_PERSIST_H_
#define	CCXX_PERSIST_H_

#ifndef CCXX_CONFIG_H_
#define CCXX_CONFIG_H_

#define CCXX_EMPTY

#define CCXX_EXPORT(t) t
#define CCXX_MEMBER(t) t
#define CCXX_MEMBER_EXPORT(t) t
#define CCXX_CLASS_EXPORT

#define THROW(x)
#define THROWS(x)

// use exception
#define HAVE_EXCEPTION 1

#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace persist {

class PersistException
{
public:
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) PersistException(const std::string& reason);
	CCXX_MEMBER_EXPORT(const std::string&) GetMessage() const;
	CCXX_MEMBER_EXPORT(virtual) ~PersistException();
protected:
	std::string myMessage;
};

// This typedef allows us to declare NewBaseObjectFunction now
typedef class BaseObject* (*NewBaseObjectFunction) (void);

/**
 * This class manages the types for generation of the persistent objects.
 * Its data structures are managed automatically by the system. They are
 * implicitly filled by the constructors who declare classes to the system.
 *
 * @author Daniel Silverstone
 * @short Type manager for persistence engine.
 */
class TypeManager
{
public:
	
	/**
	 * This manages a registration to the typemanager - attempting to
	 * remove problems with the optimisers
	 */
	class Registration
	{
	public:
		Registration(const char* name, NewBaseObjectFunction func)
			: myName(name) { TypeManager::Add(name,func); }
		virtual ~Registration() { TypeManager::Remove(myName.c_str()); }
	private:
		std::string myName;
	};
	
	/**
	 * This adds a new construction function to the type manager
	 */
	CCXX_MEMBER_EXPORT(static void) Add(const char* name, NewBaseObjectFunction construction);
	
	/**
	 * And this one removes a type from the managers lists
	 */
	CCXX_MEMBER_EXPORT(static void) Remove(const char* name);
	
	/**
	 * This function creates a new object of the required type and
	 * returns a pointer to it. NULL is returned if we couldn't find 
	 * the type
	 */
	CCXX_EXPORT(static BaseObject*) CreateInstanceOf(const char* name);
	
	typedef std::map<std::string,NewBaseObjectFunction> StringFunctionMap;
};


/*
 * The following defines are used to declare and define the relevant code
 * to allow a class to use the Persistence::Engine code.
 */

class DummyClass { };

#define DECLARE_PERSISTENCE(ClassType)					\
  public:								\
    friend persist::Engine& operator>>(persist::Engine& ar, ClassType *&ob);		\
    friend persist::Engine& operator<<(persist::Engine& ar, ClassType const *&ob);	\
    friend persist::BaseObject *CreateNew##ClassType();				\
    ClassType(persist::DummyClass dummy) { } \
    virtual const char* GetPersistenceID() const;			\
    static persist::TypeManager::Registration RegistrationFor##ClassType;

#define IMPLEMENT_PERSISTENCE(ClassType,FullyQualifiedName)		      \
  persist::BaseObject *CreateNew##ClassType() { return new ClassType(persist::DummyClass()); }  \
  const char* ClassType::GetPersistenceID()const {return FullyQualifiedName;} \
  persist::Engine& operator>>(persist::Engine& ar, ClassType *&ob)			      \
    { ar >> (persist::BaseObject *&) ob; return ar; }				      \
  persist::Engine& operator<<(persist::Engine& ar, ClassType const *ob)			      \
    { ar << (persist::BaseObject const *)ob; return ar; }			      \
  persist::TypeManager::Registration						      \
        ClassType::RegistrationFor##ClassType(FullyQualifiedName,	      \
					      CreateNew##ClassType);

class Engine;

/**
 * BaseObject
 *
 * This object is the base for all Persistent data which is not
 * natively serialised by the Persistence::Engine
 *
 * It registers itself with the Persistence::TypeManager
 * using a global constructor function. A matching deregister call
 * is made in a global destructor, to allow DLL's to use the
 * Persistence::Engine in a main executable.
 *
 * @author Daniel Silverstone
 * @short Base class for classes that will be persistent.
 */
class BaseObject
{
public:
	/**
	 * This constructor is used in serialisation processes.
	 * It is called in CreateNewInstance in order to create
	 * an instance of the class to have Read() called on it.
	 */
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) BaseObject();
	
	/**
	 * Default destructor
	 */
	CCXX_MEMBER_EXPORT(virtual) ~BaseObject();
	
	/**
	 * This returns the ID of the persistent object (Its type)
	 */
	CCXX_MEMBER_EXPORT(virtual const char*) GetPersistenceID() const;
	
	/**
	 * This method is used to write to the Persistence::Engine
	 * It is not equivalent to the << operator as it writes only the data
	 * and not the object type etc.
	 */
	CCXX_MEMBER_EXPORT(virtual bool) Write(Engine& archive) const;

	/**
	 * This method is used to read from a Persistence::Engine
	 * It is not equivalent to the >> operator as it does no 
	 * typesafety or anything.
	 */
	CCXX_MEMBER_EXPORT(virtual bool) Read(Engine& archive);
};


/**
 * Engine
 *
 * This class constructs on a standard C++ STL stream and then
 * operates in the mode specified. 
 *
 * @author Daniel Silverstone
 * @short stream serialization of persistent classes.
 */

class Engine
{
public:
	/**
	 * If problems happen which are fatal - expect one of these thrown 
	 * at you
	 */
	class Exception : public PersistException
	{
	public:
		CCXX_MEMBER_EXPORT(CCXX_EMPTY) Exception(const std::string &reason) : PersistException(reason) {}
	};
	
	/**
	 * These are the modes the Persistence::Engine can work in
	 */
	enum EngineMode
	{
		modeRead,
		modeWrite
	};
	
	/**
	 * Constructs a Persistence::Engine with the specified stream in
	 * the given mode. The stream must be initialised properly prior
	 * to this call or problems will ensue.
	 */
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) Engine(std::iostream& stream, EngineMode mode) THROWS (PersistException);
	
	/**
	 * This Flushes the buffers and closes the Persistence::Engine
	 * this must happen before the underlying stream is shut down
	 */
	CCXX_MEMBER_EXPORT(virtual) ~Engine();


	// Write operations
	CCXX_MEMBER_EXPORT(void) Write(const BaseObject *object) THROWS (Exception);
	/*
	 * shortcut, to make the following more readable
	 */
#define CCXX_ENGINEWRITE_REF(valref) WriteBinary((const uint8*)&valref,sizeof(valref))
	void Write(int8 i)   THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void Write(uint8 i)  THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void Write(int16 i)  THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void Write(uint16 i) THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void Write(int32 i)  THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void Write(uint32 i) THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void Write(float i)  THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void Write(double i) THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
#undef CCXX_ENGINEWRITE_REF

	CCXX_MEMBER_EXPORT(void) Write(const std::string& str) THROWS (Exception);
	// Every write operation boils down to one or more of theses
	CCXX_MEMBER_EXPORT(void) WriteBinary(const uint8* data, const uint32 size) THROWS (Exception);
	
	// Read Operations
	
	CCXX_MEMBER_EXPORT(void) Read(BaseObject *&object) THROWS (Exception);
#define CCXX_ENGINEREAD_REF(valref) ReadBinary((uint8*)&valref,sizeof(valref))
	void Read(int8& i)   THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void Read(uint8& i)  THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void Read(int16& i)  THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void Read(uint16& i) THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void Read(int32& i)  THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void Read(uint32& i) THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void Read(float& i)  THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void Read(double& i) THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
#undef CCXX_ENGINEREAD_REF

	CCXX_MEMBER_EXPORT(void) Read(std::string& str) THROWS (Exception);
	// Every read operation boild down to one or more of these
	CCXX_MEMBER_EXPORT(void) ReadBinary(uint8* data, uint32 size) THROWS (Exception);

private:
	/**
	 * The underlying stream
	 */
	std::iostream& myUnderlyingStream;
	
	/**
	 * The mode of the engine
	 */
	EngineMode myOperationalMode;

	/**
	 * Typedefs for the Persistence::BaseObject support
	 */
	typedef std::vector<BaseObject*>           ArchiveVector;
	typedef std::map<BaseObject const*, int32> ArchiveMap;
	typedef std::vector<std::string>                ClassVector;
	typedef std::map<std::string, int32>            ClassMap;
	
	ArchiveVector myArchiveVector;
	ArchiveMap myArchiveMap;
	ClassVector myClassVector;
	ClassMap myClassMap;

	// Compression support
};

// Standard >> and << stream operators for BaseObject
CCXX_EXPORT(Engine&) operator >>( Engine& ar, BaseObject *&ob)      THROWS (Engine::Exception);
CCXX_EXPORT(Engine&) operator <<( Engine& ar, BaseObject const *ob) THROWS (Engine::Exception);

CCXX_EXPORT(Engine&) operator >>( Engine& ar, int8& ob) THROWS (Engine::Exception);
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int8 ob)  THROWS (Engine::Exception);

CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint8& ob) THROWS (Engine::Exception);
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint8 ob)  THROWS (Engine::Exception);

CCXX_EXPORT(Engine&) operator >>( Engine& ar, int16& ob) THROWS (Engine::Exception);
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int16 ob)  THROWS (Engine::Exception);

CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint16& ob) THROWS (Engine::Exception);
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint16 ob)  THROWS (Engine::Exception);

CCXX_EXPORT(Engine&) operator >>( Engine& ar, int32& ob) THROWS (Engine::Exception);
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int32 ob)  THROWS (Engine::Exception);

CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint32& ob) THROWS (Engine::Exception);
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint32 ob)  THROWS (Engine::Exception);

CCXX_EXPORT(Engine&) operator >>( Engine& ar, float& ob) THROWS (Engine::Exception);
CCXX_EXPORT(Engine&) operator <<( Engine& ar, float ob)  THROWS (Engine::Exception);

CCXX_EXPORT(Engine&) operator >>( Engine& ar, double& ob) THROWS (Engine::Exception);
CCXX_EXPORT(Engine&) operator <<( Engine& ar, double ob)  THROWS (Engine::Exception);

CCXX_EXPORT(Engine&) operator >>( Engine& ar, std::string& ob) THROWS (Engine::Exception);
CCXX_EXPORT(Engine&) operator <<( Engine& ar, std::string ob)  THROWS (Engine::Exception);

CCXX_EXPORT(Engine&) operator >>( Engine& ar, bool& ob) THROWS (Engine::Exception);
CCXX_EXPORT(Engine&) operator <<( Engine& ar, bool ob)  THROWS (Engine::Exception);

/**
 * The following are templated classes
 */

/**
 * serialize a vector of some serializable content to
 * the engine
 */
template<class T>
Engine& operator <<( Engine& ar, typename std::vector<T> const& ob) THROWS (Engine::Exception)
{
	ar << (uint32)ob.size();
	for(uint32 i=0; i < ob.size(); ++i)
		ar << ob[i];
	return ar;
}

/**
 * deserialize a vector of deserializable content from
 * an engine.
 */
template<class T>
Engine& operator >>( Engine& ar, typename std::vector<T>& ob) THROWS (Engine::Exception)
{
	ob.clear();
	uint32 siz;
	ar >> siz;
	ob.resize(siz);
	for(uint32 i=0; i < siz; ++i)
		ar >> ob[i];
	return ar;
}

/**
 * serialize a map with keys/values which both are serializeable
 * to an engine.
 */
template<class Key, class Value>
Engine& operator <<( Engine& ar, typename std::map<Key,Value> const & ob) THROWS (Engine::Exception)
{
	ar << (uint32)ob.size();
	for(typename std::map<Key,Value>::const_iterator it = ob.begin();it != ob.end();++it)
		ar << it->first << it->second;
	return ar;
}

/**
 * deserialize a map with keys/values which both are serializeable
 * from an engine.
 */
template<class Key, class Value>
Engine& operator >>( Engine& ar, typename std::map<Key,Value>& ob) THROWS (Engine::Exception)
{
	ob.clear();
	uint32 siz;
	ar >> siz;
	for(uint32 i=0; i < siz; ++i) {
		Key a;
		ar >> a;
		ar >> ob[a];
	}
	return ar;
}

};

#endif
