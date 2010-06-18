/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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

#include "stdafx.h"

#include "global.h"
#include "persist.h"

namespace persist {

PersistException::PersistException(std::string const& reason)
	: myMessage(reason)
{
	// Nothing :)
}

PersistException::~PersistException()
{
	// Nothing :)
}

std::string const& PersistException::GetMessage() const
{
	return myMessage;
}

const char* BaseObject::GetPersistenceID() const
{
	return "BaseObject";
}

BaseObject::BaseObject()
{
	// Do nothing
}

BaseObject::~BaseObject()
{
	// Do nothing
}

bool BaseObject::Write(Engine& archive) const
{
	// Do nothing
	return true; // Successfully
}

bool BaseObject::Read(Engine& archive)
{
	// Do nothing
	return true; // Successfully
}

static TypeManager::StringFunctionMap* theInstantiationFunctions = 0;
static int refCount = 0;

TypeManager::StringFunctionMap& _internal_GetMap()
{
	return *theInstantiationFunctions;
}
void TypeManager::Add(const char* name, NewBaseObjectFunction construction)
{
	if (refCount++ == 0)
	{
		theInstantiationFunctions = new StringFunctionMap;
	}
	ASSERT(_internal_GetMap().find(std::string(name)) == _internal_GetMap().end());
	_internal_GetMap()[std::string(name)] = construction;
}

void TypeManager::Remove(const char* name)
{
	ASSERT(_internal_GetMap().find(std::string(name)) != _internal_GetMap().end());
	_internal_GetMap().erase(_internal_GetMap().find(std::string(name)));
	if (--refCount == 0)
	{
		delete theInstantiationFunctions;
		theInstantiationFunctions = 0;
	}
}

BaseObject* TypeManager::CreateInstanceOf(const char* name)
{
	ASSERT(refCount);
	ASSERT(_internal_GetMap().find(std::string(name)) != _internal_GetMap().end());
	return (_internal_GetMap()[std::string(name)])();
}

/**
* NullObject is a const uint32 which is the ID streamed to disk
* if an attempt to stream a NULL Persistence::BaseObject or
* Derivative is made...
*/

const uint32 NullObject = 0xffffffff;

Engine::Engine(std::iostream& stream, EngineMode mode) THROWS (PersistException)
	: myUnderlyingIStream(&stream), myUnderlyingOStream(&stream), myOperationalMode(mode)
{
	// Nothing else to initialise for now
}

Engine::Engine(std::istream& stream) THROWS (PersistException)
	: myUnderlyingIStream(&stream), myOperationalMode(modeRead)
{
	// Nothing else to initialise for now
}

Engine::Engine(std::ostream& stream) THROWS (PersistException)
	: myUnderlyingOStream(&stream), myOperationalMode(modeWrite)
{
	// Nothing else to initialise for now
}

Engine::~Engine()
{
	// Flush compression buffers etc here.
}


void Engine::WriteBinary(const uint8* data, const uint32 size) 
	THROWS (Engine::Exception)
{
#ifdef HAVE_VALGRIND
	VALGRIND_CHECK_MEM_IS_DEFINED(data, size);
#endif
	ASSERT(myOperationalMode == modeWrite);
	myUnderlyingOStream->write((const char *)data,size);
}

void Engine::ReadBinary(uint8* data, uint32 size) THROWS (Engine::Exception)
{
	ASSERT(myOperationalMode == modeRead);
	myUnderlyingIStream->read((char *)data,size);
}

/*
* note, does not (yet?) throw an exception, but interface
* prepared ..
*/
void Engine::Write(const BaseObject *object) THROWS (Engine::Exception)
{
	ASSERT(myOperationalMode == modeWrite);
	
	// Pre-step, if object is NULL, then don't serialise it - serialise a
	// marker to say that it is null.
	// as ID's are uint32's, NullObject will do nicely for the task
	if (object == NULL)
	{
		uint32 id = NullObject;
		Write(id);
		return;
	}
	
	// First off - has this Object been serialised already?
	ArchiveMap::const_iterator itor = myArchiveMap.find(object);
	if (itor == myArchiveMap.end())
	{
		// Unfortunately we need to serialise it - here we go ....
		uint32 id = myArchiveMap.size();
		myArchiveMap[object] = id; // bumps id automatically for next one
		Write(id);
		ClassMap::const_iterator classItor = myClassMap.find(object->GetPersistenceID());
		if (classItor == myClassMap.end())
		{
			uint32 classId = myClassMap.size();
			myClassMap[object->GetPersistenceID()] = classId;
			Write(classId);
			Write(object->GetPersistenceID());
		}
		else
		{
			Write(classItor->second);
		}
		std::string majik;
		majik = "OBST";
		Write(majik);
		object->Write(*this);
		majik = "OBEN";
		Write(majik);
	}
	else
	{
		// This object has been serialised, so just pop its ID out
		Write(itor->second);
	}
}

void Engine::Read(BaseObject *&object) THROWS (Engine::Exception)
{
	ASSERT(myOperationalMode == modeRead);
	uint32 id = 0;
	Read(id);
	// Is the ID a NULL object?
	if (id == NullObject)
	{
		object = NULL;
		return;
	}
	
	// Do we already have this object in memory?
	if (id < myArchiveVector.size())
	{
		object = myArchiveVector[id];
		return;
	}
	
	// Okay - read the identifier for the class in...
	uint32 classId = 0;
	Read(classId);
	std::string className;
	if (classId < myClassVector.size())
	{
		className = myClassVector[classId];
	}
	else
	{
		// Okay the class wasn't known yet - get its name
		Read(className);
		myClassVector.push_back(className);
	}
	
	// Create the object (of the relevant type)
	object = TypeManager::CreateInstanceOf(className.c_str());
	if (object)
	{
		// Okay then - we can make this object
		myArchiveVector.push_back(object);
		std::string majik;
		Read(majik);
		ASSERT(majik == std::string("OBST"));
		object->Read(*this);
		Read(majik);
		ASSERT(majik == std::string("OBEN"));
	}
	else
		THROW (Exception((std::string("Unable to instantiate object of class ")+className).c_str()));
}

/*
* note, does not (yet?) throw an exception, but interface
* prepared ..
*/
void Engine::Write(const std::string& str) THROWS (Engine::Exception)
{
	ASSERT(myOperationalMode == modeWrite);
	uint32 len = str.length();
	Write(len);
	WriteBinary((uint8*)str.c_str(),len);
}

void Engine::Read(std::string& str) THROWS (Engine::Exception)
{
	ASSERT(myOperationalMode == modeRead);
	uint32 len = 0;
	Read(len);
	uint8 *buffer = new uint8[len+1];
	ReadBinary(buffer,len);
	buffer[len] = 0;
	str = (char*)buffer;
	delete[] buffer;
}

#define CCXX_RE(ar,ob)   ar.Read(ob); return ar
#define CCXX_WE(ar,ob)   ar.Write(ob); return ar

CCXX_EXPORT(Engine&) operator >>( Engine& ar, BaseObject *&ob) THROWS (Engine::Exception) { 
	CCXX_RE(ar,ob);
}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, BaseObject const *ob) THROWS (Engine::Exception) {
	CCXX_WE(ar,ob);
}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, int8& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int8 ob) THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint8& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint8 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, int16& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int16 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint16& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint16 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, int32& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int32 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint32& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint32 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, float& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, float ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, double& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, double ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, std::string& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, std::string ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, bool& ob) THROWS (Engine::Exception) {
	uint32 a; ar.Read(a); ob=a==1;return ar;
}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, bool ob) THROWS (Engine::Exception) {
	uint32 a=ob?1:0; ar.Write(a); return ar;
}

#undef CCXX_RE
#undef CCXX_WE

};
