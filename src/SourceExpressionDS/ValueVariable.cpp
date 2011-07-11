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

/* SourceExpressionDS/ValueVariable.cpp
**
** Defines the SourceExpressionDS_ValueVariable class and methods.
*/

#include "Base.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectToken.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceVariable.hpp"

#include <sstream>
#include <stdint.h>



class SourceExpressionDS_ValueVariable : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_ValueVariable(SourceVariable const & var, SourcePosition const & position);

	virtual SourceExpressionDS_ValueVariable * clone() const;

	virtual char const * getName() const;

	virtual SourceExpressionDS::ExpressionType getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;
	virtual void makeObjectsSet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceVariable _var;
};



SourceExpressionDS SourceExpressionDS::make_expression_value_variable(SourceVariable const & var, SourcePosition const & position)
{
	return new SourceExpressionDS_ValueVariable(var, position);
}



SourceExpressionDS_ValueVariable::SourceExpressionDS_ValueVariable(SourceVariable const & var, SourcePosition const & position) : SourceExpressionDS_Base(position), _var(var)
{

}

SourceExpressionDS_ValueVariable * SourceExpressionDS_ValueVariable::clone() const
{
	return new SourceExpressionDS_ValueVariable(*this);
}

char const * SourceExpressionDS_ValueVariable::getName() const
{
	return "SourceExpressionDS_ValueVariable";
}

SourceExpressionDS::ExpressionType SourceExpressionDS_ValueVariable::getType() const
{
	switch (_var.getType())
	{
	case SourceVariable::VT_FIXED: return SourceExpressionDS::ET_FIXED;
	case SourceVariable::VT_INT:   return SourceExpressionDS::ET_INT;
	}

	return SourceExpressionDS::ET_VOID;
}

bool SourceExpressionDS_ValueVariable::isConstant() const
{
	return false;
}

void SourceExpressionDS_ValueVariable::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	_var.makeObjectsGet(objects);
}
void SourceExpressionDS_ValueVariable::makeObjectsSet(std::vector<ObjectToken> * const objects) const
{
	_var.makeObjectsSet(objects);
}

void SourceExpressionDS_ValueVariable::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_ValueVariable(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "var=(";
		print_debug(out, _var);
		*out << ")";
	*out << ")";
}



