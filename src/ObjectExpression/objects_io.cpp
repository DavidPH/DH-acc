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

   ObjectData_Array::read_objects(in);
   ObjectData_Auto::read_objects(in);
   ObjectData_Function::read_objects(in);
   ObjectData_Register::read_objects(in);
   ObjectData_Script::read_objects(in);
   ObjectData_Static::read_objects(in);
   ObjectData_String::read_objects(in);
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

   ObjectData_Array::write_objects(out);
   ObjectData_Auto::write_objects(out);
   ObjectData_Function::write_objects(out);
   ObjectData_Register::write_objects(out);
   ObjectData_Script::write_objects(out);
   ObjectData_Static::write_objects(out);
   ObjectData_String::write_objects(out);
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
   ObjectExpression::ObjectType type;
   read_object(in, &type);

   switch (type)
   {
   case ObjectExpression::OT_BINARY_ADD:
      *out = ObjectExpression::create_binary_add(in);
      break;

   case ObjectExpression::OT_BINARY_AND:
      *out = ObjectExpression::create_binary_and(in);
      break;

   case ObjectExpression::OT_BINARY_DIV:
      *out = ObjectExpression::create_binary_div(in);
      break;

   case ObjectExpression::OT_BINARY_IOR:
      *out = ObjectExpression::create_binary_ior(in);
      break;

   case ObjectExpression::OT_BINARY_MOD:
      *out = ObjectExpression::create_binary_mod(in);
      break;

   case ObjectExpression::OT_BINARY_MUL:
      *out = ObjectExpression::create_binary_mul(in);
      break;

   case ObjectExpression::OT_BINARY_SUB:
      *out = ObjectExpression::create_binary_sub(in);
      break;

   case ObjectExpression::OT_BINARY_XOR:
      *out = ObjectExpression::create_binary_xor(in);
      break;

   case ObjectExpression::OT_BRANCH_AND:
      *out = ObjectExpression::create_branch_and(in);
      break;

   case ObjectExpression::OT_BRANCH_IOR:
      *out = ObjectExpression::create_branch_ior(in);
      break;

   case ObjectExpression::OT_BRANCH_NOT:
      *out = ObjectExpression::create_branch_not(in);
      break;

   case ObjectExpression::OT_BRANCH_XOR:
      *out = ObjectExpression::create_branch_xor(in);
      break;

   case ObjectExpression::OT_CAST:
      *out = ObjectExpression::create_cast(in);
      break;

   case ObjectExpression::OT_UNARY_ADD:
      *out = ObjectExpression::create_unary_add(in);
      break;

   case ObjectExpression::OT_UNARY_SUB:
      *out = ObjectExpression::create_unary_sub(in);
      break;

   case ObjectExpression::OT_VALUE_COMPOUND:
      *out = ObjectExpression::create_value_compound(in);
      break;

   case ObjectExpression::OT_VALUE_FLOAT:
      *out = ObjectExpression::create_value_float(in);
      break;

   case ObjectExpression::OT_VALUE_INT:
      *out = ObjectExpression::create_value_int(in);
      break;

   case ObjectExpression::OT_VALUE_OCODE:
      *out = ObjectExpression::create_value_ocode(in);
      break;

   case ObjectExpression::OT_VALUE_SYMBOL:
      *out = ObjectExpression::create_value_symbol(in);
      break;

   case ObjectExpression::OT_NONE:
      *out = NULL;
      break;
   }
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

// EOF

