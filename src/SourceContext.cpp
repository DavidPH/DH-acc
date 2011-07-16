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



SourceContext SourceContext::global_context;



SourceContext::SourceContext() : _countRegister(0), _limitRegister(0), _parent(NULL), _inheritLocals(false)
{

}
SourceContext::SourceContext(SourceContext * parent, bool inheritLocals) : _countRegister(0), _limitRegister(0), _parent(parent), _inheritLocals(inheritLocals)
{

}

void SourceContext::addCount(int count, SourceVariable::StorageClass sc)
{
	switch (sc)
	{
	case SourceVariable::SC_REGISTER:
		addLimit(_countRegister += count, sc);
		break;
	}
}

void SourceContext::addLimit(int limit, SourceVariable::StorageClass sc)
{
	switch (sc)
	{
	case SourceVariable::SC_REGISTER:
		if (limit > _limitRegister)
			_limitRegister = limit;

		if (_inheritLocals && _parent)
			_parent->addLimit(limit, sc);

		break;
	}
}

void SourceContext::addVariable(SourceVariable const & var)
{
	_vars.push_back(var);
	_varnames.push_back(var.getNameSource());

	addCount(var.getType()->size(), var.getClass());
}

int SourceContext::getCount(SourceVariable::StorageClass sc) const
{
	switch (sc)
	{
	case SourceVariable::SC_REGISTER:
		if (_inheritLocals && _parent)
			return _parent->getCount(sc) + _countRegister;
		else
			return _countRegister;
	}

	throw SourceException("getCount", SourcePosition::none, "SourceContext");
}

int SourceContext::getLimit(SourceVariable::StorageClass sc) const
{
	switch (sc)
	{
	case SourceVariable::SC_REGISTER:
		return _limitRegister;
	}

	throw SourceException("getCount", SourcePosition::none, "SourceContext");
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
			return _vars[i];
	}

	if (_parent) return _parent->getVariable(name, position, canLocal && _inheritLocals);

	throw SourceException("no such variable", position, "SourceContext");
}



