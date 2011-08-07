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

/* SourceExpression/ValueData.cpp
**
** Defines the SourceExpression_ValueData class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectVector.hpp"
#include "../print_debug.hpp"



class SourceExpression_ValueData : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueData, SourceExpression);

public:
	SourceExpression_ValueData(SourceVariable::VariableType const * type, bool garbage, SourcePosition const & position);

	virtual bool canMakeObject() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	SourceVariable::VariableType const * _type;
	bool _garbage;
};



SourceExpression::Pointer SourceExpression::create_value_data(SourceVariable::VariableType const * type, bool garbage, SourcePosition const & position)
{
	return new SourceExpression_ValueData(type, garbage, position);
}



SourceExpression_ValueData::SourceExpression_ValueData(SourceVariable::VariableType const * type, bool garbage, SourcePosition const & position) : Super(position), _type(type), _garbage(garbage)
{

}

bool SourceExpression_ValueData::canMakeObject() const
{
	return !_garbage;
}

SourceVariable::VariableType const * SourceExpression_ValueData::getType() const
{
	return _type;
}

void SourceExpression_ValueData::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	for (size_t i(_type->size()); i--;)
	{
		if (_garbage)
			objects->addToken(OCODE_PUSHNUMBER, objects->getValue(0xDEADBEEF));
		else
			objects->addTokenPushZero();
	}
}

void SourceExpression_ValueData::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_ValueData(";
	Super::printDebug(out);
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



