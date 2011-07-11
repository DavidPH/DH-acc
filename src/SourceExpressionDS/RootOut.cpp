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

/* SourceExpressionDS/RootOut.cpp
**
** Defines the SourceExpressionDS_RootOut class and methods.
*/

#include "Base.hpp"

#include "../ObjectToken.hpp"



class SourceExpressionDS_RootOut : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootOut(SourceExpressionDS const & expr, SourcePosition const & position);

	virtual SourceExpressionDS_RootOut * clone() const;

	virtual char const * getName() const;

	virtual SourceExpressionDS::ExpressionType getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionDS _expr;
};



SourceExpressionDS SourceExpressionDS::make_expression_root_out(SourceExpressionDS const & expr, SourcePosition const & position)
{
	return new SourceExpressionDS_RootOut(expr, position);
}



SourceExpressionDS_RootOut::SourceExpressionDS_RootOut(SourceExpressionDS const & expr, SourcePosition const & position) : SourceExpressionDS_Base(position), _expr(expr)
{

}

SourceExpressionDS_RootOut * SourceExpressionDS_RootOut::clone() const
{
	return new SourceExpressionDS_RootOut(*this);
}

char const * SourceExpressionDS_RootOut::getName() const
{
	return "SourceExpressionDS_RootOut";
}

SourceExpressionDS::ExpressionType SourceExpressionDS_RootOut::getType() const
{
	return SourceExpressionDS::ET_VOID;
}

bool SourceExpressionDS_RootOut::isConstant() const
{
	return false;
}

void SourceExpressionDS_RootOut::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	_expr.makeObjectsGet(objects);

	switch (_expr.getType())
	{
	case SourceExpressionDS::ET_FIXED:
		objects->push_back(ObjectToken(ObjectToken::OCODE_BEGINPRINT, getPosition()));
		objects->push_back(ObjectToken(ObjectToken::OCODE_PRINTFIXED, getPosition()));
		objects->push_back(ObjectToken(ObjectToken::OCODE_ENDLOG, getPosition()));
		break;

	case SourceExpressionDS::ET_INT:
		objects->push_back(ObjectToken(ObjectToken::OCODE_BEGINPRINT, getPosition()));
		objects->push_back(ObjectToken(ObjectToken::OCODE_PRINTNUMBER, getPosition()));
		objects->push_back(ObjectToken(ObjectToken::OCODE_ENDLOG, getPosition()));
		break;

	case SourceExpressionDS::ET_VOID:
		break;
	}
}

void SourceExpressionDS_RootOut::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootOut(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}



