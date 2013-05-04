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
// Object-level instruction storage.
//
//-----------------------------------------------------------------------------

#ifndef HPP_ObjectToken_
#define HPP_ObjectToken_

#include "bignum.hpp"
#include "Counter.hpp"
#include "ObjectCode.hpp"
#include "SourcePosition.hpp"

#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectExpression;
class ObjectLoad;
class ObjectSave;
class SourceTokenC;

//
// ObjectToken
//
class ObjectToken
{
public:
   ObjectToken();
   ObjectToken(ObjectCode code, SourcePosition const &pos,
               std::vector<std::string> const &labels,
               std::vector<CounterPointer<ObjectExpression> > const &args);

   void addLabel(std::string const &label) {labels.push_back(label);}
   void addLabel(std::vector<std::string> const &labels);

   CounterPointer<ObjectExpression> getArg(bigsint index) const;

   // Swaps args and code.
   void swapData(ObjectToken *token);

   std::vector<CounterPointer<ObjectExpression> > args;
   std::vector<std::string> labels;
   SourcePosition pos;
   ObjectToken *prev, *next;
   ObjectCode code;


   friend ObjectSave &operator << (ObjectSave &arc, ObjectToken const &data);

   friend ObjectLoad &operator >> (ObjectLoad &arc, ObjectToken &data);
};

#endif//HPP_ObjectToken_

