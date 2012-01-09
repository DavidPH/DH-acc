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

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



class SourceExpression_BranchIf : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchIf, SourceExpression);

public:
	SourceExpression_BranchIf(SourceExpression * exprCondition, SourceExpression * exprIf, SourceContext * context, SourcePosition const & position);
	SourceExpression_BranchIf(SourceExpression * exprCondition, SourceExpression * exprIf, SourceExpression * exprElse, SourceContext * context, SourcePosition const & position);

	virtual VariableType const * getType() const;

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

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



SourceExpression_BranchIf::SourceExpression_BranchIf(SourceExpression * exprCondition, SourceExpression * exprIf, SourceExpression * exprElse, SourceContext * context, SourcePosition const & position_) : Super(position_), _exprCondition(exprCondition), _exprIf(exprIf), _exprElse(exprElse)
{
	std::string label(context->makeLabel());

	_labelIf   = label + "_if";
	_labelElse = label + "_else";
	_labelEnd  = label + "_end";

	if (_exprCondition->getType()->vt != VariableType::VT_BOOLSOFT)
		_exprCondition = create_value_cast(_exprCondition, VariableType::get_vt_boolsoft(), position);

	if (_exprElse)
	{
		if (_exprIf->getType() != _exprElse->getType() && (!_exprIf->getType()->isVoid(position) || !_exprIf->getType()->isVoid(position)))
			throw SourceException("if/else type mismatch", position, getName());
	}
	else
	{
		if (!_exprIf->getType()->isVoid(position))
			throw SourceException("if type nonvoid", position, getName());
	}
}

VariableType const * SourceExpression_BranchIf::getType() const
{
	return _exprIf->getType();
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

void SourceExpression_BranchIf::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	VariableData::Pointer src = VariableData::create_stack(_exprCondition->getType()->size(position));

	Super::recurse_makeObjects(objects, dst);

	_exprCondition->makeObjects(objects, src);
	objects->setPosition(position);
	objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(_exprElse ? _labelElse : _labelEnd));

	objects->addLabel(_labelIf);
	_exprIf->makeObjects(objects, dst);

	if (_exprElse)
	{
		objects->setPosition(position);
		objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(_labelEnd));
		objects->addLabel(_labelElse);
		_exprElse->makeObjects(objects, dst);
	}

	objects->addLabel(_labelEnd);
}


