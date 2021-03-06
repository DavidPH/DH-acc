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
   CounterPreambleNoClone(ObjectExpression_ValueSymbol, ObjectExpression);

public:
   ObjectExpression_ValueSymbol(std::string const &_value, OBJEXP_EXPR_PARM)
    : Super(OBJEXP_EXPR_PASS), value(_value) {}
   ObjectExpression_ValueSymbol(ObjectLoad &arc) : Super(arc) {arc >> value;}

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
   // save
   //
   virtual ObjectSave &save(ObjectSave &arc) const
   {
      return Super::save(arc << OT_VALUE_SYMBOL) << value;
   }

private:
   //
   // v_writeNTS0
   //
   virtual void v_writeNTS0(std::ostream *out) const
   {
      if(value[0] != '"' && value[0] != '{') *out << '$';
      *out << value << '\0';
   }

   std::string value;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::CreateValueSymbol
//
auto ObjectExpression::CreateValueSymbol(std::string const &value, OBJEXP_EXPR_ARGS) -> Reference
{
   return static_cast<Reference>(new ObjectExpression_ValueSymbol(value, pos));
}

//
// ObjectExpression::LoadValueSymbol
//
auto ObjectExpression::LoadValueSymbol(ObjectLoad &arc) -> Reference
{
   return static_cast<Reference>(new ObjectExpression_ValueSymbol(arc));
}

// EOF

