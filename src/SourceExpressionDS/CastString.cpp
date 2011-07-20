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

/* SourceExpressionDS/CastString.cpp
**
** Defines the SourceExpressionDS_CastString class and methods.
*/

#include "Base.hpp"

#include "../ObjectToken.hpp"
#include "../SourceException.hpp"



class SourceExpressionDS_CastString : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_CastString(SourceExpressionDS const & expr, SourcePosition const & position);

	virtual SourceExpressionDS_CastString * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionDS _expr;
};



SourceExpressionDS SourceExpressionDS::make_expression_cast_string(SourceExpressionDS const & expr, SourcePosition const & position)
{
	return new SourceExpressionDS_CastString(expr, position);
}



SourceExpressionDS_CastString::SourceExpressionDS_CastString(SourceExpressionDS const & expr, SourcePosition const & position) : SourceExpressionDS_Base(position), _expr(expr)
{

}

SourceExpressionDS_CastString * SourceExpressionDS_CastString::clone() const
{
	return new SourceExpressionDS_CastString(*this);
}

char const * SourceExpressionDS_CastString::getName() const
{
	return "SourceExpressionDS_CastString";
}

SourceVariable::VariableType const * SourceExpressionDS_CastString::getType() const
{
	return SourceVariable::get_VariableType(SourceVariable::VT_STRING);
}

bool SourceExpressionDS_CastString::isConstant() const
{
	return _expr.isConstant();
}

void SourceExpressionDS_CastString::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	_expr.makeObjectsGet(objects);

	switch (_expr.getType()->type)
	{
	case SourceVariable::VT_CHAR:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_LNSPEC:
	case SourceVariable::VT_NATIVE:
	case SourceVariable::VT_SCRIPT:
	case SourceVariable::VT_STRING:
		break;

	case SourceVariable::VT_FIXED:
		objects->push_back(ObjectToken(ObjectToken::OCODE_PUSHNUMBER, getPosition(), ObjectExpression::create_value_int32(16, getPosition())));
		objects->push_back(ObjectToken(ObjectToken::OCODE_SHIFTR, getPosition()));
		break;

	case SourceVariable::VT_STRUCT:
	case SourceVariable::VT_VOID:
		throw SourceException("invalid VT", getPosition(), getName());
	}
}

void SourceExpressionDS_CastString::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_CastString(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}



