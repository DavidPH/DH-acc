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

/* SourceContext.hpp
**
** Defines the SourceContext methods.
*/

#include "SourceContext.hpp"

#include "SourceException.hpp"
#include "SourceTokenC.hpp"
#include "SourceVariable.hpp"

#include <cstring>
#include <sstream>



SourceContext SourceContext::global_context;



SourceContext::SourceContext() : _labelCount(0), _parent(NULL), _returnType(SourceVariable::get_VariableType(SourceVariable::VT_VOID)), _type(CT_BLOCK), _inheritLocals(false)
{
	std::memset(_count, 0, sizeof(_count));
	std::memset(_limit, 0, sizeof(_limit));

	// Stack pointer and array temporary.
	_count[SourceVariable::SC_REGISTER_WORLD] = 2;
	_limit[SourceVariable::SC_REGISTER_WORLD] = 2;

	// Pointer-addressable space.
	_count[SourceVariable::SC_REGISTERARRAY_GLOBAL] = 1;
	_limit[SourceVariable::SC_REGISTERARRAY_GLOBAL] = 1;
}
SourceContext::SourceContext(SourceContext * parent, ContextType type) : _label(parent->makeLabelShort()), _labelCount(0), _parent(parent), _returnType(NULL), _type(type), _inheritLocals(type == CT_BLOCK)
{
	std::memset(_count, 0, sizeof(_count));
	std::memset(_limit, 0, sizeof(_limit));
}

void SourceContext::addCount(int count, SourceVariable::StorageClass sc)
{
	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_REGISTER:
		_count[sc] += count;

		addLimit(getCount(sc), sc);

		break;

	case SourceVariable::SC_CONSTANT:
		break;

	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_MAP:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
		if (_parent)
			_parent->addCount(count, sc);
		else
			_count[sc] += count;

		addLimit(getCount(sc), sc);

		break;
	}
}

void SourceContext::addLimit(int limit, SourceVariable::StorageClass sc)
{
	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_REGISTER:
		if (limit > _limit[sc])
			_limit[sc] = limit;

		if (_inheritLocals && _parent)
			_parent->addLimit(limit, sc);

		break;

	case SourceVariable::SC_CONSTANT:
		break;

	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_MAP:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
		if (limit > _limit[sc])
			_limit[sc] = limit;

		if (_parent)
			_parent->addLimit(limit, sc);

		break;
	}
}

void SourceContext::addVariable(SourceVariable const & var)
{
	_vars.push_back(var);
	_varnames.push_back(var.getNameSource());

	SourceVariable::StorageClass sc(var.getClass());
	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_CONSTANT:
	case SourceVariable::SC_REGISTER:
	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
		addCount(var.getType()->size(), sc);
		break;

	case SourceVariable::SC_REGISTERARRAY_MAP:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
		// Register arrays only require a single allocation regardless of size.
		addCount(1, sc);
		break;
	}
}

int SourceContext::getCount(SourceVariable::StorageClass sc) const
{
	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_REGISTER:
		if (_inheritLocals && _parent)
			return _parent->getCount(sc) + _count[sc];
		else
			return _count[sc];

	case SourceVariable::SC_CONSTANT:
		return 0;

	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_MAP:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
		if (_parent)
			return _parent->getCount(sc) + _count[sc];
		else
			return _count[sc];
	}

	throw SourceException("getCount", SourcePosition::none, "SourceContext");
}

std::string SourceContext::getLabel() const
{
	if (_parent)
		return _parent->getLabel() + _label;
	else
		return _label;
}

int SourceContext::getLimit(SourceVariable::StorageClass sc) const
{
	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_REGISTER:
	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_MAP:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
		return _limit[sc];

	case SourceVariable::SC_CONSTANT:
		return 0;
	}

	throw SourceException("getCount", SourcePosition::none, "SourceContext");
}

SourceVariable::VariableType const * SourceContext::getReturnType() const
{
	return _returnType ? _returnType : _parent->getReturnType();
}

SourceContext::ContextType SourceContext::getTypeRoot() const
{
	if (_type == CT_BLOCK && _parent) return _parent->getTypeRoot();

	return _type;
}

SourceVariable const & SourceContext::getVariable(SourceTokenC const & token) const
{
	return getVariable(token.getData(), token.getPosition(), true);
}
SourceVariable const & SourceContext::getVariable(std::string const & name, SourcePosition const & position, bool canLocal) const
{
	for (size_t i(_vars.size()); i--;)
	{
		if (name == _varnames[i])
		{
			switch (_vars[i].getClass())
			{
			case SourceVariable::SC_AUTO:
			case SourceVariable::SC_REGISTER:
				if (canLocal)
					return _vars[i];

				break;

			case SourceVariable::SC_CONSTANT:
			case SourceVariable::SC_REGISTER_GLOBAL:
			case SourceVariable::SC_REGISTER_MAP:
			case SourceVariable::SC_REGISTER_WORLD:
			case SourceVariable::SC_REGISTERARRAY_GLOBAL:
			case SourceVariable::SC_REGISTERARRAY_MAP:
			case SourceVariable::SC_REGISTERARRAY_WORLD:
				return _vars[i];
			}
		}
	}

	if (_parent) return _parent->getVariable(name, position, canLocal && _inheritLocals);

	throw SourceException("no such variable '" + name + "'", position, "SourceContext");
}

std::string SourceContext::makeLabel()
{
	return getLabel() + makeLabelShort();
}
std::string SourceContext::makeLabelShort()
{
	std::ostringstream oss;

	oss << "block" << ++_labelCount;

	return oss.str();
}

std::string SourceContext::makeNameObject(SourceVariable::StorageClass sc, SourceVariable::VariableType const * type, std::string const & nameSource, SourcePosition const & position) const
{
	std::string nameObject(getLabel() + nameSource);

	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_REGISTER:
		ObjectExpression::add_symbol(nameObject, ObjectExpression::create_value_int(getCount(sc), position));
		break;

	case SourceVariable::SC_CONSTANT:
		throw SourceException("makeNameObject on SC_CONSTANT", position, "SourceContext");

	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
		ObjectExpression::add_symbol(nameObject, ObjectExpression::create_value_int(getCount(sc), position));
		break;

	case SourceVariable::SC_REGISTERARRAY_MAP:
		ObjectExpression::add_registerarray_map(nameObject, getCount(sc), type->size());
		break;
	}

	return nameObject;
}

void SourceContext::setReturnType(SourceVariable::VariableType const * returnType)
{
	_returnType = returnType;
}



