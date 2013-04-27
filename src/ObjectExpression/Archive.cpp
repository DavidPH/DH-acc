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

#include <cstring>


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::Load
//
ObjectLoad &ObjectExpression::Load(ObjectLoad &arc, ObjectVector &objects)
{
   // Format verification.
   char head[7];
   arc >> head;
   if(std::memcmp(head, "object", 7))
      throw __FILE__ ": not object";

   arc >> objects >> library_table >> symbol_table >> symbol_type_table;

   ObjectData::Array   ::Load(arc);
   ObjectData::ArrayVar::Load(arc);
   ObjectData::Auto    ::Load(arc);
   ObjectData::Function::Load(arc);
   ObjectData::Label   ::Load(arc);
   ObjectData::Register::Load(arc);
   ObjectData::Script  ::Load(arc);
   ObjectData::Static  ::Load(arc);
   ObjectData::String  ::Load(arc);

   return arc;
}

//
// ObjectExpression::LoadExpr
//
ObjectExpression::Reference ObjectExpression::LoadExpr(ObjectLoad &arc)
{
   ObjectExpression::ObjectType type;

   switch((arc >> type), type)
   {
   case OT_UNARY_ADD: return LoadUnaryAdd(arc);
   case OT_UNARY_NOT: return LoadUnaryNot(arc);
   case OT_UNARY_SUB: return LoadUnarySub(arc);

   case OT_BINARY_ADD: return LoadBinaryAdd(arc);
   case OT_BINARY_AND: return LoadBinaryAnd(arc);
   case OT_BINARY_CMP: return LoadBinaryCmp(arc);
   case OT_BINARY_DIV: return LoadBinaryDiv(arc);
   case OT_BINARY_IOR: return LoadBinaryIOr(arc);
   case OT_BINARY_LSH: return LoadBinaryLSh(arc);
   case OT_BINARY_MOD: return LoadBinaryMod(arc);
   case OT_BINARY_MUL: return LoadBinaryMul(arc);
   case OT_BINARY_RSH: return LoadBinaryRSh(arc);
   case OT_BINARY_SUB: return LoadBinarySub(arc);
   case OT_BINARY_XOR: return LoadBinaryXOr(arc);

   case OT_BRANCH_AND: return LoadBranchAnd(arc);
   case OT_BRANCH_IF:  return LoadBranchIf (arc);
   case OT_BRANCH_IOR: return LoadBranchIOr(arc);
   case OT_BRANCH_NOT: return LoadBranchNot(arc);
   case OT_BRANCH_XOR: return LoadBranchXOr(arc);

   case OT_VALUE_FIX:    return LoadValueFIX   (arc);
   case OT_VALUE_FLT:    return LoadValueFLT   (arc);
   case OT_VALUE_INT:    return LoadValueINT   (arc);
   case OT_VALUE_UNS:    return LoadValueUNS   (arc);
   case OT_VALUE_OCS:    return LoadValueOCS   (arc);
   case OT_VALUE_ARR:    return LoadValueARR   (arc);
   case OT_VALUE_CAST:   return LoadValueCast  (arc);
   case OT_VALUE_PART:   return LoadValuePart  (arc);
   case OT_VALUE_SYMBOL: return LoadValueSymbol(arc);

   case OT_NONE: throw __FILE__ ": OT_NONE";
   }

   throw __FILE__ ": NOT OT";
}

//
// ObjectExpression::Save
//
ObjectSave &ObjectExpression::Save(ObjectSave &arc, ObjectVector const &objects)
{
   arc << "object" << objects << library_table << symbol_table << symbol_type_table;

   ObjectData::Array   ::Save(arc);
   ObjectData::ArrayVar::Save(arc);
   ObjectData::Auto    ::Save(arc);
   ObjectData::Function::Save(arc);
   ObjectData::Label   ::Save(arc);
   ObjectData::Register::Save(arc);
   ObjectData::Script  ::Save(arc);
   ObjectData::Static  ::Save(arc);
   ObjectData::String  ::Save(arc);

   return arc;
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
// operator ObjectSave << ObjectExpression::ExpressionType
//
ObjectSave &operator << (ObjectSave &arc, ObjectExpression::ExpressionType const &data)
{
   return arc.saveEnum(data);
}

//
// operator ObjectSave << ObjectExpression::ObjectType
//
ObjectSave &operator << (ObjectSave &arc, ObjectExpression::ObjectType const &data)
{
   return arc.saveEnum(data);
}

//
// operator ObjectSave << ObjectExpression::Pointer
//
ObjectSave &operator << (ObjectSave &arc, ObjectExpression::Pointer const &data)
{
   if(data)
      data->save(arc);
   else
      arc << ObjectExpression::OT_NONE;

   return arc;
}

//
// operator ObjectSave << ObjectExpression::Reference
//
ObjectSave &operator << (ObjectSave &arc, ObjectExpression::Reference const &data)
{
   data->save(arc);

   return arc;
}

//
// operator ObjectLoad >> ObjectExpression::ExpressionType
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectExpression::ExpressionType &data)
{
   return arc.loadEnum(data, ObjectExpression::ET_MAP);
}

//
// operator ObjectLoad >> ObjectExpression::ObjectType
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectExpression::ObjectType &data)
{
   return arc.loadEnum(data, ObjectExpression::OT_NONE);
}

//
// operator ObjectLoad >> ObjectExpression::Pointer
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectExpression::Pointer &data)
{
   try
   {
      data = ObjectExpression::LoadExpr(arc);
   }
   catch(char const *)
   {
      data = nullptr;
   }

   return arc;
}

//
// operator ObjectLoad >> ObjectExpression::Reference
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectExpression::Reference &data)
{
   data = ObjectExpression::LoadExpr(arc);

   return arc;
}

// EOF

