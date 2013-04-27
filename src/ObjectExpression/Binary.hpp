//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// ObjectExpression handling of binary expressions.
//
//-----------------------------------------------------------------------------

#ifndef HPP_Binary__ObjectExpression_
#define HPP_Binary__ObjectExpression_

#include "../ObjectExpression.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_Binary
//
class ObjectExpression_Binary : public ObjectExpression
{
   CounterPreambleAbstract(ObjectExpression_Binary, ObjectExpression);

public:
   bool canResolve() const;

   virtual ExpressionType getType() const;

protected:
   ObjectExpression_Binary(OBJEXP_EXPRBIN_ARGS);
   ObjectExpression_Binary(ObjectLoad &arc);

   virtual ObjectSave &save(ObjectSave &arc) const;

   ObjectExpression::Reference exprL, exprR;
};

#endif//HPP_Binary__ObjectExpression_

