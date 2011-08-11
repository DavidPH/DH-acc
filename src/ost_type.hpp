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

/* ost_type.hpp
**
** Declares functions, variables, and enums for dealing with output, source, and
** target types.
*/

#ifndef HPP_ost_type_
#define HPP_ost_type_



enum OutputType
{
	OUTPUT_ACS0,
	OUTPUT_ACSE,
	OUTPUT_object,

	OUTPUT_UNKNOWN
};

enum SourceType
{
	SOURCE_ASMPLX,
	SOURCE_DS,
	SOURCE_object,

	SOURCE_UNKNOWN
};

enum TargetType
{
	TARGET_Hexen,
	TARGET_ZDoom,

	TARGET_UNKNOWN
};

extern OutputType output_type;
extern SourceType source_type;
extern TargetType target_type;



void ost_init();



#endif /* HPP_ost_type_ */



