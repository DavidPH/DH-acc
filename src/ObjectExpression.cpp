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
// Base class for object-level expression handling.
//
//-----------------------------------------------------------------------------

#include "ObjectExpression.hpp"

#include "ACSP.hpp"
#include "BinaryTokenACS.hpp"
#include "ObjectCode.hpp"
#include "ObjectData.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bigsint ObjectExpression::address_count;

std::string ObjectExpression::filename, ObjectExpression::filename_raw;

std::set<std::string> ObjectExpression::library_table;

std::map<std::string, ObjectExpression::Pointer>
ObjectExpression::symbol_table;
std::map<std::string, ObjectExpression::ExpressionType>
ObjectExpression::symbol_type_table;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::ObjectExpression
//
ObjectExpression::ObjectExpression(SourcePosition const &_pos) : position(_pos)
{
}

//
// ObjectExpression::ObjectExpression
//
ObjectExpression::ObjectExpression(std::istream *in)
{
   read_object(in, &position);
}

//
// ObjectExpression::add_address_count
//
void ObjectExpression::add_address_count(bigsint const addressCount)
{
   address_count += addressCount;
}

//
// ObjectExpression::add_label
//
void ObjectExpression::add_label(std::string const & symbol)
{
   add_symbol(symbol, create_value_int(address_count, SourcePosition::none()));
}

//
// ObjectExpression::add_symbol
//
void ObjectExpression::add_symbol
(std::string const &symbol, ObjectExpression *value)
{
   add_symbol(symbol, value->getType());
   symbol_table[symbol] = value;
}

//
// ObjectExpression::add_symbol
//
void ObjectExpression::add_symbol
(std::string const &symbol, ExpressionType type)
{
   symbol_type_table[symbol] = type;
}

//
// ObjectExpression::do_deferred_allocation
//
void ObjectExpression::do_deferred_allocation()
{
   ObjectData_Auto::generate_symbols();
   ObjectData_Register::generate_symbols();
   // Array must be after Register.
   ObjectData_Array::generate_symbols();

   // For ACS+, all the following allocation is done by the linker.
   if (output_type == OUTPUT_ACSP) return;

   ObjectData_Function::generate_symbols();
   ObjectData_Script::generate_symbols();
   ObjectData_Static::generate_symbols();
   ObjectData_String::generate_symbols();
}

//
// ObjectExpression::get_address_count
//
bigsint ObjectExpression::get_address_count()
{
   return address_count;
}

//
// ObjectExpression::get_filename
//
std::string const &ObjectExpression::get_filename()
{
   return filename;
}

//
// ObjectExpression::get_filename_raw
//
std::string const &ObjectExpression::get_filename_raw()
{
   return filename_raw;
}

//
// ObjectExpression::get_symbol
//
ObjectExpression::Pointer ObjectExpression::
get_symbol(std::string const &symbol, SourcePosition const &pos)
{
   ObjectExpression::Pointer value = get_symbol_null(symbol);

   if (!value)
      ERROR_P("unknown symbol: %s", symbol.c_str());

   return value;
}

//
// ObjectExpression::get_symbol_null
//
ObjectExpression::Pointer ObjectExpression::
get_symbol_null(std::string const &symbol)
{
   std::map<std::string, ObjectExpression::Pointer>::iterator valueIt =
      symbol_table.find(symbol);

   return valueIt == symbol_table.end() ? NULL : valueIt->second;
}

//
// ObjectExpression::get_symbol_type
//
ObjectExpression::ExpressionType ObjectExpression::get_symbol_type
(std::string const &symbol, SourcePosition const &pos)
{
   std::map<std::string, ExpressionType>::iterator typeIt =
      symbol_type_table.find(symbol);

   if (typeIt == symbol_type_table.end())
      ERROR_P("unknown symbol: %s", symbol.c_str());

   return typeIt->second;
}

SourcePosition const &ObjectExpression::getPosition() const
{
   return position;
}

//
// ObjectExpression::iter_library
//
void ObjectExpression::iter_library
(void (*iterFunc)(std::ostream *, std::string const &), std::ostream *out)
{
   std::set<std::string>::iterator itr, end = library_table.end();

   for (itr = library_table.begin(); itr != end; ++itr)
      iterFunc(out, *itr);
}

//
// ObjectExpression::resolveElement
//
ObjectExpression::Pointer ObjectExpression::resolveElement(bigsint) const
{
   ERROR_N(position, "cannot resolve element");
}

//
// ObjectExpression::resolveFloat
//
bigreal ObjectExpression::resolveFloat() const
{
   ERROR_N(position, "cannot resolve float");
}

//
// ObjectExpression::resolveInt
//
bigsint ObjectExpression::resolveInt() const
{
   ERROR_N(position, "cannot resolve int");
}

//
// ObjectExpression::resolveMember
//
ObjectExpression::Pointer ObjectExpression::resolveMember
(std::string const &name) const
{
   ERROR_N(position, "cannot resolve member: %s", name.c_str());
}

//
// ObjectExpression::resolveOCode
//
ObjectCodeSet ObjectExpression::resolveOCode() const
{
   ERROR_N(position, "cannot resolve ocode");
}

//
// ObjectExpression::resolveSymbol
//
std::string ObjectExpression::resolveSymbol() const
{
   ERROR_N(position, "cannot resolve symbol");
}

//
// ObjectExpression::set_address_count
//
void ObjectExpression::set_address_count(bigsint addressCount)
{
   address_count = addressCount;
}

//
// ObjectExpression::set_filename
//
void ObjectExpression::set_filename(std::string const &name)
{
   filename = filename_raw = name;
}

//
// ObjectExpression::set_library
//
void ObjectExpression::set_library(std::string const &library)
{
   library_table.insert(library);
}

//
// ObjectExpression::writeACSP
//
void ObjectExpression::writeACSP(std::ostream *out) const
{
   if (canResolve())
   {
      BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_LITERAL);
      BinaryTokenACS::write_ACS0_32(out, *this);
   }
   else
      writeACSPLong(out);
}

//
// ObjectExpression::writeACSPLong
//
void ObjectExpression::writeACSPLong(std::ostream *) const
{
   ERROR_N(position, "cannot write ACS+");
}

//
// ObjectExpression::writeObject
//
void ObjectExpression::writeObject(std::ostream * out) const
{
   write_object(out, &position);
}

// EOF

