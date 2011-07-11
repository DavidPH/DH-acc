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



SourceContext::SourceContext() : _addressRegister(0)
{

}

void SourceContext::addVariable(SourceVariable const & var)
{
	_vars[var.getNameSource()] = var;
}

int SourceContext::getAddress(SourceVariable::StorageClass const sc)
{
	switch (sc)
	{
	case SourceVariable::SC_REGISTER: return _addressRegister++;
	}

	throw SourceException("getAddress", SourcePosition::none, "SourceContext");
}

SourceVariable const & SourceContext::getVariable(SourceTokenC const & token) const
{
	std::map<std::string, SourceVariable>::const_iterator var(_vars.find(token.getData()));

	if (var == _vars.end())
		throw SourceException("no such variable", token.getPosition(), "SourceContext");

	return var->second;
}



