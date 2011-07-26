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

/* SourceExpressionDS/RootData.cpp
**
** Defines the SourceExpressionDS_RootData class and methods.
*/

#include "Base.hpp"

#include "../ObjectVector.hpp"
#include "../print_debug.hpp"



class SourceExpressionDS_RootData : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootData(SourceVariable::VariableType const * type, bool garbage, SourcePosition const & position);

	virtual SourceExpressionDS_RootData * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceVariable::VariableType const * _type;
	bool _garbage;
};



SourceExpressionDS SourceExpressionDS::make_expression_root_data(SourceVariable::VariableType const * type, bool garbage, SourcePosition const & position)
{
	return new SourceExpressionDS_RootData(type, garbage, position);
}



SourceExpressionDS_RootData::SourceExpressionDS_RootData(SourceVariable::VariableType const * type, bool garbage, SourcePosition const & position) : SourceExpressionDS_Base(position), _type(type), _garbage(garbage)
{

}

SourceExpressionDS_RootData * SourceExpressionDS_RootData::clone() const
{
	return new SourceExpressionDS_RootData(*this);
}

char const * SourceExpressionDS_RootData::getName() const
{
	return "SourceExpressionDS_RootData";
}

SourceVariable::VariableType const * SourceExpressionDS_RootData::getType() const
{
	return _type;
}

bool SourceExpressionDS_RootData::isConstant() const
{
	return !_garbage;
}

void SourceExpressionDS_RootData::makeObjectsGet(ObjectVector * objects) const
{
	objects->setPosition(getPosition());

	for (size_t i(_type->size()); i--;)
	{
		if (_garbage)
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(0xDEADBEEF));
		else
			objects->addTokenPushZero();
	}
}

void SourceExpressionDS_RootData::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootData(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "type=(";
		print_debug(out, _type);
		*out << ")";

		*out << ", ";

		*out << "garbage=(";
		*out << _garbage;
		*out << ")";
	*out << ")";
}



