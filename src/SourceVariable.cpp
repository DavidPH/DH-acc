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



SourceVariable::SourceVariable() : _address(-1), _nameObject(), _nameSource(), _position(), _sc(), _type()
{

}
SourceVariable::SourceVariable(std::string const & nameObject, std::string const & nameSource, int const address, StorageClass const sc, VariableType const type, SourcePosition const & position) : _address(address), _nameObject(nameObject), _nameSource(nameSource), _position(position), _sc(sc), _type(type)
{

}

SourceVariable::StorageClass SourceVariable::get_StorageClass(SourceTokenC const & token)
{
	if (token.getData() == "register")
		return SC_REGISTER;

	throw SourceException("invalid storage-class", token.getPosition(), "SourceVariable");
}

SourceVariable::VariableType SourceVariable::get_VariableType(SourceTokenC const & token)
{
	if (token.getData() == "fixed")
		return VT_FIXED;

	if (token.getData() == "int")
		return VT_INT;

	throw SourceException("invalid variable-type", token.getPosition(), "SourceVariable");
}

std::string const & SourceVariable::getNameSource() const
{
	return _nameSource;
}

SourceVariable::VariableType SourceVariable::getType() const
{
	return _type;
}

void SourceVariable::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	switch (_sc)
	{
	case SC_REGISTER:
		switch (_type)
		{
		case VT_FIXED:
		case VT_INT:
			objects->push_back(ObjectToken(ObjectToken::OCODE_PUSHSCRIPTVAR, _position, ObjectExpression::create_value_int32(_address, _position)));
			break;
		}
		break;
	}
}

void SourceVariable::makeObjectsSet(std::vector<ObjectToken> * const objects) const
{
	switch (_sc)
	{
	case SC_REGISTER:
		switch (_type)
		{
		case VT_FIXED:
		case VT_INT:
			objects->push_back(ObjectToken(ObjectToken::OCODE_DUP, _position));
			objects->push_back(ObjectToken(ObjectToken::OCODE_ASSIGNSCRIPTVAR, _position, ObjectExpression::create_value_int32(_address, _position)));
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
void print_debug(std::ostream * const out, SourceVariable::VariableType const in)
{
	switch (in)
	{
	case SourceVariable::VT_FIXED: *out << "VT_FIXED"; break;
	case SourceVariable::VT_INT:   *out << "VT_INT";   break;
	}
}



