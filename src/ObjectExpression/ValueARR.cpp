//-----------------------------------------------------------------------------
//
// Copyright(C) 2012-2013 David Hill
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

#include "../ObjectArchive.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValueARR
//
class ObjectExpression_ValueARR : public ObjectExpression
{
   CounterPreambleNoClone(ObjectExpression_ValueARR, ObjectExpression);

public:
   //
   // ObjectExpression_ValueARR
   //
   ObjectExpression_ValueARR(Vector const &_elems, VecStr const &_names,
                             SourcePosition const &_pos)
    : Super(_pos), elems(_elems), names(_names), type(ET_MAP)
   {
   }

   //
   // ObjectExpression_ValueARR
   //
   ObjectExpression_ValueARR(Vector const &_elems, SourcePosition const &_pos)
    : Super(_pos), elems(_elems), type(ET_ARR)
   {
   }

   //
   // ObjectExpression_ValueARR
   //
   ObjectExpression_ValueARR(ObjectArchive &arc) : Super(arc)
   {
      arc << elems << names << type;
   }

   virtual bool canResolve() const {return true;}

   //
   // expand
   //
   virtual void expand(Vector *out)
   {
      for(Vector::iterator iter = elems.begin(); iter != elems.end(); ++iter)
         (*iter)->expand(out);
   }

   //
   // expandOnce
   //
   virtual void expandOnce(Vector *out)
   {
      for(Vector::iterator iter = elems.begin(); iter != elems.end(); ++iter)
         out->push_back(*iter);
   }

   virtual ExpressionType getType() const {return type;}

   //
   // resolveARR
   //
   virtual ObjectExpression::Reference resolveARR(biguint index) const
   {
      if(index >= elems.size())
         return Super::resolveARR(index);

      return static_cast<ObjectExpression::Reference>(elems[index]);
   }

   //
   // resolveMAP
   //
   virtual ObjectExpression::Reference resolveMAP(std::string const &name) const
   {
      for(size_t i = 0; i < names.size(); ++i)
         if(names[i] == name)
            return static_cast<ObjectExpression::Reference>(elems[i]);

      return Super::resolveMAP(name);
   }

protected:
   //
   // archive
   //
   virtual ObjectArchive &archive(ObjectArchive &arc)
   {
      return Super::archive(arc << OT_VALUE_ARR) << elems << names << type;
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
// ObjectExpression::create_value_arr
//
ObjectExpression::Reference ObjectExpression::create_value_arr(
   Vector const &elems, OBJEXP_EXPR_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_ValueARR(elems, pos));
}

//
// ObjectExpression::CreateValueARR
//
ObjectExpression::Reference ObjectExpression::CreateValueARR(ObjectArchive &arc)
{
   return static_cast<Reference>(new ObjectExpression_ValueARR(arc));
}

//
// ObjectExpression::create_value_map
//
ObjectExpression::Reference ObjectExpression::create_value_map(
   Vector const &elems, VecStr const &names, OBJEXP_EXPR_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_ValueARR(elems, names, pos));
}

// EOF

