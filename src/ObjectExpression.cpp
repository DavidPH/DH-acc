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
#include "option.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static std::string et_names[] =
{
   "ET_FIX",
   "ET_FLT",
   "ET_INT",
   "ET_UNS",
   "ET_OCS",

   "ET_ARR",
   "ET_MAP",
};

static option::option_dptr<bool> option_string_tag_handler
('\0', "string-tag", "features",
 "For targets that support it, enables tagging of strings. On by default.",
 "For targets that support it, enables tagging of strings. Turning this off "
 "will very slightly improve performance, but at the cost of ACSE-library "
 "compatibility. On by default.", &option_string_tag);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bigsint ObjectExpression::address_count;

std::string ObjectExpression::filename, ObjectExpression::filename_raw;

std::set<std::string> ObjectExpression::library_table;

std::map<std::string, ObjectExpression::Pointer> ObjectExpression::symbol_table;
std::map<std::string, ObjectExpression::ExpressionType> ObjectExpression::symbol_type_table;

bool option_string_tag = true;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::ObjectExpression
//
ObjectExpression::ObjectExpression(OBJEXP_EXPR_PARM) : pos(_pos)
{
}

//
// ObjectExpression::ObjectExpression
//
ObjectExpression::ObjectExpression(std::istream *in)
{
   read_object(in, &pos);
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
   ObjectData::ArrayVar::GenerateSymbols();
   ObjectData::Auto::GenerateSymbols();
   ObjectData::Register::GenerateSymbols();
   // Array must be after Register and ArrayVar.
   ObjectData::Array::GenerateSymbols();

   // For ACS+, all the following allocation is done by the linker.
   if(Output == OUTPUT_ACSP) return;

   ObjectData::Function::GenerateSymbols();
   ObjectData::Script::GenerateSymbols();
   ObjectData::Static::GenerateSymbols();
   ObjectData::String::GenerateSymbols();
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
      Error_P("unknown symbol: %s", symbol.c_str());

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
      Error_P("unknown symbol: %s", symbol.c_str());

   return typeIt->second;
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
// ObjectExpression::resolveFIX
//
bigreal ObjectExpression::resolveFIX() const
{
   Error_NP("cannot resolve fixed for %s", make_string(getType()).c_str());
}

//
// ObjectExpression::resolveFLT
//
bigreal ObjectExpression::resolveFLT() const
{
   Error_NP("cannot resolve float for %s", make_string(getType()).c_str());
}

//
// ObjectExpression::resolveINT
//
bigsint ObjectExpression::resolveINT() const
{
   Error_NP("cannot resolve integer for %s", make_string(getType()).c_str());
}

//
// ObjectExpression::resolveUNS
//
biguint ObjectExpression::resolveUNS() const
{
   Error_NP("cannot resolve unsigned for %s", make_string(getType()).c_str());
}

//
// ObjectExpression::resolveOCS
//
ObjectCodeSet ObjectExpression::resolveOCS() const
{
   Error_NP("cannot resolve object-code for %s", make_string(getType()).c_str());
}

//
// ObjectExpression::resolveARR
//
ObjectExpression::Reference ObjectExpression::resolveARR(biguint index) const
{
   Error_NP("cannot resolve array element: %u", static_cast<unsigned int>(index));
}

//
// ObjectExpression::resolveMAP
//
ObjectExpression::Reference ObjectExpression::resolveMAP(std::string const &name) const
{
   Error_NP("cannot resolve struct element: '%s'", name.c_str());
}

//
// ObjectExpression::resolveBinary
//
biguint ObjectExpression::resolveBinary() const
{
   switch(getType())
   {
   case ET_FIX: return static_cast<bigsint>(resolveFIX() * 65536.0);
   case ET_FLT: Error_NP("TODO");
   case ET_INT: return resolveINT();
   case ET_UNS: return resolveUNS();
   case ET_OCS: Error_NP("cannot resolve raw bits of ET_OCS");

   case ET_ARR: Error_NP("cannot resolve raw bits of ET_ARR");
   case ET_MAP: Error_NP("cannot resolve raw bits of ET_MAP");
   }

   Error_NP("invalid ET");
}

//
// ObjectExpression::resolveSymbol
//
std::string ObjectExpression::resolveSymbol() const
{
   Error_NP("cannot resolve symbol");
}

//
// ObjectExpression::resolveString
//
std::string ObjectExpression::resolveString() const
{
   std::string symbol = resolveSymbol();
   ObjectData::String const *string;

   string = ObjectData::String::Find(symbol);

   if(!string)
      Error_NP("cannot resolve string");

   return string->string;
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
   // Keep the original filename for reference later.
   filename_raw = name;
   filename.resize(2 + 16);

   biguint hash = 0x0123456789ABCDEF;

   // Start by basically copying a chunk of the filename into the hash.
   // Use 7 for the shift because ASCII forever. Right, guys? ... Guys?
   for(std::string::const_iterator itr = name.begin(), end = name.end(); itr != end; ++itr)
      hash = (hash << 7) + static_cast<unsigned char>(*itr);

   // Then actually do a hash of the name.
   for(std::string::const_iterator itr = name.begin(), end = name.end(); itr != end; ++itr)
      hash = hash * 5 + static_cast<unsigned char>(*itr);

   // Then write out the hash as a 16-character hex.
   std::string::iterator str = filename.begin();

   // Start with 0x to prevent collisions with user-defined namespaces.
   *str++ = '0';
   *str++ = 'x';

   // For the record, I also considered: (hash & 0xF)["0123456789ABCDEF"]
   for(int i = 0; i < 16; ++i)
      *str++ = "0123456789ABCDEF"[hash & 0xF], hash >>= 4;
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
   Error_NP("cannot write ACS+");
}

//
// ObjectExpression::writeObject
//
void ObjectExpression::writeObject(std::ostream *out) const
{
   write_object(out, &pos);
}

//
// make_string<ObjectExpression::ExpressionType>
//
std::string const &make_string(ObjectExpression::ExpressionType et)
{
   return et_names[et];
}

// EOF

