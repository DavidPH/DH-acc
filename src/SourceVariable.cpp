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
#include "SourceException.hpp"
#include "SourceTokenC.hpp"



SourceVariable::SourceVariable(SourceVariable const & var) : _expr(var._expr), _nameObject(var._nameObject), _nameSource(var._nameSource), _position(var._position), _sc(var._sc), _type(var._type)
{

}
SourceVariable::SourceVariable(std::string const & nameSource, VariableType const * type, ObjectExpression * expr, SourcePosition const & position) : _expr(expr), _nameSource(nameSource), _position(position), _sc(SC_CONSTANT), _type(type)
{

}
SourceVariable::SourceVariable(std::string const & nameSource, VariableType const * type, std::string const & nameObject, SourcePosition const & position) : _nameObject(nameObject), _nameSource(nameSource), _position(position), _sc(SC_CONSTANT), _type(type)
{

}
SourceVariable::SourceVariable(std::string const & nameSource, VariableType const * type, std::string const & nameObject, StorageClass sc, SourcePosition const & position) : _nameObject(nameObject), _nameSource(nameSource), _position(position), _sc(sc), _type(type)
{

}
SourceVariable::~SourceVariable()
{

}

bool SourceVariable::canMakeObject() const
{
	return _sc == SC_CONSTANT;
}
bool SourceVariable::canMakeObjectAddress() const
{
	return _sc == SC_STATIC;
}

bool SourceVariable::canMakeObjectsAddress() const
{
	return _sc == SC_AUTO || _sc == SC_STATIC;
}

SourceVariable::Pointer SourceVariable::create_constant(std::string const & nameSource, VariableType const * type, ObjectExpression * expr, SourcePosition const & position)
{
	return new SourceVariable(nameSource, type, expr, position);
}
SourceVariable::Pointer SourceVariable::create_constant(std::string const & nameSource, VariableType const * type, std::string const & nameObject, SourcePosition const & position)
{
	return new SourceVariable(nameSource, type, nameObject, position);
}
SourceVariable::Pointer SourceVariable::create_literal(VariableType const * type, ObjectExpression * expr, SourcePosition const & position)
{
	return new SourceVariable("", type, expr, position);
}
SourceVariable::Pointer SourceVariable::create_literal(VariableType const * type, std::string const & nameObject, SourcePosition const & position)
{
	return new SourceVariable("", type, nameObject, position);
}
SourceVariable::Pointer SourceVariable::create_variable(std::string const & nameSource, VariableType const * type, std::string const & nameObject, StorageClass sc, SourcePosition const & position)
{
	return new SourceVariable(nameSource, type, nameObject, sc, position);
}

SourceVariable::StorageClass SourceVariable::get_StorageClass(SourceTokenC const & token)
{
	if (token.getData() == "auto")
		return SC_AUTO;

	if (token.getData() == "register")
		return SC_REGISTER;

	if (token.getData() == "register_global")
		return SC_REGISTER_GLOBAL;

	if (token.getData() == "register_map")
		return SC_REGISTER_MAP;

	if (token.getData() == "register_world")
		return SC_REGISTER_WORLD;

	if (token.getData() == "registerarray_global")
		return SC_REGISTERARRAY_GLOBAL;

	if (token.getData() == "registerarray_map")
		return SC_REGISTERARRAY_MAP;

	if (token.getData() == "registerarray_world")
		return SC_REGISTERARRAY_WORLD;

	if (token.getData() == "static")
		return SC_STATIC;

	throw SourceException("invalid storage-class", token.getPosition(), "SourceVariable");
}

SourceVariable::StorageClass SourceVariable::getClass() const
{
	return _sc;
}

std::string const & SourceVariable::getNameObject() const
{
	return _nameObject;
}
std::string const & SourceVariable::getNameSource() const
{
	return _nameSource;
}

VariableType const * SourceVariable::getType() const
{
	return _type;
}


