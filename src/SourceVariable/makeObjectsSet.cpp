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

/* SourceVariable/makeObjectsSet.cpp
**
** Defines the SourceVariable::makeObjectsSet* methods.
*/

#include "../SourceVariable.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



void SourceVariable::makeObjectsSet(ObjectVector * objects, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	makeObjectsSetPrep(objects, &address, 0);
	makeObjectsSet(objects, position, _type, &address);
	makeObjectsSetPost(objects, 0);
	makeObjectsGet(objects, position);
}
void SourceVariable::makeObjectsSet(ObjectVector * objects, SourcePosition const & position, VariableType const * const type, int * const address) const
{
	ObjectToken::ObjectCode ocode;
	ObjectToken::ObjectCode opush;

	switch (_sc)
	{
	case SC_CONSTANT:
		throw SourceException("makeObjectsSet on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
		ocode = ObjectToken::OCODE_ASSIGNSCRIPTVAR;
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
			objects->addToken(ocode, objects->getValue((*address)--));
			break;

		case VT_ARRAY:
			throw SourceException("makeObjectsSet on register VT_ARRAY", position, "SourceVariable");

		case VT_ASMFUNC:
		case VT_VOID:
			break;

		case VT_STRUCT:
			for (size_t i(type->types.size()); i--;)
				makeObjectsSet(objects, position, type->types[i], address);
			break;
		}
		break;

	case SC_REGISTER_GLOBAL:
		ocode = ObjectToken::OCODE_ASSIGNGLOBALVAR;
		goto sc_register_case;

	case SC_REGISTER_MAP:
		ocode = ObjectToken::OCODE_ASSIGNMAPVAR;
		goto sc_register_case;

	case SC_REGISTER_WORLD:
		ocode = ObjectToken::OCODE_ASSIGNWORLDVAR;
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
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue((*address)--));
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(0));
			objects->addToken(opush, objects->getValue(_address));
			objects->addToken(ObjectToken::OCODE_ADD);
			objects->addToken(ObjectToken::OCODE_SWAP);
			objects->addToken(ocode, objects->getValue(_address));
			break;

		case VT_ARRAY:
		case VT_STRUCT:
			for (size_t i(type->types.size()); i--;)
				makeObjectsSet(objects, position, type->types[i], address);
			break;

		case VT_ASMFUNC:
		case VT_VOID:
			break;
		}
		break;

	case SC_REGISTERARRAY_GLOBAL:
		ocode = ObjectToken::OCODE_ASSIGNGLOBALARRAY;
		opush = ObjectToken::OCODE_PUSHGLOBALARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_MAP:
		ocode = ObjectToken::OCODE_ASSIGNMAPARRAY;
		opush = ObjectToken::OCODE_PUSHMAPARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_WORLD:
		ocode = ObjectToken::OCODE_ASSIGNWORLDARRAY;
		opush = ObjectToken::OCODE_PUSHWORLDARRAY;
		goto sc_registerarray_case;
	}
}

void SourceVariable::makeObjectsSetArray(ObjectVector * objects, int dimensions, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	makeObjectsSetPrep(objects, &address, dimensions);
	makeObjectsSetArray(objects, dimensions, position, _type, &address);
	makeObjectsSetPost(objects, dimensions);
	makeObjectsGetArray(objects, dimensions, position);
}
void SourceVariable::makeObjectsSetArray(ObjectVector * objects, int dimensions, SourcePosition const & position, VariableType const * const type, int * const address) const
{
	if (dimensions == 0)
	{
		makeObjectsSet(objects, position, type, address);
		return;
	}

	switch (_sc)
	{
	case SC_CONSTANT:
		throw SourceException("makeObjectsSetArray on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		throw SourceException("makeObjectsSetArray on register", position, "SourceVariable");

	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_ASMFUNC:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
		case VT_STRUCT:
		case VT_VOID:
			throw SourceException("makeObjectsSetArray on non-VT_ARRAY", position, "SourceVariable");

		case VT_ARRAY:
			makeObjectsSetArray(objects, dimensions-1, position, type->refType, address);
			break;
		}
		break;
	}
}

