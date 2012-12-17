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
// ObjectExpression object output.
//
//-----------------------------------------------------------------------------

#include "../ObjectExpression.hpp"

#include "../ObjectData.hpp"
#include "../ObjectVector.hpp"
#include "../object_io.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create
//
ObjectExpression::Reference ObjectExpression::create(std::istream *in)
{
   ObjectExpression::ObjectType type;

   switch (read_object(in, &type), type)
   {
   case ObjectExpression::OT_UNARY_ADD:
      return ObjectExpression::create_unary_add(in);

   case ObjectExpression::OT_UNARY_NOT:
      return ObjectExpression::create_unary_not(in);

   case ObjectExpression::OT_UNARY_SUB:
      return ObjectExpression::create_unary_sub(in);

   case ObjectExpression::OT_BINARY_ADD:
      return ObjectExpression::create_binary_add(in);

   case ObjectExpression::OT_BINARY_AND:
      return ObjectExpression::create_binary_and(in);

   case ObjectExpression::OT_BINARY_CMP:
      return ObjectExpression::create_binary_cmp(in);

   case ObjectExpression::OT_BINARY_DIV:
      return ObjectExpression::create_binary_div(in);

   case ObjectExpression::OT_BINARY_IOR:
      return ObjectExpression::create_binary_ior(in);

   case ObjectExpression::OT_BINARY_LSH:
      return ObjectExpression::create_binary_lsh(in);

   case ObjectExpression::OT_BINARY_MOD:
      return ObjectExpression::create_binary_mod(in);

   case ObjectExpression::OT_BINARY_MUL:
      return ObjectExpression::create_binary_mul(in);

   case ObjectExpression::OT_BINARY_RSH:
      return ObjectExpression::create_binary_rsh(in);

   case ObjectExpression::OT_BINARY_SUB:
      return ObjectExpression::create_binary_sub(in);

   case ObjectExpression::OT_BINARY_XOR:
      return ObjectExpression::create_binary_xor(in);

   case ObjectExpression::OT_BRANCH_AND:
      return ObjectExpression::create_branch_and(in);

   case ObjectExpression::OT_BRANCH_IF:
      return ObjectExpression::create_branch_if(in);

   case ObjectExpression::OT_BRANCH_IOR:
      return ObjectExpression::create_branch_ior(in);

   case ObjectExpression::OT_BRANCH_NOT:
      return ObjectExpression::create_branch_not(in);

   case ObjectExpression::OT_BRANCH_XOR:
      return ObjectExpression::create_branch_xor(in);

   case ObjectExpression::OT_CAST:
      return ObjectExpression::create_cast(in);

   case ObjectExpression::OT_VALUE_FIX:
      return ObjectExpression::create_value_fix(in);

   case ObjectExpression::OT_VALUE_FLT:
      return ObjectExpression::create_value_flt(in);

   case ObjectExpression::OT_VALUE_INT:
      return ObjectExpression::create_value_int(in);

   case ObjectExpression::OT_VALUE_UNS:
      return ObjectExpression::create_value_uns(in);

   case ObjectExpression::OT_VALUE_OCS:
      return ObjectExpression::create_value_ocs(in);

   case ObjectExpression::OT_VALUE_ARR:
      return ObjectExpression::create_value_arr(in);

   case ObjectExpression::OT_VALUE_SYMBOL:
      return ObjectExpression::create_value_symbol(in);

   case ObjectExpression::OT_NONE:
      throw "OT_NONE";
   }

   throw "NOT OT";
}

//
// ObjectExpression::read_objects
//
void ObjectExpression::read_objects(std::istream *in, ObjectVector *objects)
{
   if (in->get() != 'o' || in->get() != 'b' || in->get() != 'j' ||
       in->get() != 'e' || in->get() != 'c' || in->get() != 't')
      throw "Not object file.";

   read_object(in, objects);

   read_object(in, &library_table);

   read_object(in, &symbol_table);
   read_object(in, &symbol_type_table);

   ObjectData::Array::ReadObjects(in);
   ObjectData::ArrayVar::ReadObjects(in);
   ObjectData::Auto::ReadObjects(in);
   ObjectData::Function::ReadObjects(in);
   ObjectData::Register::ReadObjects(in);
   ObjectData::Script::ReadObjects(in);
   ObjectData::Static::ReadObjects(in);
   ObjectData::String::ReadObjects(in);
}

//
// ObjectExpression::write_objects
//
void ObjectExpression::write_objects
(std::ostream *out, ObjectVector const *objects)
{
   *out << "object";

   write_object(out, objects);

   write_object(out, &library_table);

   write_object(out, &symbol_table);
   write_object(out, &symbol_type_table);

   ObjectData::Array::WriteObjects(out);
   ObjectData::ArrayVar::WriteObjects(out);
   ObjectData::Auto::WriteObjects(out);
   ObjectData::Function::WriteObjects(out);
   ObjectData::Register::WriteObjects(out);
   ObjectData::Script::WriteObjects(out);
   ObjectData::Static::WriteObjects(out);
   ObjectData::String::WriteObjects(out);
}

//
// override_object<ObjectExpression::ExpressionType>
//
void override_object
(ObjectExpression::ExpressionType *, ObjectExpression::ExpressionType const *)
{
}

//
// override_object<ObjectExpression::Pointer>
//
void override_object
(ObjectExpression::Pointer *, ObjectExpression::Pointer const *)
{
}

//
// override_object<ObjectExpression::Reference>
//
void override_object(ObjectExpression::Reference *,
                     ObjectExpression::Reference const *)
{
}

//
// read_object<ObjectExpression::ExpressionType>
//
void read_object(std::istream *in, ObjectExpression::ExpressionType *out)
{
   *out = static_cast<ObjectExpression::ExpressionType>(read_object_int(in));

   if (*out > ObjectExpression::ET_INT)
      *out = ObjectExpression::ET_INT;
}

//
// read_object<ObjectExpression::ObjectType>
//
void read_object(std::istream *in, ObjectExpression::ObjectType *out)
{
   *out = static_cast<ObjectExpression::ObjectType>(read_object_int(in));

   if (*out > ObjectExpression::OT_NONE)
      *out = ObjectExpression::OT_NONE;
}

//
// read_object<ObjectExpression::Pointer>
//
void read_object(std::istream *in, ObjectExpression::Pointer *out)
{
   try
   {
      *out = ObjectExpression::create(in);
   }
   catch (char const *)
   {
      *out = NULL;
   }
}

//
// read_object<ObjectExpression::Reference>
//
void read_object(std::istream *in, ObjectExpression::Reference *out)
{
   *out = ObjectExpression::create(in);
}

//
// write_object<ObjectExpression::ExpressionType>
//
void write_object(std::ostream *out, ObjectExpression::ExpressionType const *in)
{
   write_object_int(out, static_cast<bigsint>(*in));
}

//
// write_object<ObjectExpression::ObjectType>
//
void write_object(std::ostream *out, ObjectExpression::ObjectType const *in)
{
   write_object_int(out, static_cast<bigsint>(*in));
}
void write_object(std::ostream *out, ObjectExpression::ObjectType const &in)
{
   write_object(out, &in);
}

//
// write_object<ObjectExpression::Pointer>
//
void write_object(std::ostream *out, ObjectExpression::Pointer const *in)
{
   if (*in)
      (*in)->writeObject(out);
   else
      write_object(out, ObjectExpression::OT_NONE);
}

//
// write_object<ObjectExpression::Reference>
//
void write_object(std::ostream *out, ObjectExpression::Reference const *in)
{
   (*in)->writeObject(out);
}

// EOF

