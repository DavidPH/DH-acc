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

/* SourceExpression/BranchSwitch.cpp
**
** Defines the SourceExpression_BranchSwitch class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../print_debug.hpp"
#include "../SourceContext.hpp"
#include "../VariableType.hpp"



class SourceExpression_BranchSwitch : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchSwitch, SourceExpression);

public:
	SourceExpression_BranchSwitch(SourceExpression * expr, SourceExpression * exprCases, SourceContext * context, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	SourceExpression::Pointer _expr;
	SourceExpression::Pointer _exprCases;

	std::vector<bigsint> _cases;
	std::vector<std::string> _caseLabels;

	std::string _caseDefault;
	std::string _caseBreak;

	bool _needDefault;
};



SourceExpression::Pointer SourceExpression::create_branch_switch(SourceExpression * expr, SourceExpression * exprCases, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchSwitch(expr, exprCases, context, position);
}



SourceExpression_BranchSwitch::SourceExpression_BranchSwitch(SourceExpression * expr, SourceExpression * exprCases, SourceContext * context, SourcePosition const & position) : Super(position), _expr(expr), _exprCases(exprCases), _cases(context->getCases(position)), _caseLabels(_cases.size()), _caseDefault(context->getLabelCaseDefault(position)), _caseBreak(context->getLabelBreak(position)), _needDefault(!context->hasLabelCaseDefault())
{
	if (_expr->getType()->vt != VariableType::VT_INT)
		_expr = create_value_cast(_expr, VariableType::get_vt_int(), position);

	if (_exprCases->getType()->vt != VariableType::VT_VOID)
		_exprCases = create_value_cast(_exprCases, VariableType::get_vt_void(), position);

	for (size_t i(0); i < _cases.size(); ++i)
		_caseLabels[i] = context->getLabelCase(_cases[i], position);
}

void SourceExpression_BranchSwitch::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	_expr->makeObjectsGet(objects);

	objects->setPosition(position);

	std::vector<ObjectExpression::Pointer> args;
	args.reserve(_cases.size() * 2);

	for (size_t i(0); i < _cases.size(); ++i)
	{
		args.push_back(objects->getValue(_cases[i]));
		args.push_back(objects->getValue(_caseLabels[i]));
	}

	objects->addToken(OCODE_BRANCHTABLE, args);

	objects->addToken(OCODE_BRANCH, objects->getValue(_caseDefault));

	_exprCases->makeObjectsGet(objects);

	if (_needDefault)
		objects->addLabel(_caseDefault);
	objects->addLabel(_caseBreak);
}

void SourceExpression_BranchSwitch::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BranchSwitch(";
	Super::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";

		*out << ", ";

		*out << "exprCases=(";
		print_debug(out, _exprCases);
		*out << ")";

		*out << ", ";

		*out << "cases=(";
		print_debug(out, _cases);
		*out << ")";

		*out << ", ";

		*out << "caseLabels=(";
		print_debug(out, _caseLabels);
		*out << ")";

		*out << ", ";

		*out << "caseDefault=(";
		print_debug(out, _caseDefault);
		*out << ")";

		*out << ", ";

		*out << "caseBreak=(";
		print_debug(out, _caseBreak);
		*out << ")";

		*out << ", ";

		*out << "needDefault=(";
		print_debug(out, _needDefault);
		*out << ")";
	*out << ")";
}


