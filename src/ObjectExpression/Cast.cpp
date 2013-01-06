//-----------------------------------------------------------------------------
//
// Copyright(C) 2012-2013 David Hill
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
// ObjectExpression handling of type-casting.
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"
#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_Cast
//
class ObjectExpression_Cast : public ObjectExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_Cast,
                                   ObjectExpression_Unary);

public:
   //
   // ObjectExpression_Cast
   //
   ObjectExpression_Cast(OBJEXP_EXPRUNA_PARM, ExpressionType _dstType,
                         ExpressionType _srcType)
     : Super(OBJEXP_EXPRUNA_PASS), dstType(_dstType), srcType(_srcType)
   {
   }

   //
   // ObjectExpression_Cast
   //
   ObjectExpression_Cast(ObjectArchive &arc) : Super(arc)
   {
      arc << dstType << srcType;
   }

   virtual bool canResolve() const {return expr->canResolve();}

   virtual ExpressionType getType() const {return dstType;}

   //
   // resolveFIX
   //
   virtual bigreal resolveFIX() const
   {
      if(dstType == ET_FIX) switch(srcType)
      {
      case ET_FIX: return expr->resolveFIX();
      case ET_FLT: return expr->resolveFLT();
      case ET_INT: return static_cast<bigreal>(expr->resolveINT());
      case ET_UNS: return static_cast<bigreal>(expr->resolveUNS());
      case ET_OCS: break;

      case ET_ARR: break;
      case ET_MAP: break;
      }

      return Super::resolveFIX();
   }

   //
   // resolveFLT
   //
   virtual bigreal resolveFLT() const
   {
      if(dstType == ET_FLT) switch(srcType)
      {
      case ET_FIX: return expr->resolveFIX();
      case ET_FLT: return expr->resolveFLT();
      case ET_INT: return static_cast<bigreal>(expr->resolveINT());
      case ET_UNS: return static_cast<bigreal>(expr->resolveUNS());
      case ET_OCS: break;

      case ET_ARR: break;
      case ET_MAP: break;
      }

      return Super::resolveFLT();
   }

   //
   // resolveINT
   //
   virtual bigsint resolveINT() const
   {
      // HACK! Allow INT for UNS.
      if(dstType == ET_INT || dstType == ET_UNS) switch(srcType)
      {
      case ET_FIX: return static_cast<bigsint>(expr->resolveFIX());
      case ET_FLT: return static_cast<bigsint>(expr->resolveFLT());
      case ET_INT: return expr->resolveINT();
      case ET_UNS: return expr->resolveUNS();
      case ET_OCS: break;

      case ET_ARR: break;
      case ET_MAP: break;
      }

      return Super::resolveINT();
   }

   //
   // resolveUNS
   //
   virtual biguint resolveUNS() const
   {
      // HACK! Allow UNS for INT.
      if(dstType == ET_UNS || dstType == ET_INT) switch(srcType)
      {
      case ET_FIX: return static_cast<bigsint>(expr->resolveFIX());
      case ET_FLT: return static_cast<bigsint>(expr->resolveFLT());
      case ET_INT: return expr->resolveINT();
      case ET_UNS: return expr->resolveUNS();
      case ET_OCS: break;

      case ET_ARR: break;
      case ET_MAP: break;
      }

      return Super::resolveUNS();
   }

protected:
   //
   // archive
   //
   virtual ObjectArchive &archive(ObjectArchive &arc)
   {
      return Super::archive(arc << OT_CAST) << dstType << srcType;
   }

private:
   //
   // writeACSPLong
   //
   virtual void writeACSPLong(std::ostream *out) const
   {
      if (dstType == srcType)
      {
         expr->writeACSP(out);
         return;
      }

      if(dstType == ET_FIX && srcType == ET_INT)
      {
         BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_F2I);
         expr->writeACSP(out);
         return;
      }

      if(dstType == ET_INT && srcType == ET_FIX)
      {
         BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_I2F);
         expr->writeACSP(out);
         return;
      }

      Error_NP("cannot write ACS+");
   }

   ExpressionType dstType;
   ExpressionType srcType;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::CreateCast
//
ObjectExpression::Reference ObjectExpression::CreateCast(ObjectArchive &arc)
{
   return static_cast<Reference>(new ObjectExpression_Cast(arc));
}

//
// ObjectExpression::create_cast_fix_to_flt
//
ObjectExpression::Reference ObjectExpression::create_cast_fix_to_flt(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_FLT, ET_FIX));
}

//
// ObjectExpression::create_cast_fix_to_int
//
ObjectExpression::Reference ObjectExpression::create_cast_fix_to_int(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_INT, ET_FIX));
}

//
// ObjectExpression::create_cast_fix_to_uns
//
ObjectExpression::Reference ObjectExpression::create_cast_fix_to_uns(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_UNS, ET_FIX));
}

//
// ObjectExpression::create_cast_flt_to_fix
//
ObjectExpression::Reference ObjectExpression::create_cast_flt_to_fix(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_FIX, ET_FLT));
}

//
// ObjectExpression::create_cast_flt_to_int
//
ObjectExpression::Reference ObjectExpression::create_cast_flt_to_int(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_INT, ET_FLT));
}

//
// ObjectExpression::create_cast_flt_to_uns
//
ObjectExpression::Reference ObjectExpression::create_cast_flt_to_uns(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_UNS, ET_FLT));
}

//
// ObjectExpression::create_cast_int_to_fix
//
ObjectExpression::Reference ObjectExpression::create_cast_int_to_fix(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_FIX, ET_INT));
}

//
// ObjectExpression::create_cast_int_to_flt
//
ObjectExpression::Reference ObjectExpression::create_cast_int_to_flt(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_FLT, ET_INT));
}

//
// ObjectExpression::create_cast_int_to_uns
//
ObjectExpression::Reference ObjectExpression::create_cast_int_to_uns(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_UNS, ET_INT));
}

//
// ObjectExpression::create_cast_uns_to_fix
//
ObjectExpression::Reference ObjectExpression::create_cast_uns_to_fix(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_FIX, ET_UNS));
}

//
// ObjectExpression::create_cast_uns_to_flt
//
ObjectExpression::Reference ObjectExpression::create_cast_uns_to_flt(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_FLT, ET_UNS));
}

//
// ObjectExpression::create_cast_uns_to_int
//
ObjectExpression::Reference ObjectExpression::create_cast_uns_to_int(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_Cast(expr, pos, ET_INT, ET_UNS));
}

// EOF

