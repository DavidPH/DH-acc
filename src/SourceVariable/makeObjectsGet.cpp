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

	ObjectExpression::Pointer addressBase;
	int address;
	makeObjectsGetPrep(objects, NULL, &addressBase, &address);
	makeObjectsGet(objects, position, _type, addressBase, &address, false);
}
void SourceVariable::makeObjectsGet(ObjectVector * objects, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address, bool dimensioned) const
{
	bool       array(false);
	ObjectCode ocode;

	switch (_sc)
	{
	case SC_AUTO:
		if (dimensioned)
		{
			array = true;
			ocode = OCODE_PUSHSTACKARRAY2;
		}
		else
			ocode = OCODE_PUSHSTACKVAR;
		goto sc_register_case;

	case SC_CONSTANT:
		ocode = OCODE_PUSHNUMBER;

		switch (type->type)
		{
		case VT_ASMFUNC:
		case VT_VOID:
			break;

		case VT_ARRAY:
		case VT_BLOCK:
		case VT_BOOLHARD:
		case VT_BOOLSOFT:
		case VT_CHAR:
		case VT_FUNCTION:
		case VT_INT:
		case VT_LINESPEC:
		case VT_NATIVE:
		case VT_POINTER:
		case VT_REAL:
		case VT_STRUCT:
		case VT_STRING:
		case VT_SCRIPT:
		case VT_UNION:
			objects->addToken(ocode, makeObject(position));
			++*address;
			break;
		}
		break;

	case SC_REGISTER:
		ocode = OCODE_PUSHSCRIPTVAR;
	sc_register_case:
		switch (type->type)
		{
		case VT_ARRAY:
		case VT_BLOCK:
		case VT_STRUCT:
			for (size_t i(0); i < type->types.size(); ++i)
				makeObjectsGet(objects, position, type->types[i], addressBase, address, dimensioned);
			break;

		case VT_ASMFUNC:
		case VT_VOID:
			break;

		case VT_BOOLHARD:
		case VT_BOOLSOFT:
		case VT_CHAR:
		case VT_FUNCTION:
		case VT_INT:
		case VT_LINESPEC:
		case VT_NATIVE:
		case VT_POINTER:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
			if (array)
				objects->addToken(ocode, objects->getValue(_nameObject), objects->getValueAdd(addressBase, (*address)++));
			else
				objects->addToken(ocode, objects->getValueAdd(addressBase, (*address)++));
			break;

		case VT_UNION:
			for (int i(0); i < type->size(); ++i)
			{
				if (array)
					objects->addToken(ocode, objects->getValue(_nameObject), objects->getValueAdd(addressBase, (*address)++));
				else
					objects->addToken(ocode, objects->getValueAdd(addressBase, (*address)++));
			}
			break;
		}
		break;

	case SC_REGISTER_GLOBAL:
		ocode = OCODE_PUSHGLOBALVAR;
		goto sc_register_case;

	case SC_REGISTER_MAP:
		ocode = OCODE_PUSHMAPVAR;
		goto sc_register_case;

	case SC_REGISTER_WORLD:
		ocode = OCODE_PUSHWORLDVAR;
		goto sc_register_case;

	case SC_REGISTERARRAY_GLOBAL:
		array = true;
		ocode = OCODE_PUSHGLOBALARRAY2;
		goto sc_register_case;

	case SC_REGISTERARRAY_MAP:
		array = true;
		ocode = OCODE_PUSHMAPARRAY2;
		goto sc_register_case;

	case SC_REGISTERARRAY_WORLD:
		array = true;
		ocode = OCODE_PUSHWORLDARRAY2;
		goto sc_register_case;

	case SC_STATIC:
		if (dimensioned)
		{
			array = true;
			ocode = OCODE_PUSHSTATICARRAY2;
		}
		else
			ocode = OCODE_PUSHSTATICVAR;
		goto sc_register_case;
	}
}

