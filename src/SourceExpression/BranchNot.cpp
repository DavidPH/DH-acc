//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
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
// SourceExpression handling of "operator !".
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchNot
//
class SourceExpression_BranchNot : public SourceExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchNot,
                                   SourceExpression_Unary);

public:
   SourceExpression_BranchNot(SRCEXP_EXPRUNA_ARGS);

   //
   // ::getType
   //
   VariableType::Reference getType() const
   {
      return VariableType::get_bt_bit_hrd();
   }

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_not
//
SRCEXP_EXPRBRA_DEFN(u, not)
{
   return new SourceExpression_BranchNot(expr, context, pos);
}

//
// SourceExpression_BranchNot::SourceExpression_BranchNot
//
SourceExpression_BranchNot::SourceExpression_BranchNot(SRCEXP_EXPRUNA_PARM)
 : Super(VariableType::get_bt_bit_sft(), SRCEXP_EXPRUNA_PASS)
{
}

void SourceExpression_BranchNot::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   VariableType::Reference type = getType();
   bigsint                 size = type->getSize(pos);
   VariableData::Pointer   src  = VariableData::create_stack(size);

   objects->addToken(OCODE_LOGICAL_NOT32I);

   make_objects_memcpy_post(objects, dst, src, type, context, pos);
}

// EOF

