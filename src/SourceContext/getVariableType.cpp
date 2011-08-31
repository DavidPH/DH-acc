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

/* SourceContext/getVariableType.cpp
**
** Defines the SourceContext::getVariableType* methods.
*/

#include "../SourceContext.hpp"

#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../VariableType.hpp"



VariableType const * SourceContext::getVariableType(SourceTokenC const & token)
{
	VariableType const * vartype(getVariableTypeNull(token.getData()));

	if (!vartype) throw SourceException("invalid variable-type", token.getPosition(), "SourceContext");

	return vartype;
}

VariableType * SourceContext::getVariableType_enum(std::string const & name)
{
	if (name.empty()) return NULL;

	for (size_t i(0); i < _enumnames.size(); ++i)
		if (_enumnames[i] == name)
			return _enums[i];

	if (_parent) return _parent->getVariableType_enum(name);

	return NULL;
}
VariableType const * SourceContext::getVariableType_enum(std::string const & name, bool block, SourcePosition const & position)
{
	VariableType * vartype(getVariableType_enum(name));

	if (vartype)
	{
		if (block && vartype->complete)
			throw SourceException("enum redefined", position, "SourceContext");

		vartype->complete = block;
	}
	else
	{
		vartype = new VariableType;

		vartype->vt        = VariableType::VT_ENUM;
		vartype->complete  = block;
		vartype->callType  = VariableType::get_vt_void();
		vartype->constType = NULL;
		vartype->refType   = VariableType::get_vt_void();

		vartype->constType = (new VariableType(*vartype))->doConst();

		_enums.push_back(vartype);
		_enumnames.push_back(name);
	}

	return vartype;
}

VariableType * SourceContext::getVariableType_struct(std::string const & name)
{
	if (name.empty()) return NULL;

	for (size_t i(0); i < _structnames.size(); ++i)
		if (_structnames[i] == name)
			return _structs[i];

	if (_parent) return _parent->getVariableType_struct(name);

	return NULL;
}
VariableType const * SourceContext::getVariableType_struct(std::string const & name, SourcePosition const & position)
{
	VariableType * vartype(getVariableType_struct(name));

	if (!vartype)
	{
		vartype = new VariableType;

		vartype->vt        = VariableType::VT_STRUCT;
		vartype->complete  = false;
		vartype->callType  = VariableType::get_vt_void();
		vartype->constType = NULL;
		vartype->refType   = VariableType::get_vt_void();

		vartype->constType = (new VariableType(*vartype))->doConst();

		_structs.push_back(vartype);
		_structnames.push_back(name);
	}

	return vartype;
}
VariableType const * SourceContext::getVariableType_struct(std::string const & name, std::vector<std::string> const & names, std::vector<VariableType const *> const & types, SourcePosition const & position)
{
	VariableType * vartype(getVariableType_struct(name));

	if (vartype)
	{
		if (vartype->complete)
			throw SourceException("struct redefined", position, "SourceContext");

		vartype->complete = true;
		vartype->names = const_cast<VariableType *>(vartype->constType)->names = names;
		vartype->types = const_cast<VariableType *>(vartype->constType)->types = types;

		const_cast<VariableType *>(vartype->constType)->doConst();
	}
	else
	{
		vartype = new VariableType;

		vartype->vt        = VariableType::VT_STRUCT;
		vartype->complete  = true;
		vartype->callType  = VariableType::get_vt_void();
		vartype->constType = NULL;
		vartype->refType   = VariableType::get_vt_void();
		vartype->names     = names;
		vartype->types     = types;

		vartype->constType = (new VariableType(*vartype))->doConst();

		_structs.push_back(vartype);
		_structnames.push_back(name);
	}

	return vartype;
}

VariableType const * SourceContext::getVariableType_typedef(SourceTokenC const & token, VariableType const * type)
{
	return getVariableType_typedef(token.getData(), type, token.getPosition());
}
VariableType const * SourceContext::getVariableType_typedef(std::string const & name, VariableType const * vartype, SourcePosition const & position)
{
	for (size_t i(0); i < _typenames.size(); ++i)
		if (_typenames[i] == name)
			throw SourceException("typedef name used '" + name + "'", position, "SourceContext");

	_typenames.push_back(name);
	_types.push_back(vartype);

	return vartype;
}

VariableType * SourceContext::getVariableType_union(std::string const & name)
{
	if (name.empty()) return NULL;

	for (size_t i(0); i < _unionnames.size(); ++i)
		if (_unionnames[i] == name)
			return _unions[i];

	if (_parent) return _parent->getVariableType_union(name);

	return NULL;
}
VariableType const * SourceContext::getVariableType_union(std::string const & name, SourcePosition const & position)
{
	VariableType * vartype(getVariableType_union(name));

	if (!vartype)
	{
		vartype = new VariableType;

		vartype->vt        = VariableType::VT_UNION;
		vartype->complete  = false;
		vartype->callType  = VariableType::get_vt_void();
		vartype->constType = NULL;
		vartype->refType   = VariableType::get_vt_void();

		vartype->constType = (new VariableType(*vartype))->doConst();

		_unions.push_back(vartype);
		_unionnames.push_back(name);
	}

	return vartype;
}
VariableType const * SourceContext::getVariableType_union(std::string const & name, std::vector<std::string> const & names, std::vector<VariableType const *> const & types, SourcePosition const & position)
{
	VariableType * vartype(getVariableType_union(name));

	if (vartype)
	{
		if (vartype->complete)
			throw SourceException("union redefined", position, "SourceContext");

		vartype->complete = true;
		vartype->names = const_cast<VariableType *>(vartype->constType)->names = names;
		vartype->types = const_cast<VariableType *>(vartype->constType)->types = types;

		const_cast<VariableType *>(vartype->constType)->doConst();
	}
	else
	{
		vartype = new VariableType;

		vartype->vt        = VariableType::VT_UNION;
		vartype->complete  = true;
		vartype->callType  = VariableType::get_vt_void();
		vartype->constType = NULL;
		vartype->refType   = VariableType::get_vt_void();
		vartype->names     = names;
		vartype->types     = types;

		vartype->constType = (new VariableType(*vartype))->doConst();

		_unions.push_back(vartype);
		_unionnames.push_back(name);
	}

	return vartype;
}

VariableType const * SourceContext::getVariableTypeNull(std::string const & name)
{
	if (name.empty()) return NULL;

	for (size_t i(0); i < _typenames.size(); ++i)
		if (_typenames[i] == name)
			return _types[i];

	for (size_t i(0); i < _enumnames.size(); ++i)
		if (_enumnames[i] == name)
			return _enums[i];

	for (size_t i(0); i < _structnames.size(); ++i)
		if (_structnames[i] == name)
			return _structs[i];

	for (size_t i(0); i < _unionnames.size(); ++i)
		if (_unionnames[i] == name)
			return _unions[i];

	if (_parent) return _parent->getVariableTypeNull(name);

	return NULL;
}


