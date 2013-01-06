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
// ObjectExpression handling of symbols.
//
//-----------------------------------------------------------------------------

#include "../ObjectExpression.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"
#include "../BinaryTokenPPACS.hpp"
#include "../ObjectArchive.hpp"
#include "../ObjectCode.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValueSymbol
//
class ObjectExpression_ValueSymbol : public ObjectExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_ValueSymbol,
                                   ObjectExpression);

public:
   ObjectExpression_ValueSymbol(std::string const &_value, OBJEXP_EXPR_PARM)
    : Super(OBJEXP_EXPR_PASS), value(_value) {}
   ObjectExpression_ValueSymbol(ObjectArchive &arc) : Super(arc) {arc << value;}

   //
   // canResolve
   //
   virtual bool canResolve() const
   {
      ObjectExpression::Pointer symbol = ObjectExpression::get_symbol_null(value);

      return symbol && symbol->canResolve();
   }

   //
   // getType
   //
   virtual ExpressionType getType() const
   {
      return ObjectExpression::get_symbol_type(value, pos);
   }

   bigreal resolveFIX() const {return get_symbol(value, pos)->resolveFIX();}
   bigreal resolveFLT() const {return get_symbol(value, pos)->resolveFLT();}
   bigsint resolveINT() const {return get_symbol(value, pos)->resolveINT();}
   biguint resolveUNS() const {return get_symbol(value, pos)->resolveUNS();}
   ObjectCodeSet resolveOCS() const {return get_symbol(value, pos)->resolveOCS();}

   virtual std::string resolveSymbol() const {return value;}

protected:
   //
   // archive
   //
   virtual ObjectArchive &archive(ObjectArchive &arc)
   {
      return Super::archive(arc << OT_VALUE_SYMBOL) << value;
   }

private:
   //
   // writeACSPLong
   //
   virtual void writeACSPLong(std::ostream *out) const
   {
      BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_SYMBOL);
      BinaryTokenPPACS::write_ACSP_string(out, value);
   }

   std::string value;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_value_symbol
//
ObjectExpression::Reference ObjectExpression::create_value_symbol(
   std::string const &value, OBJEXP_EXPR_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_ValueSymbol(value, pos));
}

//
// ObjectExpression::CreateValueSymbol
//
ObjectExpression::Reference ObjectExpression::CreateValueSymbol(ObjectArchive &arc)
{
   return static_cast<Reference>(new ObjectExpression_ValueSymbol(arc));
}

// EOF

