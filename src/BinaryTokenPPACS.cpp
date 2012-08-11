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
// PPACS (Hex++ ACS) handling.
//
//-----------------------------------------------------------------------------

#include "BinaryTokenPPACS.hpp"

#include "ACSP.hpp"
#include "ObjectExpression.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

int BinaryTokenPPACS::arg_counts[BCODE_NONE];


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenPPACS::BinaryTokenPPACS
//
BinaryTokenPPACS::
BinaryTokenPPACS(BinaryCode _code, SourcePosition const &_position,
                 std::vector<std::string> const &_labels,
                 std::vector<ObjectExpression::Pointer> const &_args)
                 : args(_args), code(_code), labels(_labels),
                   position(_position)
{
}

//
// BinaryTokenPPACS::addLabel
//
void BinaryTokenPPACS::addLabel(std::string const &label)
{
   labels.push_back(label);
}

//
// BinaryTokenPPACS::init
//
void BinaryTokenPPACS::init()
{
   BinaryTokenACS::init(arg_counts);

   #define DO_INIT(NAME,ARGC) \
   arg_counts[BCODE_##NAME] = ARGC

   // Operators
   DO_INIT(ADD_AUTO,   1);
   DO_INIT(ADD_PTR,    1);
   DO_INIT(ADD_STATIC, 1);
   DO_INIT(AND_AUTO,   1);
   DO_INIT(AND_PTR,    1);
   DO_INIT(AND_STATIC, 1);
   DO_INIT(DEC_AUTO,   1);
   DO_INIT(DEC_PTR,    1);
   DO_INIT(DEC_STATIC, 1);
   DO_INIT(DIV_STK_X,  0);
   DO_INIT(DIV_AUTO,   1);
   DO_INIT(DIV_PTR,    1);
   DO_INIT(DIV_STATIC, 1);
   DO_INIT(INC_AUTO,   1);
   DO_INIT(INC_PTR,    1);
   DO_INIT(INC_STATIC, 1);
   DO_INIT(IOR_AUTO,   1);
   DO_INIT(IOR_PTR,    1);
   DO_INIT(IOR_STATIC, 1);
   DO_INIT(LSH_AUTO,   1);
   DO_INIT(LSH_PTR,    1);
   DO_INIT(LSH_STATIC, 1);
   DO_INIT(MOD_AUTO,   1);
   DO_INIT(MOD_PTR,    1);
   DO_INIT(MOD_STATIC, 1);
   DO_INIT(MUL_STK_X,  0);
   DO_INIT(MUL_AUTO,   1);
   DO_INIT(MUL_PTR,    1);
   DO_INIT(MUL_STATIC, 1);
   DO_INIT(RSH_AUTO,   1);
   DO_INIT(RSH_PTR,    1);
   DO_INIT(RSH_STATIC, 1);
   DO_INIT(SUB_AUTO,   1);
   DO_INIT(SUB_PTR,    1);
   DO_INIT(SUB_STATIC, 1);
   DO_INIT(XOR_AUTO,   1);
   DO_INIT(XOR_PTR,    1);
   DO_INIT(XOR_STATIC, 1);

   // Jumps
   DO_INIT(JMP, 0);

   // Variable Address
   DO_INIT(GET_AUTPTR_IMM, 1);
   DO_INIT(ADD_AUTPTR,     0);
   DO_INIT(SUB_AUTPTR,     0);

   // Variable Get
   DO_INIT(GET_STATIC, 1);
   DO_INIT(GET_AUTO,   1);
   DO_INIT(GET_PTR,    1);

   // Variable Set
   DO_INIT(SET_STATIC, 1);
   DO_INIT(SET_AUTO,   1);
   DO_INIT(SET_PTR,    1);

   // ACS Extensions
   DO_INIT(MID_GET,       0);
   DO_INIT(MID_SET,       0);
   DO_INIT(THING_GET_MID, 0);
   DO_INIT(THING_SET_MID, 0);

   // ACS Printing
   DO_INIT(PRINT_END_ERROR, 0);
   DO_INIT(PRINT_END_LOG,   0);
   DO_INIT(PRINT_NUM_DEC_X, 0);

   #undef DO_INIT
}

//
// BinaryTokenPPACS::label_begin
//
BinaryTokenPPACS::label_iterator BinaryTokenPPACS::label_begin() const
{
   return labels.begin();
}

//
// BinaryTokenPPACS::label_end
//
BinaryTokenPPACS::label_iterator BinaryTokenPPACS::label_end() const
{
   return labels.end();
}

//
// BinaryTokenPPACS::size
//
size_t BinaryTokenPPACS::size() const
{
   return arg_counts[code]*4 + 4;
}

//
// BinaryTokenPPACS::writeACS0
//
void BinaryTokenPPACS::writeACS0(std::ostream *out) const
{
   BinaryTokenACS::write_ACS0_32(out, code);

   for (int i = 0; i < arg_counts[code]; ++i)
   {
      if ((size_t)i < args.size())
         BinaryTokenACS::write_ACS0_32(out, *args[i]);
      else
         BinaryTokenACS::write_ACS0_32(out, 0);
   }
}

//
// BinaryTokenPPACS::writeACSP
//
void BinaryTokenPPACS::writeACSP(std::ostream *out) const
{
   std::ostringstream token;

   for (size_t i(0); i < labels.size(); ++i)
      write_ACSP_label(out, labels[i]);

   BinaryTokenACS::write_ACS0_32(&token, code);

   for (int i = 0; i < arg_counts[code]; ++i)
   {
      if ((size_t)i < args.size())
         args[i]->writeACSP(&token);
      else
      {
         BinaryTokenACS::write_ACS0_32(&token, ACSP_EXPR_LITERAL);
         BinaryTokenACS::write_ACS0_32(&token, 0);
      }
   }

   BinaryTokenACS::write_ACS0_32(out, ACSP_TOKEN_INSTRUCTION);
   BinaryTokenACS::write_ACS0_32(out, token.str().size());
   *out << token.str();
}

//
// BinaryTokenPPACS::write_all
//
void BinaryTokenPPACS::
write_all(std::ostream *out, std::vector<BinaryTokenPPACS> const &instructions)
{
   BinaryTokenACS::output_prep(instructions);

   switch(Output)
   {
   case OUTPUT_ACS0:
      BinaryTokenACS::output_ACS0(out, instructions);
      break;

   case OUTPUT_ACSP:
      output_ACSP(out, instructions);
      break;

   default:
      ERROR_p("unknown output type");
   }
}

// EOF

