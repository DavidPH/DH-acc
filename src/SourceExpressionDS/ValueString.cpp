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

/* SourceExpressionDS/ValueString.cpp
**
** Defines the SourceExpressionDS_ValueInt class and methods.
*/

#include "Base.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectToken.hpp"



class SourceExpressionDS_ValueString : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_ValueString(ObjectExpression const & symbol, SourcePosition const & position);

	virtual SourceExpressionDS_ValueString * clone() const;

	virtual char const * getName() const;

	virtual SourceExpressionDS::ExpressionType getType() const;

	virtual bool isConstant() const;

	virtual ObjectExpression makeObject() const;
	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	ObjectExpression _symbol;
};



SourceExpressionDS SourceExpressionDS::make_expression_value_string(ObjectExpression const & symbol, SourcePosition const & position)
{
	return new SourceExpressionDS_ValueString(symbol, position);
}



SourceExpressionDS_ValueString::SourceExpressionDS_ValueString(ObjectExpression const & symbol, SourcePosition const & position) : SourceExpressionDS_Base(position), _symbol(symbol)
{

}

SourceExpressionDS_ValueString * SourceExpressionDS_ValueString::clone() const
{
	return new SourceExpressionDS_ValueString(*this);
}

char const * SourceExpressionDS_ValueString::getName() const
{
	return "SourceExpressionDS_ValueString";
}

SourceExpressionDS::ExpressionType SourceExpressionDS_ValueString::getType() const
{
	return SourceExpressionDS::ET_STRING;
}

bool SourceExpressionDS_ValueString::isConstant() const
{
	return true;
}

ObjectExpression SourceExpressionDS_ValueString::makeObject() const
{
	return _symbol;
}
void SourceExpressionDS_ValueString::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	objects->push_back(ObjectToken(ObjectToken::OCODE_PUSHNUMBER, getPosition(), _symbol));
}

void SourceExpressionDS_ValueString::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_ValueString(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "symbol=(";
		print_debug(out, _symbol);
		*out << ")";
	*out << ")";
}



