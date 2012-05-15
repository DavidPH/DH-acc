//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2012 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// ObjectExpression handling of unary expressions.
//
//-----------------------------------------------------------------------------

#ifndef HPP_Unary__ObjectExpression_
#define HPP_Unary__ObjectExpression_

#include "../ObjectExpression.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_Unary
//
class ObjectExpression_Unary : public ObjectExpression
{
	MAKE_ABSTRACT_COUNTER_CLASS_BASE(ObjectExpression_Unary, ObjectExpression);

public:
   virtual bool canResolve() const;

   virtual ExpressionType getType() const;

protected:
   ObjectExpression_Unary(OBJEXP_EXPRUNA_ARGS);
   ObjectExpression_Unary(std::istream *in);

   virtual void writeObject(std::ostream *out) const;

   ObjectExpression::Reference expr;
};

#endif//HPP_Unary__ObjectExpression_

