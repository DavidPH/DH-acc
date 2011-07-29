/* Copyright (C) 2011 David Hill
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* SourceVariable/makeObjectsGet.cpp
**
** Defines the SourceVariable::makeObjectsGet* methods.
*/

#include "../SourceVariable.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



void SourceVariable::makeObjectsGet(ObjectVector * objects, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	makeObjectsGetPrep(objects, &address, NULL);
	makeObjectsGet(objects, position, _type, &address);
}
void SourceVariable::makeObjectsGet(ObjectVector * objects, SourcePosition const & position, VariableType const * type, int * address) const
{
	ObjectToken::ObjectCode ocode;

	switch (_sc)
	{
	case SC_CONSTANT:
		ocode = ObjectToken::OCODE_PUSHNUMBER;

		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_ARRAY:
		case VT_BLOCK:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_STRUCT:
		case VT_STRING:
		case VT_SCRIPT:
			objects->addToken(ocode, makeObject(position));
			++*address;
			break;

		case VT_ASMFUNC:
		case VT_VOID:
			break;
		}
		break;

	case SC_REGISTER:
		ocode = ObjectToken::OCODE_PUSHSCRIPTVAR;
	sc_register_case:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
			objects->addToken(ocode, objects->getValue((*address)++));
			break;

		case VT_ARRAY:
			throw SourceException("makeObjectsGet on register VT_ARRAY", position, "SourceVariable");

		case VT_ASMFUNC:
		case VT_VOID:
			break;

		case VT_BLOCK:
		case VT_STRUCT:
			for (size_t i(0); i < type->types.size(); ++i)
				makeObjectsGet(objects, position, type->types[i], address);
			break;
		}
		break;

	case SC_REGISTER_GLOBAL:
		ocode = ObjectToken::OCODE_PUSHGLOBALVAR;
		goto sc_register_case;

	case SC_REGISTER_MAP:
		ocode = ObjectToken::OCODE_PUSHMAPVAR;
		goto sc_register_case;

	case SC_REGISTER_WORLD:
		ocode = ObjectToken::OCODE_PUSHWORLDVAR;
		goto sc_register_case;

	sc_registerarray_case:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue((*address)++));
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(0));
			objects->addToken(ocode, objects->getValue(_address));
			objects->addToken(ObjectToken::OCODE_ADD);
			objects->addToken(ocode, objects->getValue(_address));
			break;

		case VT_ARRAY:
		case VT_BLOCK:
		case VT_STRUCT:
			for (size_t i(0); i < type->types.size(); ++i)
				makeObjectsGet(objects, position, type->types[i], address);
			break;

		case VT_ASMFUNC:
		case VT_VOID:
			break;
		}
		break;

	case SC_REGISTERARRAY_GLOBAL:
		ocode = ObjectToken::OCODE_PUSHGLOBALARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_MAP:
		ocode = ObjectToken::OCODE_PUSHMAPARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_WORLD:
		ocode = ObjectToken::OCODE_PUSHWORLDARRAY;
		goto sc_registerarray_case;
	}
}

void SourceVariable::makeObjectsGetArray(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	makeObjectsGetPrep(objects, &address, dimensions);
	makeObjectsGetArray(objects, dimensions->size(), position, _type, &address);
}
void SourceVariable::makeObjectsGetArray(ObjectVector * objects, int dimensions, SourcePosition const & position, VariableType const * type, int * address) const
{
	if (dimensions == 0)
	{
		makeObjectsGet(objects, position, type, address);
		return;
	}

	switch (_sc)
	{
	case SC_CONSTANT:
		throw SourceException("makeObjectsGetArray on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		throw SourceException("makeObjectsGetArray on register", position, "SourceVariable");

	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_ASMFUNC:
		case VT_BLOCK:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
		case VT_STRUCT:
		case VT_VOID:
			throw SourceException("makeObjectsGetArray on non-VT_ARRAY", position, "SourceVariable");

		case VT_ARRAY:
			makeObjectsGetArray(objects, dimensions-1, position, type->refType, address);
			break;
		}
		break;
	}
}

void SourceVariable::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	makeObjectsGetPrep(objects, &address, NULL);
	makeObjectsGetMember(objects, names, position, _type, &address);
}
void SourceVariable::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position, VariableType const * type, int * address) const
{
	if (names->empty())
	{
		makeObjectsGet(objects, position, type, address);
		return;
	}

	switch (_sc)
	{
	case SC_CONSTANT:
		throw SourceException("makeObjectGetMember on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_ARRAY:
		case VT_ASMFUNC:
		case VT_BLOCK:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
		case VT_VOID:
			throw SourceException("makeObjectGetMember on non-VT_STRUCT", position, "SourceVariable");

		case VT_STRUCT:
			for (size_t i(0); i < type->types.size(); ++i)
			{
				if (type->names[i] == names->back())
				{
					names->pop_back();
					makeObjectsGetMember(objects, names, position, type->types[i], address);
					return;
				}
				else
				{
					makeObjectsGetSkip(type->types[i], address);
				}
			}
			break;
		}
		break;
	}
}

void SourceVariable::makeObjectsGetPrep(ObjectVector * objects, int * address, std::vector<CounterPointer<SourceExpression> > * dimensions) const
{
	switch (_sc)
	{
	case SC_CONSTANT:
	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		*address = _address;
		break;

	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		makeObjectsSetPrep(objects, address, dimensions);
		*address = 0;
		break;
	}
}

void SourceVariable::makeObjectsGetSkip(VariableType const * type, int * address) const
{
	switch (type->type)
	{
	case VT_ACSFUNC:
	case VT_CHAR:
	case VT_INT:
	case VT_LNSPEC:
	case VT_NATIVE:
	case VT_REAL:
	case VT_SCRIPT:
	case VT_STRING:
		++*address;
		break;

	case VT_ARRAY:
	case VT_BLOCK:
	case VT_STRUCT:
		for (size_t i(0); i < type->types.size(); ++i)
			makeObjectsGetSkip(type->types[i], address);
		break;

	case VT_ASMFUNC:
	case VT_VOID:
		break;
	}
}



