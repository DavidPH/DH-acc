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

/* SourceVariable.hpp
**
** Defines the SourceVariable methods.
*/

#include "SourceVariable.hpp"

#include "ObjectExpression.hpp"
#include "ObjectToken.hpp"
#include "print_debug.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"



std::vector<std::string>                    SourceVariable::_names;
std::vector<SourceVariable::VariableType *> SourceVariable::_types;



static struct _SourceVariable_init_c
{
	_SourceVariable_init_c()
	{
		SourceVariable::init();
	}
} _SourceVarible_init;



int SourceVariable::VariableType::size() const
{
	switch (type)
	{
	case VT_FIXED:
	case VT_INT:
	case VT_STRING:
		return 1;

	case VT_VOID:
		return 0;

	case VT_STRUCT:
	{
		int s(0);

		for (size_t i(0); i < types.size(); ++i)
			s += types[i]->size();

		return s;
	}
	}

	return 0;
}



SourceVariable::SourceVariable() : _address(-1), _nameObject(), _nameSource(), _position(), _sc(), _type()
{

}
SourceVariable::SourceVariable(std::string const & nameObject, std::string const & nameSource, int const address, StorageClass const sc, VariableType const * const type, SourcePosition const & position) : _address(address), _nameObject(nameObject), _nameSource(nameSource), _position(position), _sc(sc), _type(type)
{

}

void SourceVariable::add_struct(std::string const & name, std::vector<std::string> const & names, std::vector<VariableType const *> const & types)
{
	VariableType * type(new VariableType);

	type->type = VT_STRUCT;
	type->names = names;
	type->types = types;

	_names.push_back(name);
	_types.push_back(type);
}

SourceVariable::StorageClass SourceVariable::get_StorageClass(SourceTokenC const & token)
{
	if (token.getData() == "register")
		return SC_REGISTER;

	throw SourceException("invalid storage-class", token.getPosition(), "SourceVariable");
}

SourceVariable::VariableType const * SourceVariable::get_VariableType(SourceTokenC const & token)
{
	VariableType const * type(get_VariableType_null(token.getData()));

	if (!type) throw SourceException("invalid variable-type", token.getPosition(), "SourceVariable");

	return type;
}
SourceVariable::VariableType const * SourceVariable::get_VariableType(VariableTypeInternal const type)
{
	return _types[(type <= VT_VOID) ? type : VT_VOID];
}

SourceVariable::VariableType const * SourceVariable::get_VariableType_null(std::string const & name)
{
	for (size_t i(0); i < _names.size(); ++i)
		if (name == _names[i])
			return _types[i];

	return NULL;
}

std::string const & SourceVariable::getNameSource() const
{
	return _nameSource;
}

SourceVariable::VariableType const * SourceVariable::getType() const
{
	return _type;
}

void SourceVariable::init()
{
	_names.resize(VT_VOID+1);
	_types.resize(VT_VOID+1);
	VariableType type;

	type.type = VT_FIXED;
	_names[VT_FIXED] = "fixed";
	_types[VT_FIXED] = new VariableType(type);

	type.type = VT_INT;
	_names[VT_INT] = "int";
	_types[VT_INT] = new VariableType(type);

	type.type = VT_STRING;
	_names[VT_STRING] = "string";
	_types[VT_STRING] = new VariableType(type);

	type.type = VT_VOID;
	_names[VT_VOID] = "void";
	_types[VT_VOID] = new VariableType(type);
}

void SourceVariable::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	int address(_address);
	makeObjectsGet(objects, _type, &address);
}
void SourceVariable::makeObjectsGet(std::vector<ObjectToken> * const objects, VariableType const * const type, int * const address) const
{
	switch (_sc)
	{
	case SC_REGISTER:
		switch (type->type)
		{
		case VT_FIXED:
		case VT_INT:
		case VT_STRING:
			objects->push_back(ObjectToken(ObjectToken::OCODE_PUSHSCRIPTVAR, _position, ObjectExpression::create_value_int32((*address)++, _position)));
			break;

		case VT_VOID:
			break;

		case VT_STRUCT:
			for (size_t i(0); i < type->types.size(); ++i)
				makeObjectsGet(objects, type->types[i], address);
			break;
		}
		break;
	}
}

void SourceVariable::makeObjectsSet(std::vector<ObjectToken> * const objects) const
{
	int address(_address + _type->size() - 1);
	makeObjectsSet(objects, _type, &address);
	makeObjectsGet(objects);
}
void SourceVariable::makeObjectsSet(std::vector<ObjectToken> * const objects, VariableType const * const type, int * const address) const
{
	switch (_sc)
	{
	case SC_REGISTER:
		switch (type->type)
		{
		case VT_FIXED:
		case VT_INT:
		case VT_STRING:
			objects->push_back(ObjectToken(ObjectToken::OCODE_ASSIGNSCRIPTVAR, _position, ObjectExpression::create_value_int32((*address)--, _position)));
			break;

		case VT_VOID:
			break;

		case VT_STRUCT:
			for (size_t i(type->types.size()); i--;)
				makeObjectsSet(objects, type->types[i], address);
			break;
		}
		break;
	}
}



void print_debug(std::ostream * const out, SourceVariable const & in)
{
	*out << "SourceVariable(";
		*out << "address=(";
		*out << in._address;
		*out << ")";

		*out << ", ";

		*out << "nameObject=(";
		print_debug(out, in._nameObject);
		*out << ")";

		*out << ", ";

		*out << "nameSource=(";
		print_debug(out, in._nameSource);
		*out << ")";

		*out << ", ";

		*out << "position=(";
		print_debug(out, in._position);
		*out << ")";

		*out << ", ";

		*out << "sc=(";
		print_debug(out, in._sc);
		*out << ")";

		*out << ", ";

		*out << "type=(";
		print_debug(out, in._type);
		*out << ")";
	*out << ")";
}
void print_debug(std::ostream * const out, SourceVariable::StorageClass const in)
{
	switch (in)
	{
	case SourceVariable::SC_REGISTER: *out << "SC_REGISTER"; break;
	}
}
void print_debug(std::ostream * const out, SourceVariable::VariableType const & in)
{
	*out << "SourceVariable::VariableType(";
		*out << "type=(";
		print_debug(out, in.type);
		*out << ")";

		*out << ", ";

		*out << "names=(";
		print_debug(out, in.names);
		*out << ")";

		*out << ", ";

		*out << "types=(";
		print_debug(out, in.types);
		*out << ")";
	*out << ")";
}
void print_debug(std::ostream * const out, SourceVariable::VariableTypeInternal const in)
{
	switch (in)
	{
	case SourceVariable::VT_FIXED:  *out << "VT_FIXED";  break;
	case SourceVariable::VT_INT:    *out << "VT_INT";    break;
	case SourceVariable::VT_STRING: *out << "VT_STRING"; break;
	case SourceVariable::VT_VOID:   *out << "VT_VOID";   break;
	case SourceVariable::VT_STRUCT: *out << "VT_STRUCT"; break;
	}
}



