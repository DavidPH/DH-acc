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

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



void SourceVariable::makeObjectsGet(ObjectVector * objects, SourcePosition const & position) const
{
	objects->setPosition(position);

	ObjectExpression::Pointer addressBase;
	int address;
	makeObjectsGetPrep(objects, NULL, &addressBase, &address, position);
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
			throw SourceException("dimensioned SC_AUTO", position, "SourceVariable");

		ocode = OCODE_GET_AUTO_VAR32I;
		goto sc_register_case;

	case SC_CONSTANT:
		ocode = OCODE_GET_LITERAL32I;

		switch (type->vt)
		{
		case VariableType::VT_ASMFUNC:
		case VariableType::VT_VOID:
			break;

		case VariableType::VT_ARRAY:
		case VariableType::VT_BLOCK:
		case VariableType::VT_BOOLHARD:
		case VariableType::VT_BOOLSOFT:
		case VariableType::VT_CHAR:
		case VariableType::VT_ENUM:
		case VariableType::VT_FUNCTION:
		case VariableType::VT_INT:
		case VariableType::VT_LABEL:
		case VariableType::VT_LINESPEC:
		case VariableType::VT_NATIVE:
		case VariableType::VT_POINTER:
		case VariableType::VT_REAL:
		case VariableType::VT_STRUCT:
		case VariableType::VT_STRING:
		case VariableType::VT_SCRIPT:
		case VariableType::VT_UNION:
			objects->addToken(ocode, makeObject(position));
			++*address;
			break;
		}
		break;

	case SC_REGISTER:
		ocode = OCODE_GET_REGISTER_VAR32I;
	sc_register_case:
		switch (type->vt)
		{
		case VariableType::VT_ARRAY:
		case VariableType::VT_BLOCK:
		case VariableType::VT_STRUCT:
			for (size_t i(0); i < type->types.size(); ++i)
				makeObjectsGet(objects, position, type->types[i], addressBase, address, dimensioned);
			break;

		case VariableType::VT_ASMFUNC:
		case VariableType::VT_VOID:
			break;

		case VariableType::VT_BOOLHARD:
		case VariableType::VT_BOOLSOFT:
		case VariableType::VT_CHAR:
		case VariableType::VT_ENUM:
		case VariableType::VT_FUNCTION:
		case VariableType::VT_INT:
		case VariableType::VT_LABEL:
		case VariableType::VT_LINESPEC:
		case VariableType::VT_NATIVE:
		case VariableType::VT_POINTER:
		case VariableType::VT_REAL:
		case VariableType::VT_SCRIPT:
		case VariableType::VT_STRING:
			if (array)
				objects->addToken(ocode, objects->getValue(_nameObject), objects->getValueAdd(addressBase, (*address)++));
			else
				objects->addToken(ocode, objects->getValueAdd(addressBase, (*address)++));
			break;

		case VariableType::VT_UNION:
			for (int i(0); i < type->size(position); ++i)
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
		ocode = OCODE_GET_GLOBALREGISTER_VAR32I;
		goto sc_register_case;

	case SC_REGISTER_MAP:
		ocode = OCODE_GET_MAPREGISTER_VAR32I;
		goto sc_register_case;

	case SC_REGISTER_WORLD:
		ocode = OCODE_GET_WORLDREGISTER_VAR32I;
		goto sc_register_case;

	case SC_REGISTERARRAY_GLOBAL:
		array = true;
		ocode = OCODE_GET_GLOBALARRAYTEMP_VAR32I;
		goto sc_register_case;

	case SC_REGISTERARRAY_MAP:
		array = true;
		ocode = OCODE_GET_MAPARRAYTEMP_VAR32I;
		goto sc_register_case;

	case SC_REGISTERARRAY_WORLD:
		array = true;
		ocode = OCODE_GET_WORLDARRAYTEMP_VAR32I;
		goto sc_register_case;

	case SC_STATIC:
		if (dimensioned)
			throw SourceException("dimensioned SC_STATIC", position, "SourceVariable");

		ocode = OCODE_GET_STATIC_VAR32I;
		goto sc_register_case;
	}
}

void SourceVariable::makeObjectsGetArray(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, SourcePosition const & position) const
{
	objects->setPosition(position);

	ObjectExpression::Pointer addressBase;
	int address;
	makeObjectsGetPrep(objects, dimensions, &addressBase, &address, position);
	makeObjectsGetArray(objects, dimensions->size(), position, _type, addressBase, &address);
}
void SourceVariable::makeObjectsGetArray(ObjectVector * objects, size_t dimensions, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const
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
		switch (type->vt)
		{
		case VariableType::VT_ARRAY:
			makeObjectsGetArray(objects, dimensions-1, position, type->refType, addressBase, address);
			break;

		default:
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
	makeObjectsGetPrep(objects, NULL, &addressBase, &address, position);
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
		switch (type->vt)
		{
		case VariableType::VT_STRUCT:
		case VariableType::VT_UNION:
			for (size_t i(0); i < type->types.size(); ++i)
			{
				if (type->names[i] == names->back())
				{
					names->pop_back();
					makeObjectsGetMember(objects, names, position, type->types[i], addressBase, address);
					return;
				}
				else if (type->vt == VariableType::VT_STRUCT)
				{
					makeObjectsGetSkip(type->types[i], address);
				}
			}
			break;

		default:
			throw SourceException("makeObjectGetMember on non-VT_STRUCT", position, "SourceVariable");
		}
		break;


	case SC_CONSTANT:
		throw SourceException("makeObjectGetMember on SC_CONSTANT", position, "SourceVariable");
	}
}

void SourceVariable::makeObjectsGetPrep(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, ObjectExpression::Pointer * addressBase, int * address, SourcePosition const & position) const
{
	switch (_sc)
	{
	case SC_AUTO:
	case SC_STATIC:
		makeObjectsAccessPrep(objects, dimensions, addressBase, address, position);
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
		makeObjectsAccessPrep(objects, dimensions, addressBase, address, position);
		*addressBase = objects->getValue(0);
		*address = 0;
		break;
	}
}

void SourceVariable::makeObjectsGetSkip(VariableType const * type, int * address) const
{
	*address += type->size(_position);
}


