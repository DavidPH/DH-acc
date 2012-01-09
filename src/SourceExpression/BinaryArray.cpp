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
// SourceExpression handling of "operator []".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



//
// SourceExpression_BinaryArray
//
class SourceExpression_BinaryArray : public SourceExpression_Binary
{
   MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryArray,
                           SourceExpression_Binary);

public:
   SourceExpression_BinaryArray(SourceExpression *exprL,
                                SourceExpression *exprR,
                                SourcePosition const &position);

   virtual bool canGetData() const;

   virtual VariableData::Pointer getData() const;

   virtual VariableType const *getType() const;

protected:
   virtual void printDebug(std::ostream *out) const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};



//
// SourceExpression::create_binary_array
//
SourceExpression::Pointer SourceExpression::
create_binary_array(SourceExpression *exprL, SourceExpression *exprR,
                    SourcePosition const &position)
{
   VariableType const *typeL = exprL->getType();
   VariableType const *typeR = exprR->getType();

   // This allows C semantics for array access. Specifically that x[y] be the
   // same as *(x+y).
   if (
      typeL->vt == VariableType::VT_POINTER ||
      typeR->vt == VariableType::VT_POINTER ||
      (typeL->vt == VariableType::VT_ARRAY && exprL->canMakeObjectsAddress()) ||
      (typeR->vt == VariableType::VT_ARRAY && exprR->canMakeObjectsAddress())
   )
   {
      return create_unary_dereference(create_binary_add(exprL, exprR, position),
                                      position);
   }

   return new SourceExpression_BinaryArray(exprL, exprR, position);
}



//
// SourceExpression_BinaryArray::SourceExpression_BinaryArray
//
SourceExpression_BinaryArray::
SourceExpression_BinaryArray(SourceExpression *_exprL,
                             SourceExpression *_exprR,
                             SourcePosition const &_position)
                             : Super(_exprL, _exprR, _position)
{
   // Can only be done for VT_ARRAY or VT_STRING.
   if (exprL->getType()->vt != VariableType::VT_ARRAY
    && exprL->getType()->vt != VariableType::VT_STRING)
      throw SourceException("expected VT_ARRAY or VT_STRING for exprL got "
                            + (std::string)make_string(exprL->getType()->vt),
                            position, getName());

   if (exprR->getType()->vt != VariableType::VT_INT)
      exprR = create_value_cast(exprR, VariableType::get_vt_int(), position);
}

//
// SourceExpression_BinaryArray::canGetData
//
bool SourceExpression_BinaryArray::canGetData() const
{
   return true;
}

//
// SourceExpression_BinaryArray::getData
//
VariableData::Pointer SourceExpression_BinaryArray::getData() const
{
   VariableData::Pointer src = exprL->getData();
   VariableType const *type = getType();
   bigsint typeSize = type->size(position);

   if (src->type != VariableData::MT_REGISTERARRAY)
      throw SourceException("cannot getData", position, getName());

   SourceExpression::Pointer offset = exprR;

   // If the type's size isn't 1, need to multiply the offset.
   if (typeSize != 1)
   {
      offset = create_binary_mul(offset, create_value_int(typeSize, position),
                                 position);
   }

   // If there is already an offset, add it.
   if (src->offsetExpr)
      offset = create_binary_add(src->offsetExpr, offset, position);

   return VariableData::create_registerarray(type->size(position),
                                             src->sectionRA, src->address,
                                             offset);
}

//
// SourceExpression_BinaryArray::getType
//
VariableType const *SourceExpression_BinaryArray::getType() const
{
   return exprL->getType()->refType;
}

//
// SourceExpression_BinaryArray::printDebug
//
void SourceExpression_BinaryArray::printDebug(std::ostream *out) const
{
   *out << "SourceExpression_BinaryArray(";
   Super::printDebug(out);
   *out << ")";
}

//
// SourceExpression_BinaryArray::virtual_makeObjects
//
void SourceExpression_BinaryArray::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (exprL->getType()->vt == VariableType::VT_STRING)
   {
      bigsint typeSize = getType()->size(position);
      bigsint sizeL = exprL->getType()->size(position);
      bigsint sizeR = exprR->getType()->size(position);

      VariableData::Pointer src = VariableData::create_stack(typeSize);

      make_objects_memcpy_prep(objects, dst, src, position);

      exprL->makeObjects(objects, VariableData::create_stack(sizeL));
      exprR->makeObjects(objects, VariableData::create_stack(sizeR));

      objects->setPosition(position);

      // 2 = arg count, 15 = native get_char
      objects->addToken(OCODE_MISC_NATIVE, objects->getValue(2),
                        objects->getValue(15));

      make_objects_memcpy_post(objects, dst, src, position);
   }
   else
   {
      VariableData::Pointer src = getData();

      make_objects_memcpy_prep(objects, dst, src, position);
      make_objects_memcpy_post(objects, dst, src, position);
   }
}



// EOF

