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
// Hex++ formats (ACS+) output.
//
//-----------------------------------------------------------------------------

#include "../BinaryTokenPPACS.hpp"

#include "../ACSP.hpp"
#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenPPACS::output_ACSP
//
template<typename T> void BinaryTokenPPACS::output_ACSP
(std::ostream *out, std::vector<T> const &instructions)
{
   typename std::vector<T>::const_iterator instr;

   *out << 'A' << 'C' << 'S' << '+';

   for (instr = instructions.begin(); instr != instructions.end(); ++instr)
      instr->writeACSP(out);

   ObjectData_Script::iterate    (write_ACSP_script, out);
   ObjectExpression ::iter_static(write_ACSP_static, out);
   ObjectData_String::iterate    (write_ACSP_string, out);

   BinaryTokenACS::write_ACS0_32(out, ACSP_TOKEN_EOF);
   BinaryTokenACS::write_ACS0_32(out, 0);
}

template void BinaryTokenPPACS::output_ACSP<BinaryTokenPPACS>
(std::ostream *out, std::vector<BinaryTokenPPACS> const &instructions);

// EOF

