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

/* SourceExpressionDS/ValueInt.cpp
**
** Defines the SourceExpressionDS_ValueInt class and methods.
*/

#include "Base.hpp"

#include "../ObjectToken.hpp"
#include "../SourceTokenC.hpp"

#include <sstream>
#include <stdint.h>



class SourceExpressionDS_ValueInt : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_ValueInt(SourceTokenC const & token);

	virtual SourceExpressionDS_ValueInt * clone() const;

	virtual SourceExpressionDS::ExpressionType getType() const;

	virtual bool isConstant() const;

	virtual void makeObjects(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	int32_t _value;
};



SourceExpressionDS SourceExpressionDS::make_expression_value_int(SourceTokenC const & token)
{
	return new SourceExpressionDS_ValueInt(token);
}



SourceExpressionDS_ValueInt::SourceExpressionDS_ValueInt(SourceTokenC const & token) : SourceExpressionDS_Base(token.getPosition()), _value(0)
{
	std::istringstream(token.getData()) >> _value;
}

SourceExpressionDS_ValueInt * SourceExpressionDS_ValueInt::clone() const
{
	return new SourceExpressionDS_ValueInt(*this);
}

SourceExpressionDS::ExpressionType SourceExpressionDS_ValueInt::getType() const
{
	return SourceExpressionDS::ET_INT;
}

bool SourceExpressionDS_ValueInt::isConstant() const
{
	return true;
}

void SourceExpressionDS_ValueInt::makeObjects(std::vector<ObjectToken> * const objects) const
{
	std::vector<ObjectExpression> args;

	args.push_back(ObjectExpression::create_value_int32(_value, getPosition()));

	objects->push_back(ObjectToken(ObjectToken::OCODE_PUSHNUMBER, getPosition(), getLabels(), args));
}

void SourceExpressionDS_ValueInt::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_ValueInt(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "value=(";
		*out << _value;
		*out << ")";
	*out << ")";
}



