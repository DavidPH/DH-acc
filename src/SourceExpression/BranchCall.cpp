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

/* SourceExpression/BranchCall.cpp
**
** Defines the SourceExpression_BranchCall class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../print_debug.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"



class SourceExpression_BranchCall : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchCall, SourceExpression);

public:
	SourceExpression_BranchCall(SourceExpression * expr, std::vector<SourceExpression::Pointer> const & args, SourceContext * context, SourcePosition const & position);

	virtual VariableType const * getType() const;

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	std::vector<SourceExpression::Pointer> _args;
	SourceExpression::Pointer _expr;
	bigsint _stack;
};



SourceExpression::Pointer SourceExpression::create_branch_call(SourceExpression * expr, std::vector<SourceExpression::Pointer> const & args, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchCall(expr, args, context, position);
}



SourceExpression_BranchCall::SourceExpression_BranchCall(SourceExpression * expr, std::vector<SourceExpression::Pointer> const & args, SourceContext * context, SourcePosition const & position) : Super(position), _args(args), _expr(expr), _stack(context->getLimit(SourceVariable::SC_AUTO))
{
	VariableType const * type(_expr->getType());

	if (_args.size() != type->types.size())
		throw SourceException("incorrect arg count", position, getName());

	for (size_t i(0); i < _args.size(); ++i)
	{
		if (_args[i]->getType() != type->types[i])
			_args[i] = create_value_cast(_args[i], type->types[i], position);
	}
}

VariableType const * SourceExpression_BranchCall::getType() const
{
	return _expr->getType()->callType;
}

void SourceExpression_BranchCall::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	make_objects_call(objects, _expr, _args, objects->getValue(_stack), position);
}

void SourceExpression_BranchCall::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BranchCall(";
	Super::printDebug(out);
	*out << " ";
		*out << "args=(";
		print_debug(out, _args);
		*out << ")";

		*out << ", ";

		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}



