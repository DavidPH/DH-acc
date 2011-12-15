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

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../SourceExpression.hpp"
#include "../VariableType.hpp"



void SourceVariable::makeObjectsSet(ObjectVector * objects, SourcePosition const & position) const
{
	objects->setPosition(position);

	ObjectExpression::Pointer addressBase;
	int address;
	makeObjectsSetPrep(objects, NULL, &addressBase, &address, position);
	makeObjectsSet(objects, position, _type, addressBase, &address, false);
}
void SourceVariable::makeObjectsSet(ObjectVector * objects, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address, bool dimensioned) const
{
	bool       array(false);
	ObjectCode ocode;

	switch (_sc)
	{
	case SC_AUTO:
		if (dimensioned)
			throw SourceException("dimensioned SC_AUTO", position, "SourceVariable");

		ocode = OCODE_SET_AUTO_VAR32I;
		goto sc_register_case;

	case SC_CONSTANT:
		throw SourceException("makeObjectsSet on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
		ocode = OCODE_SET_REGISTER_VAR32I;
	sc_register_case:
		switch (type->vt)
		{
		case VariableType::VT_ARRAY:
		case VariableType::VT_BLOCK:
		case VariableType::VT_STRUCT:
			for (size_t i(type->types.size()); i--;)
				makeObjectsSet(objects, position, type->types[i], addressBase, address, dimensioned);
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
				objects->addToken(ocode, objects->getValue(_nameObject), objects->getValueAdd(addressBase, (*address)--));
			else
				objects->addToken(ocode, objects->getValueAdd(addressBase, (*address)--));
			break;

		case VariableType::VT_UNION:
			for (int i(type->size(position)); i--;)
			{
				if (array)
					objects->addToken(ocode, objects->getValue(_nameObject), objects->getValueAdd(addressBase, (*address)--));
				else
					objects->addToken(ocode, objects->getValueAdd(addressBase, (*address)--));
			}
			break;
		}
		break;

	case SC_REGISTER_GLOBAL:
		ocode = OCODE_SET_GLOBALREGISTER_VAR32I;
		goto sc_register_case;

	case SC_REGISTER_MAP:
		ocode = OCODE_SET_MAPREGISTER_VAR32I;
		goto sc_register_case;

	case SC_REGISTER_WORLD:
		ocode = OCODE_SET_WORLDREGISTER_VAR32I;
		goto sc_register_case;

	case SC_REGISTERARRAY_GLOBAL:
		array = true;
		ocode = OCODE_SET_GLOBALARRAYTEMP_VAR32I;
		goto sc_register_case;

	case SC_REGISTERARRAY_MAP:
		array = true;
		ocode = OCODE_SET_MAPARRAYTEMP_VAR32I;
		goto sc_register_case;

	case SC_REGISTERARRAY_WORLD:
		array = true;
		ocode = OCODE_SET_WORLDARRAYTEMP_VAR32I;
		goto sc_register_case;

	case SC_STATIC:
		if (dimensioned)
			throw SourceException("dimensioned SC_STATIC", position, "SourceVariable");

		ocode = OCODE_SET_STATIC_VAR32I;
		goto sc_register_case;
	}
}

void SourceVariable::makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions, SourcePosition const & position) const
{
	objects->setPosition(position);

	ObjectExpression::Pointer addressBase;
	int address;
	makeObjectsSetPrep(objects, dimensions, &addressBase, &address, position);
	makeObjectsSetArray(objects, dimensions->size(), position, _type, addressBase, &address);
}
void SourceVariable::makeObjectsSetArray(ObjectVector * objects, size_t dimensions, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const
{
	if (dimensions == 0)
	{
		makeObjectsSet(objects, position, type, addressBase, address, true);
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
			makeObjectsSetArray(objects, dimensions-1, position, type->refType, addressBase, address);
			break;

		default:
			throw SourceException("makeObjectsSetArray on non-VT_ARRAY", position, "SourceVariable");
		}
		break;

	case SC_CONSTANT:
		throw SourceException("makeObjectsSetArray on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		throw SourceException("makeObjectsSetArray on register", position, "SourceVariable");
	}
}

void SourceVariable::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const
{
	objects->setPosition(position);

	ObjectExpression::Pointer addressBase;
	int address;
	makeObjectsSetPrep(objects, NULL, &addressBase, &address, position);
	makeObjectsSetMember(objects, names, position, _type, addressBase, &address);
}
void SourceVariable::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const
{
	if (names->empty())
	{
		makeObjectsSet(objects, position, type, addressBase, address, false);
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
			for (size_t i(type->types.size()); i--;)
			{
				if (type->names[i] == names->back())
				{
					names->pop_back();
					makeObjectsSetMember(objects, names, position, type->types[i], addressBase, address);
					return;
				}
				else if (type->vt == VariableType::VT_STRUCT)
				{
					makeObjectsSetSkip(type->types[i], address);
				}
			}
			break;

		default:
			throw SourceException("attempt to set member from non-struct", position, "SourceVariable");
		}
		break;

	case SC_CONSTANT:
		throw SourceException("attempt to set SC_CONSTANT", position, "SourceVariable");
	}
}

void SourceVariable::makeObjectsSetPrep(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions, ObjectExpression::Pointer * addressBase, int * address, SourcePosition const & position) const
{
	makeObjectsAccessPrep(objects, dimensions, addressBase, address, position);
}

void SourceVariable::makeObjectsSetSkip(VariableType const * type, int * address) const
{
	*address -= type->size(_position);
}


