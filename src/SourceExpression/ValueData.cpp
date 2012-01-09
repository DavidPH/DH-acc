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

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../print_debug.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



class SourceExpression_ValueData : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueData, SourceExpression);

public:
	SourceExpression_ValueData(VariableType const * type, bool garbage, SourcePosition const & position);

	virtual VariableType const * getType() const;

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

	VariableType const * _type;
	bool _garbage;
};



SourceExpression::Pointer SourceExpression::create_value_data(VariableType const * type, bool garbage, SourcePosition const & position)
{
	return new SourceExpression_ValueData(type, garbage, position);
}



SourceExpression_ValueData::SourceExpression_ValueData(VariableType const * type, bool garbage, SourcePosition const & position_) : Super(position_), _type(type), _garbage(garbage)
{

}

VariableType const * SourceExpression_ValueData::getType() const
{
	return _type;
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

void SourceExpression_ValueData::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	if (dst->type == VariableData::MT_VOID) return;

	bigsint srcSize = _type->size(position);

	VariableData::Pointer src = VariableData::create_stack(srcSize);

	make_objects_memcpy_prep(objects, dst, src, position);

	for (bigsint i = srcSize; i--;)
	{
		// FIXME: Should be based on type.
		if (_garbage)
			objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0xDEADBEEF));
		else
			objects->addTokenPushZero();
	}

	make_objects_memcpy_post(objects, dst, src, position);
}


