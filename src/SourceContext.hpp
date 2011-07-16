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
** Defines the SourceContext class.
*/

#ifndef HPP_SourceContext_
#define HPP_SourceContext_

#include "SourceVariable.hpp"

#include <string>
#include <vector>

class SourceTokenC;



class SourceContext
{
public:
	SourceContext();
	SourceContext(SourceContext * parent, bool inheritLocals);

	void addVariable(SourceVariable const & var);

	int getCount(SourceVariable::StorageClass sc) const;

	int getLimit(SourceVariable::StorageClass sc) const;

	SourceVariable const & getVariable(SourceTokenC const & token) const;



	static SourceContext global_context;

private:
	void addCount(int count, SourceVariable::StorageClass sc);
	void addLimit(int limit, SourceVariable::StorageClass sc);

	SourceVariable const & getVariable(std::string const & name, SourcePosition const & position, bool canLocal) const;

	int _countRegister;

	int _limitRegister;

	SourceContext * _parent;

	std::vector<SourceVariable> _vars;
	std::vector<std::string> _varnames;

	unsigned _inheritLocals : 1;
};



#endif /* HPP_SourceContext_ */



