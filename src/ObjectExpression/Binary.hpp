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

/* ObjectExpression/Binary.hpp
**
** Defines the ObjectExpression_Binary class.
*/

#ifndef HPP_Binary__ObjectExpression_
#define HPP_Binary__ObjectExpression_

#include "Base.hpp"



class ObjectExpression_Binary : public ObjectExpression_Base
{
public:
	ObjectExpression_Binary(ObjectExpression const & exprL, ObjectExpression const & exprR, SourcePosition const & position);

	virtual ObjectExpression::ExpressionType getType() const;

	virtual void printDebug(std::ostream * out) const;

	virtual ObjectExpression::float_t resolveFloat() const;
	virtual ObjectExpression::int_t resolveInt() const;

protected:
	ObjectExpression _exprL;
	ObjectExpression _exprR;
};



#endif /* HPP_Binary__ObjectExpression_ */



