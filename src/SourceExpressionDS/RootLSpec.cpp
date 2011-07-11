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

/* SourceExpressionDS/RootLSpec.cpp
**
** Defines the SourceExpressionDS_RootLSpec class and methods.
*/

#include "Base.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectToken.hpp"
#include "../print_debug.hpp"
#include "../SourceException.hpp"



class SourceExpressionDS_RootLSpec : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootLSpec(SourceExpressionDS const & spec, std::vector<SourceExpressionDS> const & args, SourcePosition const & position);

	virtual SourceExpressionDS_RootLSpec * clone() const;

	virtual SourceExpressionDS::ExpressionType getType() const;

	virtual bool isConstant() const;

	virtual void makeObjects(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionDS _spec;
	std::vector<SourceExpressionDS> _args;
};



SourceExpressionDS SourceExpressionDS::make_expression_root_lspec(SourceExpressionDS const & spec, std::vector<SourceExpressionDS> const & args, SourcePosition const & position)
{
	return new SourceExpressionDS_RootLSpec(spec, args, position);
}



SourceExpressionDS_RootLSpec::SourceExpressionDS_RootLSpec(SourceExpressionDS const & spec, std::vector<SourceExpressionDS> const & args, SourcePosition const & position) : SourceExpressionDS_Base(position), _spec(spec), _args(args)
{

}

SourceExpressionDS_RootLSpec * SourceExpressionDS_RootLSpec::clone() const
{
	return new SourceExpressionDS_RootLSpec(*this);
}

SourceExpressionDS::ExpressionType SourceExpressionDS_RootLSpec::getType() const
{
	return SourceExpressionDS::ET_VOID;
}

bool SourceExpressionDS_RootLSpec::isConstant() const
{
	return false;
}

void SourceExpressionDS_RootLSpec::makeObjects(std::vector<ObjectToken> * const objects) const
{
	if (_spec.isConstant())
	{
		ObjectExpression specObj(_spec.createObject());

		for (size_t i(0); i < _args.size() && i < 5; ++i)
			_args[i].makeObjects(objects);

		switch (_args.size())
		{
		case 0: objects->push_back(ObjectToken(ObjectToken::OCODE_PUSHNUMBER, getPosition(), ObjectExpression::create_value_int32(0, getPosition())));
		case 1: objects->push_back(ObjectToken(ObjectToken::OCODE_LSPEC1, getPosition(), specObj)); break;
		case 2: objects->push_back(ObjectToken(ObjectToken::OCODE_LSPEC2, getPosition(), specObj)); break;
		case 3: objects->push_back(ObjectToken(ObjectToken::OCODE_LSPEC3, getPosition(), specObj)); break;
		case 4: objects->push_back(ObjectToken(ObjectToken::OCODE_LSPEC4, getPosition(), specObj)); break;
		default:
		case 5: objects->push_back(ObjectToken(ObjectToken::OCODE_LSPEC5, getPosition(), specObj)); break;
		}
	}
	else
	{
		throw SourceException("non-constant line specials not yet supported", getPosition(), "SourceExpressionDS_RootLSpec");
	}
}

void SourceExpressionDS_RootLSpec::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootLSpec(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "spec=(";
		print_debug(out, _spec);
		*out << ")";

		*out << ", ";

		*out << "args=(";
		print_debug(out, _args);
		*out << ")";
	*out << ")";
}



