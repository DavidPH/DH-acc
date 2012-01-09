//-----------------------------------------------------------------------------
//
// Copyright(C) 2011 David Hill
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
// SourceExpression handling of "operator =".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



//
// SourceExpression_BinaryAssign
//
class SourceExpression_BinaryAssign : public SourceExpression_Binary
{
   MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryAssign,
                           SourceExpression_Binary);

public:
   SourceExpression_BinaryAssign(SourceExpression *exprL,
                                 SourceExpression *exprR, bool allowConst,
                                 SourcePosition const &position);

   virtual bool canMakeObject() const;

protected:
   virtual void printDebug(std::ostream *out) const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   bool allowConst;
};



//
// SourceExpression::create_binary_assign
//
SourceExpression::Pointer SourceExpression::
create_binary_assign(SourceExpression *exprL, SourceExpression *exprR,
                     SourcePosition const &position)
{
   return new SourceExpression_BinaryAssign(exprL, exprR, false, position);
}

//
// SourceExpression::create_binary_assign_const
//
SourceExpression::Pointer SourceExpression::
create_binary_assign_const(SourceExpression *exprL, SourceExpression *exprR,
                           SourcePosition const &position)
{
   return new SourceExpression_BinaryAssign(exprL, exprR, true, position);
}



//
// SourceExpression_BinaryAssign::SourceExpression_BinaryAssign
//
SourceExpression_BinaryAssign::
SourceExpression_BinaryAssign(SourceExpression *_exprL,
                              SourceExpression *_exprR, bool _allowConst,
                              SourcePosition const &_position)
                              : Super(_exprL, _exprR, NULL, _exprL->getType(),
                                      _position),
                                allowConst(_allowConst)
{
}

//
// SourceExpression_BinaryAssign::canMakeObject
//
// Nothing with side-effects can be made into an object. More specifically,
// code must be emitted even if the yielded value is unused.
//
bool SourceExpression_BinaryAssign::canMakeObject() const
{
   return false;
}

//
// SourceExpression_BinaryAssign::printDebug
//
void SourceExpression_BinaryAssign::printDebug(std::ostream *out) const
{
   *out << "SourceExpressionDS_BinaryAssign(";
   Super::printDebug(out);
   *out << ")";
}

//
// SourceExpression_BinaryAssign::virtual_makeObjects
//
void SourceExpression_BinaryAssign::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (!allowConst && !exprL->getType()->constType)
      throw SourceException("attempt to assign const type", position,
                            getName());

   if (dst->type == VariableData::MT_VOID)
   {
      exprR->makeObjects(objects, exprL->getData());
   }
   else
   {
      // TODO
      throw SourceException("Using the result of assignment not currently"
                            " supported.", position, getName());
   }
}



// EOF

