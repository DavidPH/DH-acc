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

/* SourceExpressionDS/CastReal.cpp
**
** Defines the SourceExpressionDS_CastReal class and methods.
*/

#include "Base.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



class SourceExpressionDS_CastReal : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_CastReal(SourceExpressionDS const & expr, SourcePosition const & position);

	virtual SourceExpressionDS_CastReal * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionDS _expr;
};



SourceExpressionDS SourceExpressionDS::make_expression_cast_real(SourceExpressionDS const & expr, SourcePosition const & position)
{
	return new SourceExpressionDS_CastReal(expr, position);
}



SourceExpressionDS_CastReal::SourceExpressionDS_CastReal(SourceExpressionDS const & expr, SourcePosition const & position) : SourceExpressionDS_Base(position), _expr(expr)
{

}

SourceExpressionDS_CastReal * SourceExpressionDS_CastReal::clone() const
{
	return new SourceExpressionDS_CastReal(*this);
}

char const * SourceExpressionDS_CastReal::getName() const
{
	return "SourceExpressionDS_CastReal";
}

SourceVariable::VariableType const * SourceExpressionDS_CastReal::getType() const
{
	return SourceVariable::get_VariableType(SourceVariable::VT_REAL);
}

bool SourceExpressionDS_CastReal::isConstant() const
{
	return _expr.isConstant();
}

void SourceExpressionDS_CastReal::makeObjectsGet(ObjectVector * objects) const
{
	_expr.makeObjectsGet(objects);

	objects->setPosition(getPosition());

	switch (_expr.getType()->type)
	{
	case SourceVariable::VT_ACSFUNC:
	case SourceVariable::VT_CHAR:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_LNSPEC:
	case SourceVariable::VT_NATIVE:
	case SourceVariable::VT_SCRIPT:
	case SourceVariable::VT_STRING:
		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(16));
		objects->addToken(ObjectToken::OCODE_SHIFTL);
		break;

	case SourceVariable::VT_ASMFUNC:
	case SourceVariable::VT_STRUCT:
	case SourceVariable::VT_VOID:
		throw SourceException("invalid VT", getPosition(), getName());

	case SourceVariable::VT_REAL:
		break;
	}
}

void SourceExpressionDS_CastReal::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_CastReal(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}



