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
// Object-level instruction storage.
//
//-----------------------------------------------------------------------------

#include "ObjectToken.hpp"

#include "ObjectExpression.hpp"
#include "object_io.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"

#include <algorithm>


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectToken::ObjectToken
//
ObjectToken::ObjectToken() : prev(NULL), next(NULL), code(OCODE_NONE)
{
}

//
// ObjectToken::ObjectToken
//
ObjectToken::ObjectToken
(ObjectCode _code, SourcePosition const &_pos,
 std::vector<std::string> const &_labels,
 ObjectExpression::Vector const &_args)
 : args(_args), labels(_labels), pos(_pos), prev(NULL), next(NULL), code(_code)
{
}

//
// ObjectToken::addLabel
//
void ObjectToken::addLabel(std::vector<std::string> const &_labels)
{
   std::vector<std::string>::const_iterator it;
   for (it = _labels.begin(); it != _labels.end(); ++it)
      addLabel(*it);
}

//
// ObjectToken::getArg
//
ObjectExpression::Pointer ObjectToken::getArg(bigsint index) const
{
   static ObjectExpression::Pointer expr =
      ObjectExpression::create_value_int(0, SourcePosition::builtin());

   if (index < static_cast<bigsint>(args.size()))
      return args[index];
   else
      return expr;
}

//
// ObjectToken::swapData
//
void ObjectToken::swapData(ObjectToken *token)
{
   std::swap(this->args, token->args);
   std::swap(this->code, token->code);
}

//
// override_object<ObjectToken>
//
bool override_object(ObjectToken *, ObjectToken const &)
{
   return false;
}

//
// read_object<ObjectToken>
//
void read_object(std::istream *in, ObjectToken *out)
{
   read_object(in, &out->args);
   read_object(in, &out->labels);
   read_object(in, &out->pos);
   read_object(in, &out->code);
}

//
// write_object<ObjectToken>
//
void write_object(std::ostream *out, ObjectToken const &in)
{
   write_object(out, in.args);
   write_object(out, in.labels);
   write_object(out, in.pos);
   write_object(out, in.code);
}

// EOF

