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



VariableType const * SourceContext::getVariableType(SourceTokenC const & token)
{
	VariableType const * vartype(getVariableTypeNull(token.getData()));

	if (!vartype) throw SourceException("invalid variable-type", token.getPosition(), "SourceContext");

	return vartype;
}
VariableType const * SourceContext::getVariableType(VariableType::Type vt)
{
	// Builtin types always go to the global context.
	if (_parent) return _parent->getVariableType(vt);

	return _types[(vt <= VariableType::VT_VOID) ? vt : VariableType::VT_VOID];
}
VariableType const * SourceContext::getVariableType(VariableType::Type vt, VariableType const * callType, VariableType const * refType, std::vector<VariableType const *> const & types)
{
	// Anonymous types always go to the global context.
	if (_parent) return _parent->getVariableType(vt, callType, refType, types);

	for (std::vector<VariableType *>::iterator it(_types.begin()); it != _types.end(); ++it)
	{
		VariableType const * vartype(*it);

		if (vartype->vt != vt) continue;
		if (vartype->callType != callType) continue;
		if (vartype->refType != refType) continue;
		if (vartype->types.size() != types.size()) continue;

		bool matched(true);

		for (size_t j(0); j < vartype->types.size(); ++j)
		{
			if (vartype->types[j] != types[j])
			{
				matched = false;
				break;
			}
		}

		if (matched) return vartype;
	}

	VariableType * vartype(new VariableType);

	vartype->vt       = vt;
	vartype->callType = callType;
	vartype->refType  = refType;
	vartype->names    = std::vector<std::string>(types.size(), "");
	vartype->types    = types;

	_types.push_back(vartype);
	_typenames.push_back("");

	return vartype;
}

VariableType const * SourceContext::getVariableType_array(VariableType const * refType, bigsint count)
{
	return getVariableType(VariableType::VT_ARRAY, getVariableType(VariableType::VT_VOID), refType, std::vector<VariableType const *>(count, refType));
}

VariableType const * SourceContext::getVariableType_asmfunc(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return getVariableType(VariableType::VT_ASMFUNC, callType, getVariableType(VariableType::VT_VOID), types);
}

VariableType const * SourceContext::getVariableType_block(std::vector<VariableType const *> const & types)
{
	return getVariableType(VariableType::VT_BLOCK, getVariableType(VariableType::VT_VOID), getVariableType(VariableType::VT_VOID), types);
}

VariableType const * SourceContext::getVariableType_function(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return getVariableType(VariableType::VT_FUNCTION, callType, getVariableType(VariableType::VT_VOID), types);
}

VariableType const * SourceContext::getVariableType_linespec(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return getVariableType(VariableType::VT_LINESPEC, callType, getVariableType(VariableType::VT_VOID), types);
}

VariableType const * SourceContext::getVariableType_native(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return getVariableType(VariableType::VT_NATIVE, callType, getVariableType(VariableType::VT_VOID), types);
}

VariableType const * SourceContext::getVariableType_pointer(VariableType const * refType)
{
	return getVariableType(VariableType::VT_POINTER, getVariableType(VariableType::VT_VOID), refType, std::vector<VariableType const *>());
}

VariableType const * SourceContext::getVariableType_script(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return getVariableType(VariableType::VT_SCRIPT, callType, getVariableType(VariableType::VT_VOID), types);
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

		vartype->vt       = VariableType::VT_STRUCT;
		vartype->callType = getVariableType(VariableType::VT_VOID);
		vartype->refType  = getVariableType(VariableType::VT_VOID);

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
		if (!vartype->names.empty())
			throw SourceException("existing VT_STRUCT names not empty", position, "SourceVariable");

		if (!vartype->types.empty())
			throw SourceException("existing VT_STRUCT types not empty", position, "SourceVariable");

		vartype->names = names;
		vartype->types = types;
	}
	else
	{
		vartype = new VariableType;

		vartype->vt       = VariableType::VT_STRUCT;
		vartype->callType = getVariableType(VariableType::VT_VOID);
		vartype->refType  = getVariableType(VariableType::VT_VOID);
		vartype->names    = names;
		vartype->types    = types;

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
	_types.push_back(const_cast<VariableType *>(vartype));

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

		vartype->vt       = VariableType::VT_UNION;
		vartype->callType = getVariableType(VariableType::VT_VOID);
		vartype->refType  = getVariableType(VariableType::VT_VOID);

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
		if (!vartype->names.empty())
			throw SourceException("existing VT_UNION names not empty", position, "SourceVariable");

		if (!vartype->types.empty())
			throw SourceException("existing VT_UNION types not empty", position, "SourceVariable");

		vartype->names = names;
		vartype->types = types;
	}
	else
	{
		vartype = new VariableType;

		vartype->vt       = VariableType::VT_UNION;
		vartype->callType = getVariableType(VariableType::VT_VOID);
		vartype->refType  = getVariableType(VariableType::VT_VOID);
		vartype->names    = names;
		vartype->types    = types;

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

	for (size_t i(0); i < _structnames.size(); ++i)
		if (_structnames[i] == name)
			return _structs[i];

	for (size_t i(0); i < _unionnames.size(); ++i)
		if (_unionnames[i] == name)
			return _unions[i];

	if (_parent) return _parent->getVariableTypeNull(name);

	return NULL;
}



