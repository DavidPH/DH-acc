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
// ACS (Hexen ACS) handling.
//
//-----------------------------------------------------------------------------

#include "BinaryTokenACS.hpp"

#include "ObjectExpression.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

int BinaryTokenACS::arg_counts[BCODE_NONE];


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenACS::BinaryTokenACS
//
BinaryTokenACS::
BinaryTokenACS(BinaryCode _code, SourcePosition const &_position,
               std::vector<std::string> const &_labels,
               std::vector<ObjectExpression::Pointer> const &_args)
               : args(_args), code(_code), labels(_labels), position(_position)
{
}

//
// BinaryTokenACS::addLabel
//
void BinaryTokenACS::addLabel(std::string const &label)
{
   labels.push_back(label);
}

//
// BinaryTokenACS::init
//
void BinaryTokenACS::init()
{
   init(arg_counts);
}

//
// BinaryTokenACS::init
//
void BinaryTokenACS::init(int *argCounts)
{
   #define DO_INIT(NAME,ARGC)\
   argCounts[BCODE_##NAME] = ARGC

   DO_INIT(NOP, 0);

   // Operators
   DO_INIT(ADD_STK,    0);
   DO_INIT(ADD_REG,    1);
   DO_INIT(ADD_MAPREG, 1);
   DO_INIT(ADD_WLDREG, 1);
   DO_INIT(AND_STK,    0);
   DO_INIT(CMP_EQ,     0);
   DO_INIT(CMP_GE,     0);
   DO_INIT(CMP_GT,     0);
   DO_INIT(CMP_LE,     0);
   DO_INIT(CMP_LT,     0);
   DO_INIT(CMP_NE,     0);
   DO_INIT(DEC_REG,    1);
   DO_INIT(DEC_MAPREG, 1);
   DO_INIT(DEC_WLDREG, 1);
   DO_INIT(DIV_STK,    0);
   DO_INIT(DIV_REG,    1);
   DO_INIT(DIV_MAPREG, 1);
   DO_INIT(DIV_WLDREG, 1);
   DO_INIT(INC_REG,    1);
   DO_INIT(INC_MAPREG, 1);
   DO_INIT(INC_WLDREG, 1);
   DO_INIT(IOR_STK,    0);
   DO_INIT(LSH_STK,    0);
   DO_INIT(MOD_STK,    0);
   DO_INIT(MOD_REG,    1);
   DO_INIT(MOD_MAPREG, 1);
   DO_INIT(MOD_WLDREG, 1);
   DO_INIT(MUL_STK,    0);
   DO_INIT(MUL_REG,    1);
   DO_INIT(MUL_MAPREG, 1);
   DO_INIT(MUL_WLDREG, 1);
   DO_INIT(NOT_STK,    0);
   DO_INIT(RSH_STK,    0);
   DO_INIT(SUB_STK,    0);
   DO_INIT(SUB_REG,    1);
   DO_INIT(SUB_MAPREG, 1);
   DO_INIT(SUB_WLDREG, 1);
   DO_INIT(XOR_STK,    0);
   DO_INIT(NEG_STK,    0);
   DO_INIT(LOGAND_STK, 0);
   DO_INIT(LOGIOR_STK, 0);

   // Jumps
   DO_INIT(JMP_HLT,     0);
   DO_INIT(JMP_IMM,     1);
   DO_INIT(JMP_NIL,     1);
   DO_INIT(JMP_RET_SCR, 0);
   DO_INIT(JMP_RST,     0);
   DO_INIT(JMP_TRU,     1);
   DO_INIT(JMP_VAL,     2);

   // Stack-ops
   DO_INIT(STK_DROP, 0);

   // Variable Get
   DO_INIT(GET_IMM,    1);
   DO_INIT(GET_REG,    1);
   DO_INIT(GET_MAPREG, 1);
   DO_INIT(GET_WLDREG, 1);

   // Variable Set
   DO_INIT(SET_REG,    1);
   DO_INIT(SET_MAPREG, 1);
   DO_INIT(SET_WLDREG, 1);

   // ACS
   DO_INIT(GAME_GET_PLAYERCOUNT,         0);
   DO_INIT(GAME_GET_SKILL,               0);
   DO_INIT(GAME_GET_THINGCOUNT_SID,      0);
   DO_INIT(GAME_GET_THINGCOUNT_SID_IMM,  2);
   DO_INIT(GAME_GET_TIME,                0);
   DO_INIT(GAME_GET_TYPE,                0);
   DO_INIT(LINE_CLR_SPECIAL,             0);
   DO_INIT(LINE_GET_SIDE,                0);
   DO_INIT(LTAG_SET_BLOCK,               0);
   DO_INIT(LTAG_SET_SPECIAL,             0);
   DO_INIT(LTAG_SET_TEXTURE,             0);
   DO_INIT(MISC_RANDOM,                  0);
   DO_INIT(MISC_RANDOM_IMM,              2);
   DO_INIT(SOUND_AMBIENT,                0);
   DO_INIT(SOUND_SECTOR,                 0);
   DO_INIT(SOUND_SEQUENCE,               0);
   DO_INIT(SOUND_MTAG,                   0);
   DO_INIT(SPECIAL_EXEC1,                1);
   DO_INIT(SPECIAL_EXEC1_IMM,            2);
   DO_INIT(SPECIAL_EXEC2,                1);
   DO_INIT(SPECIAL_EXEC2_IMM,            3);
   DO_INIT(SPECIAL_EXEC3,                1);
   DO_INIT(SPECIAL_EXEC3_IMM,            4);
   DO_INIT(SPECIAL_EXEC4,                1);
   DO_INIT(SPECIAL_EXEC4_IMM,            5);
   DO_INIT(SPECIAL_EXEC5,                1);
   DO_INIT(SPECIAL_EXEC5_IMM,            6);
   DO_INIT(STAG_SET_TEXTURE_CEILING,     0);
   DO_INIT(STAG_SET_TEXTURE_CEILING_IMM, 2);
   DO_INIT(STAG_SET_TEXTURE_FLOOR,       0);
   DO_INIT(STAG_SET_TEXTURE_FLOOR_IMM,   2);
   DO_INIT(WAIT_POLYOBJECT,              0);
   DO_INIT(WAIT_POLYOBJECT_IMM,          1);
   DO_INIT(WAIT_SCRIPT,                  0);
   DO_INIT(WAIT_SCRIPT_IMM,              1);
   DO_INIT(WAIT_STAG,                    0);
   DO_INIT(WAIT_STAG_IMM,                1);
   DO_INIT(WAIT_TICS,                    0);
   DO_INIT(WAIT_TICS_IMM,                2);

   // ACS Printing
   DO_INIT(PRINT_CHARACTER, 0);
   DO_INIT(PRINT_END,       0);
   DO_INIT(PRINT_END_BOLD,  0);
   DO_INIT(PRINT_NUM_DEC,   0);
   DO_INIT(PRINT_START,     0);
   DO_INIT(PRINT_STR,       0);

   #undef DO_INIT
}

//
// BinaryTokenACS::label_begin
//
BinaryTokenACS::label_iterator BinaryTokenACS::label_begin() const
{
   return labels.begin();
}

//
// BinaryTokenACS::label_end
//
BinaryTokenACS::label_iterator BinaryTokenACS::label_end() const
{
   return labels.end();
}

//
// BinaryTokenACS::size
//
size_t BinaryTokenACS::size() const
{
   return arg_counts[code]*4 + 4;
}

//
// BinaryTokenACS::writeACS0
//
void BinaryTokenACS::writeACS0(std::ostream *out) const
{
   write_ACS0_32(out, code);

   for (int i = 0; i < arg_counts[code]; ++i)
   {
      if ((size_t)i < args.size())
         write_ACS0_32(out, *args[i]);
      else
         write_ACS0_32(out, 0);
   }
}

//
// BinaryTokenACS::write_all
//
void BinaryTokenACS::
write_all(std::ostream *out, std::vector<BinaryTokenACS> const &instructions)
{
   output_prep(instructions);

   switch(Output)
   {
   case OUTPUT_ACS0:
      output_ACS0(out, instructions);
      break;

   default:
      Error_p("unknown output type");
   }
}

// EOF

