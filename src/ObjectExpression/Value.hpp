//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 David Hill
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
// Common header for Value* classes.
//
//-----------------------------------------------------------------------------

#ifndef HPP_Value__ObjectExpression_
#define HPP_Value__ObjectExpression_

#include "../ObjectExpression.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"
#include "../ObjectArchive.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define CreateValueX(T, X) \
   CreateValueXPart(T, X, X##_HH) \
   CreateValueXPart(T, X, X##_H) \
   CreateValueXPart(T, X, X) \
   CreateValueXPart(T, X, X##_L) \
   CreateValueXPart(T, X, X##_LL) \
   CreateValueXArc(X)

#define CreateValueXPart(T, X, X_) \
   ObjectExpression::Reference ObjectExpression::CreateValue##X_(T value, OBJEXP_EXPR_ARGS) \
   { \
      return static_cast<Reference>(new ObjectExpression_Value##X(value, ET_##X_, pos)); \
   }

#define CreateValueXArc(X) \
   auto ObjectExpression::LoadValue##X(ObjectLoad &arc) -> Reference \
   { \
      return static_cast<Reference>(new ObjectExpression_Value##X(arc)); \
   }

#endif//HPP_Value__ObjectExpression_

