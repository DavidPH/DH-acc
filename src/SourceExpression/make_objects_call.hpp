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
// SourceExpression handling of calling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_make_objects_call__SourceExpression_
#define HPP_make_objects_call__SourceExpression_


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define FUNCTION_DATA                                                     \
   VariableType::Vector const &callTypes = type->getTypes();              \
   bigsint callSize = 0;                                                  \
                                                                          \
   VariableType::Reference retnType = type->getReturn();                  \
   bigsint retnSize = retnType->getSize(position);                        \
                                                                          \
   VariableData::Pointer src = VariableData::create_stack(retnSize);

#define FUNCTION_PREAMBLE                                                 \
   FUNCTION_DATA                                                          \
                                                                          \
   make_objects_memcpy_prep(objects, dst, src, position);

#define FUNCTION_ARGS                                                     \
   /* Evaluate the arguments. */                                          \
   for (size_t i = 0; i < callTypes.size(); ++i)                          \
   {                                                                      \
      if (!callTypes[i])                                                  \
         throw SourceException("variadic", position, __func__);           \
                                                                          \
      callSize += callTypes[i]->getSize(position);                        \
                                                                          \
      if (i >= args.size())                                               \
         throw SourceException("bad count", position, __func__);          \
                                                                          \
      SourceExpression::Pointer arg = args[i];                            \
      VariableType::Reference argType = arg->getType();                   \
      bigsint argSize = argType->getSize(position);                       \
      VariableData::Pointer argDst = VariableData::create_stack(argSize); \
                                                                          \
      if (argType != callTypes[i])                                        \
         throw SourceException("bad type", args[i]->position, __func__);  \
                                                                          \
      arg->makeObjects(objects, argDst);                                  \
   }                                                                      \
                                                                          \
   objects->setPosition(position);

#endif//HPP_make_objects_call__SourceExpression_

