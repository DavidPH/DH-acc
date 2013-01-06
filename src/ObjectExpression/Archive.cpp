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
// ObjectExpression object output.
//
//-----------------------------------------------------------------------------

#include "../ObjectExpression.hpp"

#include "../ObjectArchive.hpp"
#include "../ObjectData.hpp"
#include "../ObjectVector.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::Archive
//
ObjectArchive &ObjectExpression::Archive(ObjectArchive &arc, ObjectVector &objects)
{
   // Format verification.
   if(arc.isSaving())
   {
      std::string head("object");
      arc << head;
   }
   else
   {
      std::string head;
      arc << head;
      if(head != "object")
         throw __FILE__ ": not object";
   }

   arc << objects << library_table << symbol_table << symbol_type_table;

   ObjectData::Array   ::Archive(arc);
   ObjectData::ArrayVar::Archive(arc);
   ObjectData::Auto    ::Archive(arc);
   ObjectData::Function::Archive(arc);
   ObjectData::Label   ::Archive(arc);
   ObjectData::Register::Archive(arc);
   ObjectData::Script  ::Archive(arc);
   ObjectData::Static  ::Archive(arc);
   ObjectData::String  ::Archive(arc);

   return arc;
}

//
// ObjectExpression::Create
//
ObjectExpression::Reference ObjectExpression::Create(ObjectArchive &arc)
{
   ObjectExpression::ObjectType type;

   switch((arc << type), type)
   {
   case OT_UNARY_ADD: return CreateUnaryAdd(arc);
   case OT_UNARY_NOT: return CreateUnaryNot(arc);
   case OT_UNARY_SUB: return CreateUnarySub(arc);

   case OT_BINARY_ADD: return CreateBinaryAdd(arc);
   case OT_BINARY_AND: return CreateBinaryAnd(arc);
   case OT_BINARY_CMP: return CreateBinaryCmp(arc);
   case OT_BINARY_DIV: return CreateBinaryDiv(arc);
   case OT_BINARY_IOR: return CreateBinaryIOr(arc);
   case OT_BINARY_LSH: return CreateBinaryLSh(arc);
   case OT_BINARY_MOD: return CreateBinaryMod(arc);
   case OT_BINARY_MUL: return CreateBinaryMul(arc);
   case OT_BINARY_RSH: return CreateBinaryRSh(arc);
   case OT_BINARY_SUB: return CreateBinarySub(arc);
   case OT_BINARY_XOR: return CreateBinaryXOr(arc);

   case OT_BRANCH_AND: return CreateBranchAnd(arc);
   case OT_BRANCH_IF:  return CreateBranchIf (arc);
   case OT_BRANCH_IOR: return CreateBranchIOr(arc);
   case OT_BRANCH_NOT: return CreateBranchNot(arc);
   case OT_BRANCH_XOR: return CreateBranchXOr(arc);

   case OT_CAST: return CreateCast(arc);

   case OT_VALUE_FIX:    return CreateValueFIX   (arc);
   case OT_VALUE_FLT:    return CreateValueFLT   (arc);
   case OT_VALUE_INT:    return CreateValueINT   (arc);
   case OT_VALUE_UNS:    return CreateValueUNS   (arc);
   case OT_VALUE_OCS:    return CreateValueOCS   (arc);
   case OT_VALUE_ARR:    return CreateValueARR   (arc);
   case OT_VALUE_SYMBOL: return CreateValueSymbol(arc);

   case OT_NONE: throw __FILE__ ": OT_NONE";
   }

   throw __FILE__ ": NOT OT";
}

//
// OA_Override<ObjectExpression::ExpressionType>
//
void OA_Override(ObjectExpression::ExpressionType &out, ObjectExpression::ExpressionType const &in)
{
   out = in;
}

//
// OA_Override<ObjectExpression::Pointer>
//
void OA_Override(ObjectExpression::Pointer &out, ObjectExpression::Pointer const &in)
{
   out = in;
}

//
// OA_Override<ObjectExpression::Reference>
//
void OA_Override(ObjectExpression::Reference &out, ObjectExpression::Reference const &in)
{
   out = in;
}

//
// operator ObjectArchive << ObjectExpression::ExpressionType
//
ObjectArchive &operator << (ObjectArchive &arc, ObjectExpression::ExpressionType &data)
{
   return arc.archiveEnum(data, ObjectExpression::ET_INT);
}

//
// operator ObjectArchive << ObjectExpression::ObjectType
//
ObjectArchive &operator << (ObjectArchive &arc, ObjectExpression::ObjectType &data)
{
   return arc.archiveEnum(data, ObjectExpression::OT_NONE);
}

//
// operator ObjectArchive << ObjectExpression::ObjectType
//
ObjectArchive &operator << (ObjectArchive &arc, ObjectExpression::ObjectType const &_data)
{
   ObjectExpression::ObjectType data = _data;
   return arc << data;
}

//
// operator ObjectArchive << ObjectExpression::Pointer
//
ObjectArchive &operator << (ObjectArchive &arc, ObjectExpression::Pointer &data)
{
   if(arc.isSaving())
   {
      if(data)
         data->archive(arc);
      else
         arc << ObjectExpression::OT_NONE;
   }
   else
   {
      try
      {
         data = ObjectExpression::Create(arc);
      }
      catch(char const *)
      {
         data = NULL;
      }
   }

   return arc;
}

//
// operator ObjectArchive << ObjectExpression::Reference
//
ObjectArchive &operator << (ObjectArchive &arc, ObjectExpression::Reference &data)
{
   if(arc.isSaving())
      data->archive(arc);
   else
      data = ObjectExpression::Create(arc);

   return arc;
}

// EOF

