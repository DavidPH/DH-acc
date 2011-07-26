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
	enum ContextType
	{
		CT_ACSFUNC,
		CT_BLOCK,
		CT_SCRIPT
	};



	SourceContext();
	SourceContext(SourceContext * parent, ContextType type);

	void addVariable(SourceVariable const & var);

	int getCount(SourceVariable::StorageClass sc) const;

	std::string getLabel() const;

	int getLimit(SourceVariable::StorageClass sc) const;

	SourceVariable::VariableType const * getReturnType() const;

	ContextType getType() const;
	ContextType getTypeRoot() const;

	SourceVariable const & getVariable(SourceTokenC const & token) const;

	std::string makeLabel();

	void setReturnType(SourceVariable::VariableType const * returnType);



	static SourceContext global_context;

private:
	void addCount(int count, SourceVariable::StorageClass sc);
	void addLimit(int limit, SourceVariable::StorageClass sc);

	SourceVariable const & getVariable(std::string const & name, SourcePosition const & position, bool canLocal) const;

	std::string makeLabelShort();

	int _count[SourceVariable::SC_REGISTERARRAY_WORLD+1];

	std::string _label;
	int _labelCount;

	int _limit[SourceVariable::SC_REGISTERARRAY_WORLD+1];

	SourceContext * _parent;

	SourceVariable::VariableType const * _returnType;

	ContextType _type;

	std::vector<SourceVariable> _vars;
	std::vector<std::string> _varnames;

	unsigned _inheritLocals : 1;
};



#endif /* HPP_SourceContext_ */


