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

/* SourceExpressionDS/RootWhile.cpp
**
** Defines the SourceExpressionDS_RootWhile class and methods.
*/

#include "Base.hpp"

#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"



class SourceExpressionDS_RootWhile : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootWhile(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprWhile, SourceContext * context, SourcePosition const & position);

	virtual SourceExpressionDS_RootWhile * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionDS _exprCondition;
	SourceExpressionDS _exprWhile;

	std::string _labelCondition;
	std::string _labelEnd;
};



SourceExpressionDS SourceExpressionDS::make_expression_root_while(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprWhile, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpressionDS_RootWhile(exprCondition, exprWhile, context, position);
}



SourceExpressionDS_RootWhile::SourceExpressionDS_RootWhile(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprWhile, SourceContext * context, SourcePosition const & position) : SourceExpressionDS_Base(position), _exprCondition(exprCondition), _exprWhile(exprWhile)
{
	std::string label(context->makeLabel());

	_labelCondition = label + "_condition";
	_labelEnd       = label + "_end";
}

SourceExpressionDS_RootWhile * SourceExpressionDS_RootWhile::clone() const
{
	return new SourceExpressionDS_RootWhile(*this);
}

char const * SourceExpressionDS_RootWhile::getName() const
{
	return "SourceExpressionDS_RootWhile";
}

SourceVariable::VariableType const * SourceExpressionDS_RootWhile::getType() const
{
	return SourceVariable::get_VariableType(SourceVariable::VT_VOID);
}

bool SourceExpressionDS_RootWhile::isConstant() const
{
	return false;
}

void SourceExpressionDS_RootWhile::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(_labelCondition);
	_exprCondition.makeObjectsGet(objects);
	objects->setPosition(getPosition());
	objects->addToken(ObjectToken::OCODE_BRANCHZERO, objects->getValue(_labelEnd));

	_exprWhile.makeObjectsGet(objects);
	objects->addToken(ObjectToken::OCODE_BRANCH, objects->getValue(_labelCondition));

	objects->addLabel(_labelEnd);
}

void SourceExpressionDS_RootWhile::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootWhile(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "exprCondition=(";
		print_debug(out, _exprCondition);
		*out << ")";

		*out << ", ";

		*out << "exprWhile=(";
		print_debug(out, _exprWhile);
		*out << ")";
	*out << ")";
}



