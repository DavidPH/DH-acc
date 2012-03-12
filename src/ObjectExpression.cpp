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
#include "SourceException.hpp"
#include "SourceTokenC.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_dptr<bool> option_string_fold_handle
('\0', "string-fold", "optimization", "Removes duplicate strings.", NULL,
 &option_string_fold);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bigsint ObjectExpression::_address_count;

std::string ObjectExpression::_filename;

std::set<std::string> ObjectExpression::_library_table;

std::map<std::string, ObjectData_Auto> ObjectExpression::_auto_table;

std::map<std::string, ObjectData_Static> ObjectExpression::_static_table;

std::map<std::string, ObjectExpression::Pointer>        ObjectExpression::_symbol_table;
std::map<std::string, ObjectExpression::ExpressionType> ObjectExpression::_symbol_type_table;

bool option_string_fold = false;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

ObjectExpression::ObjectExpression(SourcePosition const & position_) : position(position_)
{

}
ObjectExpression::ObjectExpression(std::istream * in)
{
	read_object(in, &position);
}

void ObjectExpression::add_address_count(bigsint const addressCount)
{
	_address_count += addressCount;
}

void ObjectExpression::add_auto(std::string const & name, bigsint size, bigsint number)
{
	ObjectData_Auto s = {name, number, size};
	_auto_table[name] = s;

	add_symbol(name, create_value_int(number, SourcePosition::none()));
}

void ObjectExpression::add_label(std::string const & symbol)
{
	add_symbol(symbol, create_value_int(_address_count, SourcePosition::none()));
}

void ObjectExpression::add_static(std::string const & name, bigsint size)
{
	ObjectData_Static s = {name, -1, size};
	_static_table[name] = s;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_static(std::string const & name, bigsint size, bigsint number)
{
	ObjectData_Static s = {name, number, size};
	_static_table[name] = s;

	add_symbol(name, create_value_int(number, SourcePosition::none()));
}

void ObjectExpression::add_symbol(std::string const & symbol, ObjectExpression * value)
{
	add_symbol(symbol, value->getType());
	_symbol_table[symbol] = value;
}
void ObjectExpression::add_symbol(std::string const & symbol, ExpressionType type)
{
	_symbol_type_table[symbol] = type;
}

bigsint ObjectExpression::get_address_count()
{
	return _address_count;
}

std::string const & ObjectExpression::get_filename()
{
	return _filename;
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
      _symbol_table.find(symbol);

   return valueIt == _symbol_table.end() ? NULL : valueIt->second;
}

//
// ObjectExpression::get_symbol_type
//
ObjectExpression::ExpressionType ObjectExpression::get_symbol_type
(std::string const &symbol, SourcePosition const &pos)
{
   std::map<std::string, ExpressionType>::iterator typeIt =
      _symbol_type_table.find(symbol);

   if (typeIt == _symbol_type_table.end())
      ERROR_P("unknown symbol: %s", symbol.c_str());

   return typeIt->second;
}

SourcePosition const & ObjectExpression::getPosition() const
{
	return position;
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

void ObjectExpression::set_address_count(bigsint addressCount)
{
	_address_count = addressCount;
}

void ObjectExpression::set_filename(std::string const & filename)
{
	_filename = filename;
}

void ObjectExpression::set_library(std::string const & library)
{
	_library_table.insert(library);
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

void ObjectExpression::writeObject(std::ostream * out) const
{
	write_object(out, position);
}

// EOF

