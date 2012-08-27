//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2012 David Hill
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
// Output/source/target type handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_ost_type_
#define HPP_ost_type_


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// OutputType
//
enum OutputType
{
   OUTPUT_ACS0,
   OUTPUT_ACSE,
   OUTPUT_ACSP,
   OUTPUT_object,

   OUTPUT_UNKNOWN
};

//
// SourceType
//
enum SourceType
{
   SOURCE_ASM,
   SOURCE_C,
   SOURCE_DS,
   SOURCE_object,

   SOURCE_UNKNOWN
};

//
// TargetType
//
enum TargetType
{
   TARGET_Eternity,
   TARGET_Hexen,
   TARGET_ZDoom,

   TARGET_UNKNOWN
};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern OutputType Output;
extern SourceType Source;
extern TargetType Target, Tune;

#endif//HPP_ost_type_

