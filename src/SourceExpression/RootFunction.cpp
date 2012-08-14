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
// SourceExpression handling of the start of a script.
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
// SourceExpression_RootFunction
//
class SourceExpression_RootFunction : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_RootFunction, SourceExpression);

public:
   SourceExpression_RootFunction(VariableType *_type,
      VariableType::Vector const &_args, SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), type(_type), args(_args)
   {
   }

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      bigsint sizeAuto = 0, sizeReg = 0, sizeAll = 0;

      // TODO: Variadic functions.
      if(!args.empty() && !args.back())
         return;

      // If necessary, move any of the passed arguments.
      // TODO: QUAL_AUTOCALL
      for(VariableType::Vector::iterator itr = args.begin(),
          end = args.end(); itr != end; ++itr)
      {
         StoreType store = (*itr)->getStoreType();
         bigsint   size  = (*itr)->getSize(pos);

         if(store == STORE_REGISTER)
         {
            if(sizeReg != sizeAll) for(bigsint i = size; i--;)
            {
               objects->addToken(OCODE_GET_REG, objects->getValue(sizeAll + i));
               objects->addToken(OCODE_SET_REG, objects->getValue(sizeReg + i));
            }

            sizeReg += size;
         }
         else if(store == STORE_AUTO)
         {
            for(bigsint i = size; i--;)
            {
               objects->addToken(OCODE_GET_REG,  objects->getValue(sizeAll  + i));
               objects->addToken(OCODE_SET_AUTO, objects->getValue(sizeAuto + i));
            }

            sizeAuto += size;
         }
         else
            Error_NP("unexpected argument storage");

         sizeAll += size;
      }
   }

   VariableType::Reference type;
   VariableType::Vector args;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_root_function
//
SourceExpression::Reference SourceExpression::create_root_function(
   VariableType *type, VariableType::Vector const &args, SRCEXP_EXPR_ARGS)
{
   return static_cast<Reference>(
      new SourceExpression_RootFunction(type, args, context, pos));
}

// EOF

