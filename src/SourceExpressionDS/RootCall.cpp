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

/* SourceExpressionDS/RootCall.cpp
**
** Defines the SourceExpressionDS_RootCall class and methods.
*/

#include "Base.hpp"

#include "../print_debug.hpp"
#include "../SourceException.hpp"



class SourceExpressionDS_RootCall : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootCall(SourceExpressionDS const & expr, std::vector<SourceExpressionDS> const & args, SourcePosition const & position);

	virtual SourceExpressionDS_RootCall * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	std::vector<SourceExpressionDS> _args;
	SourceExpressionDS _expr;
};



SourceExpressionDS SourceExpressionDS::make_expression_root_call(SourceExpressionDS const & expr, std::vector<SourceExpressionDS> const & args, SourcePosition const & position)
{
	return new SourceExpressionDS_RootCall(expr, args, position);
}



SourceExpressionDS_RootCall::SourceExpressionDS_RootCall(SourceExpressionDS const & expr, std::vector<SourceExpressionDS> const & args, SourcePosition const & position) : SourceExpressionDS_Base(position), _args(args), _expr(expr)
{
	SourceVariable::VariableType const * type(_expr.getType());

	if (_args.size() != type->types.size())
		throw SourceException("incorrect arg count", getPosition(), "SourceExpressionDS_RootCall");

	for (size_t i(0); i < _args.size(); ++i)
	{
		if (_args[i].getType() != type->types[i])
			_args[i] = SourceExpressionDS::make_expression_cast(_args[i], type->types[i], getPosition());
	}
}

SourceExpressionDS_RootCall * SourceExpressionDS_RootCall::clone() const
{
	return new SourceExpressionDS_RootCall(*this);
}

char const * SourceExpressionDS_RootCall::getName() const
{
	return "SourceExpressionDS_RootCall";
}

SourceVariable::VariableType const * SourceExpressionDS_RootCall::getType() const
{
	return _expr.getType()->callType;
}

bool SourceExpressionDS_RootCall::isConstant() const
{
	return false;
}

void SourceExpressionDS_RootCall::makeObjectsGet(ObjectVector * objects) const
{
	_expr.makeObjectsCall(objects, _args);
}

void SourceExpressionDS_RootCall::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootCall(";
	SourceExpressionDS_Base::printDebug(out);
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



