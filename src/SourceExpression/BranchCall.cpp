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
// SourceExpression handling of "operator ()".
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchCall
//
class SourceExpression_BranchCall : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchCall,
                                   SourceExpression);

public:
   SourceExpression_BranchCall
   (SourceExpression *expr, Vector const &args, SRCEXP_EXPR_ARGS);

   virtual VariableType::Reference getType() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Vector args;
   SourceExpression::Pointer expr;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_call
//
SRCEXP_EXPRBRA_DEFN(a, call)
{
   return new SourceExpression_BranchCall(expr, args, context, pos);
}

//
// SourceExpression_BranchCall::SourceExpression_BranchCall
//
SourceExpression_BranchCall::SourceExpression_BranchCall
(SourceExpression *_expr, Vector const &_args, SRCEXP_EXPR_PARM)
 : Super(SRCEXP_EXPR_PASS), args(_args), expr(_expr)
{
   VariableType::Reference type = expr->getType();

   if(type->getBasicType() == VariableType::BT_PTR)
   {
      expr = create_unary_dereference(expr, context, pos);
      type = expr->getType();
   }

   if(!VariableType::IsTypeFunction(type->getBasicType()))
      Error_NP("expected function type");

   VariableType::Vector const &types = type->getTypes();

   for(size_t i = 0; i < types.size() && i < args.size(); ++i)
   {
      if(i < types.size() && types[i])
         args[i] = create_value_cast_implicit(args[i], types[i], context, pos);
      else
      {
         // Default argument promotions.
         VariableType::Reference argType = args[i]->getType();

         switch(argType->getBasicType())
         {
            // float becomes double.
         case VariableType::BT_FLT:
            argType = VariableType::get_bt_flt_l();
            break;

            // Integer promotions.
         case VariableType::BT_BIT_HRD:
         case VariableType::BT_BIT_SFT:
         case VariableType::BT_CHR:
         case VariableType::BT_INT_HH:
         case VariableType::BT_INT_H:
            argType = VariableType::get_bt_int();
            break;

         case VariableType::BT_UNS_HH:
         case VariableType::BT_UNS_H:
            argType = VariableType::get_bt_uns();
            break;

            // T[] becomes T *.
         case VariableType::BT_ARR:
            argType = argType->getReturn()->getPointer();
            // T __local * becomes T *.
            // T __near * becomes T *.
         case VariableType::BT_PTR:
            if(argType->getReturn()->getStoreType() == STORE_AUTO ||
               argType->getReturn()->getStoreType() == STORE_STATIC)
            {
               if(option_near_pointers)
                  argType = argType->getReturn()->setStorage(STORE_STATIC)->getPointer();
               else
                  argType = argType->getReturn()->setStorage(STORE_FAR)->getPointer();
            }
            break;

            // Everything else remains unchanged.
         default:
            break;
         }

         args[i] = create_value_cast_implicit(args[i], argType, context, pos);
      }
   }
}

//
// SourceExpression_BranchCall::getType
//
VariableType::Reference SourceExpression_BranchCall::getType() const
{
   return expr->getType()->getReturn();
}

//
// SourceExpression_BranchCall::virtual_makeObjects
//
void SourceExpression_BranchCall::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   make_objects_call(objects, dst, expr, args, context, pos);
}

// EOF

