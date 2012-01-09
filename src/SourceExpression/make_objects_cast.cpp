/* Copyright (C) 2011 David Hill
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* SourceExpression/make_objects_casp.cpp
**
** Defines the SourceExpression::make_objects_cast function.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



void SourceExpression::make_objects_memcpy_cast(ObjectVector *objects, VariableData *dst, VariableData *src, VariableType const *dstType, VariableType const *srcType, SourcePosition const &position)
{
	// Don't want to slog through all that if not throwing, but also want to
	// avoid code duplication. (Especially with the cast-to-std::string.)
	#define TYPES_STRING ((std::string)make_string(srcType->vt) + "->" + make_string(dstType->vt))

	// If no cast, this just becomes a memcpy.
	if (dstType == srcType)
	{
		// This must already have been done.
		//make_objects_memcpy_prep(objects, dst, src, position);
		make_objects_memcpy_post(objects, dst, src, position);

		return;
	}

	// If casting to void, just memcpy to void.
	if (dstType->vt == VariableType::VT_VOID)
	{
		make_objects_memcpy_void(objects, src, position);

		// If there's an offset temp, it needs to be dealt with.
		if (dst->offsetTemp)
			make_objects_memcpy_void(objects, dst->offsetTemp, position);

		return;
	}

	// All casting is stack-ops.
	VariableData::Pointer tmp = VariableData::create_stack(src->size);

	// This must already have been done.
	//make_objects_memcpy_prep(objects, dst, tmp, position);

	make_objects_memcpy_prep(objects, tmp, src, position);
	make_objects_memcpy_post(objects, tmp, src, position);

	switch (srcType->vt)
	{
	case VariableType::VT_ARRAY:
	case VariableType::VT_ASMFUNC:
	case VariableType::VT_BLOCK:
	case VariableType::VT_STRUCT:
	case VariableType::VT_UNION:
	case VariableType::VT_VOID:
		throw SourceException("invalid VT from (" + TYPES_STRING + ")", position, "SourceExpression");

	case VariableType::VT_BOOLHARD:
	case VariableType::VT_CHAR:
	case VariableType::VT_ENUM:
	case VariableType::VT_FUNCTION:
	case VariableType::VT_INT:
	case VariableType::VT_LABEL:
	case VariableType::VT_LINESPEC:
	case VariableType::VT_NATIVE:
	case VariableType::VT_POINTER:
	case VariableType::VT_SCRIPT:
	case VariableType::VT_STRING:
		switch (dstType->vt)
		{
		case VariableType::VT_ARRAY:
		case VariableType::VT_ASMFUNC:
		case VariableType::VT_BLOCK:
		case VariableType::VT_STRUCT:
		case VariableType::VT_UNION:
		case VariableType::VT_VOID:
			throw SourceException("invalid VT to (" + TYPES_STRING + ")", position, "SourceExpression");

		case VariableType::VT_BOOLHARD:
			objects->addToken(OCODE_LOGICAL_NOT32I);
			objects->addToken(OCODE_LOGICAL_NOT32I);
			break;

		case VariableType::VT_BOOLSOFT:
		case VariableType::VT_CHAR:
		case VariableType::VT_ENUM:
		case VariableType::VT_FUNCTION:
		case VariableType::VT_INT:
		case VariableType::VT_LABEL:
		case VariableType::VT_LINESPEC:
		case VariableType::VT_NATIVE:
		case VariableType::VT_POINTER:
		case VariableType::VT_SCRIPT:
		case VariableType::VT_STRING:
			break;

		case VariableType::VT_REAL:
			objects->addToken(OCODE_CONVERT_32I_32F);
			break;
		}
		break;

	case VariableType::VT_BOOLSOFT:
		switch (dstType->vt)
		{
		case VariableType::VT_ARRAY:
		case VariableType::VT_ASMFUNC:
		case VariableType::VT_BLOCK:
		case VariableType::VT_STRUCT:
		case VariableType::VT_UNION:
		case VariableType::VT_VOID:
			throw SourceException("invalid VT to (" + TYPES_STRING + ")", position, "SourceExpression");

		case VariableType::VT_BOOLHARD:
		case VariableType::VT_CHAR:
		case VariableType::VT_ENUM:
		case VariableType::VT_FUNCTION:
		case VariableType::VT_INT:
		case VariableType::VT_LABEL:
		case VariableType::VT_LINESPEC:
		case VariableType::VT_NATIVE:
		case VariableType::VT_POINTER:
		case VariableType::VT_SCRIPT:
		case VariableType::VT_STRING:
			objects->addToken(OCODE_LOGICAL_NOT32I);
			objects->addToken(OCODE_LOGICAL_NOT32I);
			break;

		case VariableType::VT_BOOLSOFT:
			break;

		case VariableType::VT_REAL:
			objects->addToken(OCODE_LOGICAL_NOT32F);
			objects->addToken(OCODE_LOGICAL_NOT32F);
			objects->addToken(OCODE_CONVERT_32I_32F);
			break;
		}
		break;

	case VariableType::VT_REAL:
		switch (dstType->vt)
		{
		case VariableType::VT_ARRAY:
		case VariableType::VT_ASMFUNC:
		case VariableType::VT_BLOCK:
		case VariableType::VT_STRUCT:
		case VariableType::VT_UNION:
		case VariableType::VT_VOID:
			throw SourceException("invalid VT to (" + TYPES_STRING + ")", position, "SourceExpression");

		case VariableType::VT_BOOLHARD:
			objects->addToken(OCODE_LOGICAL_NOT32F);
			objects->addToken(OCODE_LOGICAL_NOT32F);
			break;

		case VariableType::VT_BOOLSOFT:
		case VariableType::VT_REAL:
			break;

		case VariableType::VT_CHAR:
		case VariableType::VT_ENUM:
		case VariableType::VT_FUNCTION:
		case VariableType::VT_INT:
		case VariableType::VT_LABEL:
		case VariableType::VT_LINESPEC:
		case VariableType::VT_NATIVE:
		case VariableType::VT_POINTER:
		case VariableType::VT_SCRIPT:
		case VariableType::VT_STRING:
			objects->addToken(OCODE_CONVERT_32F_32I);
			break;
		}
		break;
	}

	make_objects_memcpy_post(objects, dst, tmp, position);

	#undef TYPES_STRING
}


