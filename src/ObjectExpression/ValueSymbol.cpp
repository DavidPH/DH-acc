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

/* ObjectExpression/ValueSymbol.cpp
**
** Defines the ObjectExpression_ValueSymbol class and methods.
*/

#include "../ObjectExpression.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"
#include "../BinaryTokenPPACS.hpp"
#include "../object_io.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValueSymbol
//
class ObjectExpression_ValueSymbol : public ObjectExpression
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_ValueSymbol, ObjectExpression);

public:
	ObjectExpression_ValueSymbol(std::string const & value, SourcePosition const & position);
	ObjectExpression_ValueSymbol(std::istream * in);

	virtual ExpressionType getType() const;

	virtual bigreal resolveFloat() const;
	virtual bigsint resolveInt() const;

	virtual void writeACSP(std::ostream * out) const;

protected:
	virtual void writeObject(std::ostream * out) const;

private:
	std::string _value;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_value_symbol
//
ObjectExpression::Pointer ObjectExpression::create_value_symbol(std::string const & value, SourcePosition const & position)
{
	return new ObjectExpression_ValueSymbol(value, position);
}

//
// ObjectExpression::create_value_symbol
//
ObjectExpression::Pointer ObjectExpression::create_value_symbol(std::istream * in)
{
	return new ObjectExpression_ValueSymbol(in);
}

//
// ObjectExpression_ValueSymbol::ObjectExpression_ValueSymbol
//
ObjectExpression_ValueSymbol::ObjectExpression_ValueSymbol(std::string const & value, SourcePosition const & position_) : Super(position_), _value(value)
{
}

//
// ObjectExpression_ValueSymbol::ObjectExpression_ValueSymbol
//
ObjectExpression_ValueSymbol::ObjectExpression_ValueSymbol(std::istream * in) : Super(in)
{
	read_object(in, &_value);
}

//
// ObjectExpression_ValueSymbol::getType
//
ObjectExpression::ExpressionType ObjectExpression_ValueSymbol::getType() const
{
	return ObjectExpression::get_symbol_type(_value, position);
}

//
// ObjectExpression_ValueSymbol::resolveFloat
//
bigreal ObjectExpression_ValueSymbol::resolveFloat() const
{
	return ObjectExpression::get_symbol(_value, position)->resolveFloat();
}

//
// ObjectExpression_ValueSymbol::resolveInt
//
bigsint ObjectExpression_ValueSymbol::resolveInt() const
{
	return ObjectExpression::get_symbol(_value, position)->resolveInt();
}

//
// ObjectExpression_ValueSymbol::writeACSP
//
void ObjectExpression_ValueSymbol::writeACSP(std::ostream * out) const
{
	BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_SYMBOL);
	BinaryTokenPPACS::write_ACSP_string(out, _value);
}

//
// ObjectExpression_ValueSymbol::writeObject
//
void ObjectExpression_ValueSymbol::writeObject(std::ostream * out) const
{
	write_object(out, OT_VALUE_SYMBOL);

	Super::writeObject(out);

	write_object(out, _value);
}

// EOF