void SourceVariable::makeObjectsGetArray(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, SourcePosition const & position) const
{
	objects->setPosition(position);

	ObjectExpression::Pointer addressBase;
	int address;
	makeObjectsGetPrep(objects, dimensions, &addressBase, &address);
	makeObjectsGetArray(objects, dimensions->size(), position, _type, addressBase, &address);
}
void SourceVariable::makeObjectsGetArray(ObjectVector * objects, int dimensions, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const
{
	if (dimensions == 0)
	{
		makeObjectsGet(objects, position, type, addressBase, address, true);
		return;
	}

	switch (_sc)
	{
	case SC_AUTO:
	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
	case SC_STATIC:
		switch (type->type)
		{
		case VT_ARRAY:
			makeObjectsGetArray(objects, dimensions-1, position, type->refType, addressBase, address);
			break;

		case VT_ASMFUNC:
		case VT_BLOCK:
		case VT_BOOLHARD:
		case VT_BOOLSOFT:
		case VT_CHAR:
		case VT_FUNCTION:
		case VT_INT:
		case VT_LINESPEC:
		case VT_NATIVE:
		case VT_POINTER:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
		case VT_STRUCT:
		case VT_UNION:
		case VT_VOID:
			throw SourceException("makeObjectsGetArray on non-VT_ARRAY", position, "SourceVariable");
		}
		break;

	case SC_CONSTANT:
		throw SourceException("makeObjectsGetArray on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		throw SourceException("makeObjectsGetArray on register", position, "SourceVariable");
	}
}

void SourceVariable::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const
{
	objects->setPosition(position);

	ObjectExpression::Pointer addressBase;
	int address;
	makeObjectsGetPrep(objects, NULL, &addressBase, &address);
	makeObjectsGetMember(objects, names, position, _type, addressBase, &address);
}
void SourceVariable::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const
{
	if (names->empty())
	{
		makeObjectsGet(objects, position, type, addressBase, address, false);
		return;
	}

	switch (_sc)
	{
	case SC_AUTO:
	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
	case SC_STATIC:
		switch (type->type)
		{
		case VT_ARRAY:
		case VT_ASMFUNC:
		case VT_BLOCK:
		case VT_BOOLHARD:
		case VT_BOOLSOFT:
		case VT_CHAR:
		case VT_FUNCTION:
		case VT_INT:
		case VT_LINESPEC:
		case VT_NATIVE:
		case VT_POINTER:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
		case VT_VOID:
			throw SourceException("makeObjectGetMember on non-VT_STRUCT", position, "SourceVariable");

		case VT_STRUCT:
		case VT_UNION:
			for (size_t i(0); i < type->types.size(); ++i)
			{
				if (type->names[i] == names->back())
				{
					names->pop_back();
					makeObjectsGetMember(objects, names, position, type->types[i], addressBase, address);
					return;
				}
				else if (type->type == VT_STRUCT)
				{
					makeObjectsGetSkip(type->types[i], address);
				}
			}
			break;
		}
		break;

	case SC_CONSTANT:
		throw SourceException("makeObjectGetMember on SC_CONSTANT", position, "SourceVariable");
	}
}

void SourceVariable::makeObjectsGetPrep(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, ObjectExpression::Pointer * addressBase, int * address) const
{
	switch (_sc)
	{
	case SC_AUTO:
	case SC_STATIC:
		makeObjectsSetPrep(objects, dimensions, addressBase, address);
		*address = 0;
		break;

	case SC_CONSTANT:
	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		*addressBase = objects->getValue(_nameObject);
		*address = 0;
		break;

	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		makeObjectsSetPrep(objects, dimensions, addressBase, address);
		*addressBase = objects->getValue(0);
		*address = 0;
		break;
	}
}

void SourceVariable::makeObjectsGetSkip(VariableType const * type, int * address) const
{
	*address += type->size();
}



