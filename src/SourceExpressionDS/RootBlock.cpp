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

/* SourceExpressionDS/RootBlock.cpp
**
** Defines the SourceExpressionDS_RootBlock class and methods.
*/

#include "Base.hpp"

#include "../ObjectVector.hpp"
#include "../print_debug.hpp"



class SourceExpressionDS_RootBlock : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootBlock(std::vector<SourceExpressionDS> const & expressions, std::vector<std::string> const & labels, SourcePosition const & position);

	virtual SourceExpressionDS_RootBlock * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual std::vector<SourceExpressionDS> getVector() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	std::vector<SourceExpressionDS> _expressions;
};



SourceExpressionDS SourceExpressionDS::make_expression_root_block(std::vector<SourceExpressionDS> const & expressions, SourcePosition const & position)
{
	return new SourceExpressionDS_RootBlock(expressions, std::vector<std::string>(), position);
}
SourceExpressionDS SourceExpressionDS::make_expression_root_block(std::vector<SourceExpressionDS> const & expressions, std::vector<std::string> const & labels, SourcePosition const & position)
{
	return new SourceExpressionDS_RootBlock(expressions, labels, position);
}



SourceExpressionDS_RootBlock::SourceExpressionDS_RootBlock(std::vector<SourceExpressionDS> const & expressions, std::vector<std::string> const & labels, SourcePosition const & position) : SourceExpressionDS_Base(labels, position), _expressions(expressions)
{

}

SourceExpressionDS_RootBlock * SourceExpressionDS_RootBlock::clone() const
{
	return new SourceExpressionDS_RootBlock(*this);
}

char const * SourceExpressionDS_RootBlock::getName() const
{
	return "SourceExpressionDS_RootBlock";
}

SourceVariable::VariableType const * SourceExpressionDS_RootBlock::getType() const
{
	return _expressions.empty() ? SourceVariable::get_VariableType(SourceVariable::VT_VOID) : _expressions.back().getType();
}

std::vector<SourceExpressionDS> SourceExpressionDS_RootBlock::getVector() const
{
	return _expressions;
}

bool SourceExpressionDS_RootBlock::isConstant() const
{
	// TODO: The block is constant if all expressions are.
	return false;
}

void SourceExpressionDS_RootBlock::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(getLabels());

	for (size_t i(0); i < _expressions.size(); ++i)
		_expressions[i].makeObjectsGet(objects);
}

void SourceExpressionDS_RootBlock::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootBlock(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "expressions=(";
		print_debug(out, _expressions);
		*out << ")";
	*out << ")";
}



