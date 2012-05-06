//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// SourceExpression handling of stackptr access.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_ValueStackptr
//
class SourceExpression_ValueStackptr : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_ValueStackptr,
      SourceExpression);

public:
   //
   // ::SourceExpression_ValueStackptr
   //
   SourceExpression_ValueStackptr(SRCEXP_EXPR_PARM) : Super(SRCEXP_EXPR_PASS)
   {
   }

   //
   // ::getType
   //
   virtual CounterReference<VariableType> getType() const
   {
      return VariableType::get_bt_uint();
   }

   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      if (dst->type == VariableData::MT_VOID) return;

      VariableType::Reference srcType = VariableType::get_bt_uint();
      bigsint                 srcSize = srcType->getSize(pos);
      VariableData::Pointer   src     = VariableData::create_stack(srcSize);

      make_objects_memcpy_prep(objects, dst, src, pos);
      objects->addToken(OCODE_ADDR_AUTO, objects->getValue(0));
      make_objects_memcpy_post(objects, dst, src, srcType, context, pos);
   }

private:
   std::string name;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_stackptr
//
SRCEXP_EXPRVAL_DEFN(, stackptr)
{
   return new SourceExpression_ValueStackptr(context, pos);
}

// EOF