void SourceVariable::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	std::vector<std::string> namesOriginal(*names);
	makeObjectsSetPrep(objects, &address, 0);
	makeObjectsSetMember(objects, names, position, _type, &address);
	makeObjectsSetPost(objects, 0);
	makeObjectsGetMember(objects, &namesOriginal, position);
}
void SourceVariable::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position, VariableType const * const type, int * const address) const
{
	if (names->empty())
	{
		makeObjectsSet(objects, position, type, address);
		return;
	}

	switch (_sc)
	{
	case SC_CONSTANT:
		throw SourceException("attempt to set SC_CONSTANT", position, "SourceVariable");

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
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
		case VT_VOID:
			throw SourceException("attempt to set member from non-struct", position, "SourceVariable");

		case VT_STRUCT:
			for (size_t i(0); i < type->types.size(); ++i)
			{
				if (type->names[i] == names->back())
				{
					names->pop_back();
					makeObjectsSetMember(objects, names, position, type->types[i], address);
					return;
				}
				else
				{
					makeObjectsSetSkip(type->types[i], address);
				}
			}
			break;
		}
		break;
	}
}

void SourceVariable::makeObjectsSetPost(ObjectVector * objects, int dimensions) const
{
	ObjectToken::ObjectCode ocode;

	switch (_sc)
	{
	case SC_CONSTANT:
	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		break;

	sc_registerarray_case:
	{
		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(0));
		objects->addToken(ocode, objects->getValue(_address));
		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(1));
		objects->addToken(ObjectToken::OCODE_SUB);

		VariableType const * type = _type;

		for (int i(dimensions); i--;) type = type->refType;
		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(type->size()));
		objects->addToken(ObjectToken::OCODE_DIV);

		if (dimensions) for (int i(dimensions); --i;) objects->addTokenPushZero();
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

void SourceVariable::makeObjectsSetPrep(ObjectVector * objects, int * address, int dimensions) const
{
	ObjectToken::ObjectCode ocode;

	switch (_sc)
	{
	case SC_CONSTANT:
	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		*address = _address + _type->size() - 1;
		break;

	sc_registerarray_case:
	{
		VariableType const * type = _type;
		if (!dimensions) objects->addTokenPushZero();
		else
		{
			objects->addTokenPushZero();
			objects->addToken(ObjectToken::OCODE_SWAP);
		}

		for (int i(dimensions); i--;)
		{
			type = type->refType;
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(type->size()));
			objects->addToken(ObjectToken::OCODE_MUL);
			objects->addToken(ObjectToken::OCODE_ADD);
			if (i) objects->addToken(ObjectToken::OCODE_SWAP);
		}

		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(1));
		objects->addToken(ObjectToken::OCODE_ADD);

		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(0));
		objects->addToken(ObjectToken::OCODE_SWAP);

		objects->addToken(ocode, objects->getValue(_address));

		*address = type->size() - 1;
	}
		break;

	case SC_REGISTERARRAY_GLOBAL:
		ocode = ObjectToken::OCODE_ASSIGNGLOBALARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_MAP:
		ocode = ObjectToken::OCODE_ASSIGNMAPARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_WORLD:
		ocode = ObjectToken::OCODE_ASSIGNWORLDARRAY;
		goto sc_registerarray_case;
	}
}

void SourceVariable::makeObjectsSetSkip(VariableType const * const type, int * const address) const
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
		--*address;
		break;

	case VT_ARRAY:
	case VT_STRUCT:
		for (size_t i(type->types.size()); i--;)
			makeObjectsSetSkip(type->types[i], address);
		break;

	case VT_ASMFUNC:
	case VT_VOID:
		break;
	}
}



