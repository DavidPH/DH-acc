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

/* SourceExpression/BranchIf.cpp
**
** Defines the SourceExpression_BranchIf class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"



class SourceExpression_BranchIf : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchIf, SourceExpression);

public:
	SourceExpression_BranchIf(SourceExpression * exprCondition, SourceExpression * exprIf, SourceContext * context, SourcePosition const & position);
	SourceExpression_BranchIf(SourceExpression * exprCondition, SourceExpression * exprIf, SourceExpression * exprElse, SourceContext * context, SourcePosition const & position);

	virtual SourceVariable::VariableType const * getType() const;

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	SourceExpression::Pointer _exprCondition;
	SourceExpression::Pointer _exprIf;
	SourceExpression::Pointer _exprElse;

	std::string _labelIf;
	std::string _labelElse;
	std::string _labelEnd;
};



SourceExpression::Pointer SourceExpression::create_branch_if(SourceExpression * exprCondition, SourceExpression * exprIf, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchIf(exprCondition, exprIf, NULL, context, position);
}
SourceExpression::Pointer SourceExpression::create_branch_if(SourceExpression * exprCondition, SourceExpression * exprIf, SourceExpression * exprElse, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchIf(exprCondition, exprIf, exprElse, context, position);
}



SourceExpression_BranchIf::SourceExpression_BranchIf(SourceExpression * exprCondition, SourceExpression * exprIf, SourceExpression * exprElse, SourceContext * context, SourcePosition const & position) : Super(position), _exprCondition(exprCondition), _exprIf(exprIf), _exprElse(exprElse)
{
	std::string label(context->makeLabel());

	_labelIf   = label + "_if";
	_labelElse = label + "_else";
	_labelEnd  = label + "_end";

	if (_exprCondition->getType()->type != SourceVariable::VT_BOOLSOFT)
		_exprCondition = create_value_cast(_exprCondition, SourceVariable::get_VariableType(SourceVariable::VT_BOOLSOFT), position);

	if (_exprElse)
	{
		if (_exprIf->getType() != _exprElse->getType() && (!_exprIf->getType()->isVoid() || !_exprIf->getType()->isVoid()))
			throw SourceException("if/else type mismatch", position, getName());
	}
	else
	{
		if (!_exprIf->getType()->isVoid())
			throw SourceException("if type nonvoid", position, getName());
	}
}

SourceVariable::VariableType const * SourceExpression_BranchIf::getType() const
{
	return _exprIf->getType();
}

void SourceExpression_BranchIf::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	_exprCondition->makeObjectsGet(objects);
	objects->setPosition(position);
	objects->addToken(ObjectToken::OCODE_BRANCHZERO, objects->getValue(_exprElse ? _labelElse : _labelEnd));

	objects->addLabel(_labelIf);
	_exprIf->makeObjectsGet(objects);

	if (_exprElse)
	{
		objects->setPosition(position);
		objects->addToken(ObjectToken::OCODE_BRANCH, objects->getValue(_labelEnd));
		objects->addLabel(_labelElse);
		_exprElse->makeObjectsGet(objects);
	}

	objects->addLabel(_labelEnd);
}

void SourceExpression_BranchIf::printDebug(std::ostream * const out) const
{
	*out << "SourceExpression_BranchIf(";
	Super::printDebug(out);
	*out << " ";
		*out << "exprCondition=(";
		print_debug(out, _exprCondition);
		*out << ")";

		*out << ", ";

		*out << "exprIf=(";
		print_debug(out, _exprIf);
		*out << ")";

		*out << ", ";

		*out << "exprElse=(";
		print_debug(out, _exprElse);
		*out << ")";
	*out << ")";
}



