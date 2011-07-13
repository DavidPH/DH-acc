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

/* SourceExpressionDS/RootVoid.cpp
**
** Defines the SourceExpressionDS_RootVoid class and methods.
*/

#include "Base.hpp"

#include "../ObjectToken.hpp"



class SourceExpressionDS_RootVoid : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootVoid(SourceExpressionDS const & expr, SourcePosition const & position);

	virtual SourceExpressionDS_RootVoid * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionDS _expr;

	void doVoid(std::vector<ObjectToken> * const objects, SourceVariable::VariableType const * const type) const;
};



SourceExpressionDS SourceExpressionDS::make_expression_cast_void(SourceExpressionDS const & expr, SourcePosition const & position)
{
	return new SourceExpressionDS_RootVoid(expr, position);
}
SourceExpressionDS SourceExpressionDS::make_expression_root_void(SourceExpressionDS const & expr, SourcePosition const & position)
{
	return new SourceExpressionDS_RootVoid(expr, position);
}



SourceExpressionDS_RootVoid::SourceExpressionDS_RootVoid(SourceExpressionDS const & expr, SourcePosition const & position) : SourceExpressionDS_Base(position), _expr(expr)
{

}

SourceExpressionDS_RootVoid * SourceExpressionDS_RootVoid::clone() const
{
	return new SourceExpressionDS_RootVoid(*this);
}

void SourceExpressionDS_RootVoid::doVoid(std::vector<ObjectToken> * const objects, SourceVariable::VariableType const * const type) const
{
	switch (type->type)
	{
	case SourceVariable::VT_FIXED:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_STRING:
		objects->push_back(ObjectToken(ObjectToken::OCODE_DROP, getPosition()));
		break;

	case SourceVariable::VT_VOID:
		break;

	case SourceVariable::VT_STRUCT:
		for (size_t i(type->types.size()); i--;)
			doVoid(objects, type->types[i]);
		break;
	}
}

char const * SourceExpressionDS_RootVoid::getName() const
{
	return "SourceExpressionDS_RootVoid";
}

SourceVariable::VariableType const * SourceExpressionDS_RootVoid::getType() const
{
	return SourceVariable::get_VariableType(SourceVariable::VT_VOID);
}

bool SourceExpressionDS_RootVoid::isConstant() const
{
	return _expr.isConstant();
}

void SourceExpressionDS_RootVoid::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	_expr.makeObjectsGet(objects);
	doVoid(objects, _expr.getType());
}

void SourceExpressionDS_RootVoid::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootVoid(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}



