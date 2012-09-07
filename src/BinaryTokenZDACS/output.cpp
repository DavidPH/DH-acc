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
// ZDoom formats (ACSE, ACSe) output.
//
//-----------------------------------------------------------------------------

#include "../BinaryTokenZDACS.hpp"

#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_fake_ACS0;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenZDACS::output_ACSE
//
template<typename T>
void BinaryTokenZDACS::
output_ACSE(std::ostream *out, std::vector<T> const &instructions)
{
   typename std::vector<T>::const_iterator iter;

   std::ostringstream chunkout;

   // Header
   if (option_fake_ACS0)
   {
      *out << 'A' << 'C' << 'S' << '\0';
      BinaryTokenACS::write_ACS0_32(out, 0);
   }
   else
   {
      *out << 'A' << 'C' << 'S' << 'E';
      BinaryTokenACS::write_ACS0_32(out, ObjectExpression::get_address_count());
   }

   // Instructions
   for (iter = instructions.begin(); iter != instructions.end(); ++iter)
      iter->writeACS0(out);

   // AIMP - Map Array Imports
   ObjectData_Array::IterateMap(write_ACSE_array_AIMP_counter, &chunkout);
   write_ACSE_counter(&chunkout);
   ObjectData_Array::IterateMap(write_ACSE_array_AIMP, &chunkout);
   write_ACSE_chunk(out, &chunkout, "AIMP");

   // AINI - Map Array Initialization
   ObjectData_Array::IterateMap(write_ACSE_array_AINI, out);

   // ARAY - Map Array Declarations
   ObjectData_Array::IterateMap(write_ACSE_array_ARAY, &chunkout);
   write_ACSE_chunk(out, &chunkout, "ARAY");

   // ASTR - Map Array Strings
   ObjectData_Array::IterateMap(write_ACSE_array_ASTR, &chunkout);
   write_ACSE_chunk(out, &chunkout, "ASTR");

   // FNAM - Function Names
   ObjectData_Function::iterate(write_ACSE_function_FNAM, NULL);
   write_ACSE_stringtable(&chunkout, false);
   write_ACSE_chunk(out, &chunkout, "FNAM");

   // FUNC - Functions
   ObjectData_Function::iterate(write_ACSE_function_FUNC, &chunkout);
   write_ACSE_chunk(out, &chunkout, "FUNC");

   // LOAD - Load Libraries
   ObjectExpression::iter_library(write_ACSE_library, &chunkout);
   write_ACSE_chunk(out, &chunkout, "LOAD");

   // MEXP - Map Register Exports
   ObjectData_Register::iterate_map(write_ACSE_register_MEXP, NULL);
   ObjectData_Array   ::IterateMap(write_ACSE_array_MEXP,    NULL);
   write_ACSE_stringtable(&chunkout, false);
   write_ACSE_chunk(out, &chunkout, "MEXP");

   // MIMP - Map Register Imports
   ObjectData_Register::iterate_map(write_ACSE_register_MIMP, &chunkout);
   write_ACSE_chunk(out, &chunkout, "MIMP");

   // MINI - Map Register Initialization
   ObjectData_Register::iterate_map(write_ACSE_register_MINI, out);

   // MSTR - Map Register Strings
   ObjectData_Register::iterate_map(write_ACSE_register_MSTR, &chunkout);
   write_ACSE_chunk(out, &chunkout, "MSTR");

   // SPTR - Script Pointers
   ObjectData_Script::iterate(write_ACSE_script, &chunkout);
   write_ACSE_chunk(out, &chunkout, "SPTR");

   // SFLG - Script Flags
   ObjectData_Script::iterate(write_ACSE_script_SFLG, &chunkout);
   write_ACSE_chunk(out, &chunkout, "SFLG");

   // SNAM - Script Names
   ObjectData_Script::iterate(write_ACSE_script_SNAM, &chunkout);
   write_ACSE_stringtable(&chunkout, false);
   write_ACSE_chunk(out, &chunkout, "SNAM");

   // STRL - String Literals
   ObjectData_String::iterate(write_ACSE_string_STRL, NULL);
   write_ACSE_stringtable(&chunkout, true);
   write_ACSE_chunk(out, &chunkout, "STRL");

   // SVCT - Script Variable Counts
   ObjectData_Script::iterate(write_ACSE_script_SVCT, &chunkout);
   write_ACSE_chunk(out, &chunkout, "SVCT");

   // Header (really)
   if (option_fake_ACS0)
   {
      BinaryTokenACS::write_ACS0_32(out, ObjectExpression::get_address_count());
      *out << 'A' << 'C' << 'S' << 'E';

      bigsint index = out->tellp(); out->seekp(4);
      BinaryTokenACS::write_ACS0_32(out, index);
   }
}

template void BinaryTokenZDACS::output_ACSE<BinaryTokenZDACS>(std::ostream *out, std::vector<BinaryTokenZDACS> const & instructions);

// EOF

