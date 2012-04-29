//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// ObjectExpression handling of arrays and structs.
//
//-----------------------------------------------------------------------------

#include "../ObjectExpression.hpp"

#include "../object_io.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValueCompound
//
class ObjectExpression_ValueCompound : public ObjectExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_ValueCompound,
                                   ObjectExpression);

public:
   //
   // ::ObjectExpression_ValueCompound
   //
   ObjectExpression_ValueCompound
   (Vector const &_elems, VecStr const &_names, SourcePosition const &_pos)
   : Super(_pos), elems(_elems), names(_names), type(ET_STRUCT)
   {
   }
   //
   // ::ObjectExpression_ValueCompound
   //
   ObjectExpression_ValueCompound
   (Vector const &_elems, SourcePosition const &_pos)
   : Super(_pos), elems(_elems), type(ET_ARRAY)
   {
   }

   //
   // ::ObjectExpression_ValueCompound
   //
   ObjectExpression_ValueCompound(std::istream *in) : Super(in)
   {
      read_object(in, &elems);
      read_object(in, &names);

      read_object(in, &type);
   }

   //
   // ::canResolve
   //
   virtual bool canResolve() const
   {
      return true;
   }

   //
   // ::expand
   //
   virtual void expand(Vector *out)
   {
      for (Vector::iterator iter = elems.begin(); iter != elems.end(); ++iter)
         (*iter)->expand(out);
   }

   //
   // ::expandOnce
   //
   virtual void expandOnce(Vector *out)
   {
      for (Vector::iterator iter = elems.begin(); iter != elems.end(); ++iter)
         out->push_back(*iter);
   }

   //
   // ::getType
   //
   virtual ExpressionType getType() const
   {
      return type;
   }

   //
   // ::resolveElement
   //
   virtual ObjectExpression::Pointer resolveElement(bigsint index) const
   {
      size_t i = static_cast<size_t>(index);

      if (i >= elems.size())
         return Super::resolveElement(index);

      return elems[i];
   }

   //
   // ::resolveMember
   //
   virtual ObjectExpression::Pointer resolveMember(std::string const &name) const
   {
      for (size_t i = 0; i < names.size(); ++i)
         if (names[i] == name)
            return elems[i];

      return Super::resolveMember(name);
   }

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_VALUE_COMPOUND);

      Super::writeObject(out);

      write_object(out, &elems);
      write_object(out, &names);

      write_object(out, &type);
   }

private:
   Vector elems;
   VecStr names;

   ExpressionType type;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_value_array
//
ObjectExpression::Pointer ObjectExpression::create_value_array
(Vector const &elems, SourcePosition const &position)
{
   return new ObjectExpression_ValueCompound(elems, position);
}

//
// ObjectExpression::create_value_compound
//
ObjectExpression::Pointer ObjectExpression::create_value_compound
(std::istream *in)
{
   return new ObjectExpression_ValueCompound(in);
}

//
// ObjectExpression::create_value_struct
//
ObjectExpression::Pointer ObjectExpression::create_value_struct
(Vector const &elems, VecStr const &names, SourcePosition const &position)
{
   return new ObjectExpression_ValueCompound(elems, names, position);
}

// EOF

