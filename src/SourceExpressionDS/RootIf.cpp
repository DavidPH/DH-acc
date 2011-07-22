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

/* SourceExpressionDS/RootIf.cpp
**
** Defines the SourceExpressionDS_RootIf class and methods.
*/

#include "Base.hpp"

#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"



class SourceExpressionDS_RootIf : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootIf(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprIf, SourceContext * context, SourcePosition const & position);
	SourceExpressionDS_RootIf(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprIf, SourceExpressionDS const & exprElse, SourceContext * context, SourcePosition const & position);

	virtual SourceExpressionDS_RootIf * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionDS _exprCondition;
	SourceExpressionDS _exprIf;
	SourceExpressionDS _exprElse;

	std::string _labelIf;
	std::string _labelElse;
	std::string _labelEnd;

	bool _hasElse;
};



SourceExpressionDS SourceExpressionDS::make_expression_root_if(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprIf, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpressionDS_RootIf(exprCondition, exprIf, context, position);
}
SourceExpressionDS SourceExpressionDS::make_expression_root_if(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprIf, SourceExpressionDS const & exprElse, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpressionDS_RootIf(exprCondition, exprIf, exprElse, context, position);
}



SourceExpressionDS_RootIf::SourceExpressionDS_RootIf(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprIf, SourceContext * context, SourcePosition const & position) : SourceExpressionDS_Base(position), _exprCondition(exprCondition), _exprIf(exprIf), _hasElse(false)
{
	std::string label(context->makeLabel());

	_labelIf   = label + "_if";
	_labelEnd  = label + "_end";
}
SourceExpressionDS_RootIf::SourceExpressionDS_RootIf(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprIf, SourceExpressionDS const & exprElse, SourceContext * context, SourcePosition const & position) : SourceExpressionDS_Base(position), _exprCondition(exprCondition), _exprIf(exprIf), _exprElse(exprElse), _hasElse(true)
{
	std::string label(context->makeLabel());

	_labelIf   = label + "_if";
	_labelElse = label + "_else";
	_labelEnd  = label + "_end";
}

SourceExpressionDS_RootIf * SourceExpressionDS_RootIf::clone() const
{
	return new SourceExpressionDS_RootIf(*this);
}

char const * SourceExpressionDS_RootIf::getName() const
{
	return "SourceExpressionDS_RootIf";
}

SourceVariable::VariableType const * SourceExpressionDS_RootIf::getType() const
{
	return SourceVariable::get_VariableType(SourceVariable::VT_VOID);
}

bool SourceExpressionDS_RootIf::isConstant() const
{
	return false;
}

void SourceExpressionDS_RootIf::makeObjectsGet(ObjectVector * objects) const
{
	_exprCondition.makeObjectsGet(objects);
	objects->setPosition(getPosition());
	objects->addToken(ObjectToken::OCODE_BRANCHZERO, objects->getValue(_hasElse ? _labelElse : _labelEnd));

	objects->addLabel(_labelIf);
	_exprIf.makeObjectsGet(objects);

	if (_hasElse)
	{
		objects->addToken(ObjectToken::OCODE_BRANCH, objects->getValue(_labelEnd));
		objects->addLabel(_labelElse);
		_exprElse.makeObjectsGet(objects);
	}

	objects->addLabel(_labelEnd);
}

void SourceExpressionDS_RootIf::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootIf(";
	SourceExpressionDS_Base::printDebug(out);
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



