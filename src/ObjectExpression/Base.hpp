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

/* ObjectExpression/Base.hpp
**
** Defines the ObjectExpression_Base class.
*/

#ifndef HPP_Base__ObjectExpression_
#define HPP_Base__ObjectExpression_

#include "../ObjectExpression.hpp"



class ObjectExpression_Base
{
public:
	ObjectExpression_Base(SourcePosition const & position);
	virtual ~ObjectExpression_Base();

	virtual ObjectExpression_Base * clone() const = 0;

	virtual char const * getName() const = 0;

	SourcePosition const & getPosition() const;

	virtual ObjectExpression::ExpressionType getType() const = 0;

	virtual void printDebug(std::ostream * out) const;

	virtual ObjectExpression::float_t resolveFloat() const = 0;
	virtual ObjectExpression::int_t resolveInt() const = 0;

private:
	SourcePosition _position;
};



#endif /* HPP_Base__ObjectExpression_ */



