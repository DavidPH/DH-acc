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
	makeObjectsSetPrep(objects, NULL, &addressBase, &address);
	makeObjectsSet(objects, position, _type, addressBase, &address, false);
	makeObjectsGet(objects, position);
}
void SourceVariable::makeObjectsSet(ObjectVector * objects, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address, bool dimensioned) const
{
	bool       array(false);
	ObjectCode ocode;

	switch (_sc)
	{
	case SC_AUTO:
		if (dimensioned)
		{
			array = true;
			ocode = OCODE_ASSIGNSTACKARRAY2;
		}
		else
			ocode = OCODE_ASSIGNSTACKVAR;
		goto sc_register_case;

	case SC_CONSTANT:
		throw SourceException("makeObjectsSet on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
		ocode = OCODE_ASSIGNSCRIPTVAR;
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
		case VariableType::VT_FUNCTION:
		case VariableType::VT_INT:
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
			for (int i(0); i < type->size(); ++i)
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
		ocode = OCODE_ASSIGNGLOBALVAR;
		goto sc_register_case;

	case SC_REGISTER_MAP:
		ocode = OCODE_ASSIGNMAPVAR;
		goto sc_register_case;

	case SC_REGISTER_WORLD:
		ocode = OCODE_ASSIGNWORLDVAR;
		goto sc_register_case;

	case SC_REGISTERARRAY_GLOBAL:
		array = true;
		ocode = OCODE_ASSIGNGLOBALARRAY2;
		goto sc_register_case;

	case SC_REGISTERARRAY_MAP:
		array = true;
		ocode = OCODE_ASSIGNMAPARRAY2;
		goto sc_register_case;

	case SC_REGISTERARRAY_WORLD:
		array = true;
		ocode = OCODE_ASSIGNWORLDARRAY2;
		goto sc_register_case;

	case SC_STATIC:
		if (dimensioned)
		{
			array = true;
			ocode = OCODE_ASSIGNSTATICARRAY2;
		}
		else
			ocode = OCODE_ASSIGNSTATICVAR;
		goto sc_register_case;
	}
}

void SourceVariable::makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions, SourcePosition const & position) const
{
	objects->setPosition(position);

	ObjectExpression::Pointer addressBase;
	int address;
	makeObjectsSetPrep(objects, dimensions, &addressBase, &address);
	makeObjectsSetArray(objects, dimensions->size(), position, _type, addressBase, &address);
	makeObjectsGetArray(objects, dimensions, position);
}
void SourceVariable::makeObjectsSetArray(ObjectVector * objects, int dimensions, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const
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
	std::vector<std::string> namesOriginal(*names);
	makeObjectsSetPrep(objects, NULL, &addressBase, &address);
	makeObjectsSetMember(objects, names, position, _type, addressBase, &address);
	makeObjectsGetMember(objects, &namesOriginal, position);
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
			for (size_t i(0); i < type->types.size(); ++i)
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

void SourceVariable::makeObjectsSetPrep(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions, ObjectExpression::Pointer * addressBase, int * address) const
{
	switch (_sc)
	{
	case SC_AUTO:
	case SC_STATIC:
	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		if (dimensions)
		{
			VariableType const * type = _type;

			if ((*dimensions)[0])
			{
				for (size_t i(dimensions->size()); i--;)
				{
					type = type->refType;

					(*dimensions)[i]->makeObjectsGet(objects);
					objects->addToken(OCODE_PUSHNUMBER, objects->getValue(type->size()));
					objects->addToken(OCODE_MUL);

					if (i != (dimensions->size() - 1)) objects->addToken(OCODE_ADD);

					(*dimensions)[i] = NULL;
				}

				objects->addToken(OCODE_ASSIGNWORLDVAR, objects->getValue(1));
			}
			else
			{
				// We've already calculated the index, don't
				// need to do anything but reset address.
				for (size_t i(0); i < dimensions->size(); ++i)
					type = type->refType;
			}

			*addressBase = objects->getValue(0);
			*address = type->size() - 1;
		}
		else if (_sc == SC_AUTO)
		{
			*addressBase = objects->getValue(_nameObject);
			*address = _type->size() - 1;
		}
		else
		{
			objects->addToken(OCODE_PUSHNUMBER, objects->getValue(0));

			objects->addToken(OCODE_ASSIGNWORLDVAR, objects->getValue(1));

			*addressBase = objects->getValue(0);
			*address = _type->size() - 1;
		}

		break;

	case SC_CONSTANT:
	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		*addressBase = objects->getValue(_nameObject);
		*address = _type->size() - 1;
		break;
	}
}

void SourceVariable::makeObjectsSetSkip(VariableType const * type, int * address) const
{
	*address -= type->size();
}



