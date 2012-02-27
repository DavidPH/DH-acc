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

   // Arithmetic
   DO_INIT(ADD, 0);
   DO_INIT(DIV, 0);
   DO_INIT(MOD, 0);
   DO_INIT(MUL, 0);
   DO_INIT(SUB, 0);

   // Bitwise
   DO_INIT(BITWISE_AND, 0);
   DO_INIT(BITWISE_IOR, 0);
   DO_INIT(BITWISE_LSH, 0);
   DO_INIT(BITWISE_RSH, 0);
   DO_INIT(BITWISE_XOR, 0);

   // Branching
   DO_INIT(BRANCH_CASE,     2);
   DO_INIT(BRANCH_GOTO_IMM, 1);
   DO_INIT(BRANCH_TRUE,     1);
   DO_INIT(BRANCH_ZERO,     1);

   // Comparison
   DO_INIT(CMP_EQ, 0);
   DO_INIT(CMP_GE, 0);
   DO_INIT(CMP_GT, 0);
   DO_INIT(CMP_LE, 0);
   DO_INIT(CMP_LT, 0);
   DO_INIT(CMP_NE, 0);

   // Logical
   DO_INIT(LOGICAL_AND, 0);
   DO_INIT(LOGICAL_IOR, 0);
   DO_INIT(LOGICAL_NOT, 0);

   // Stack-ops
   DO_INIT(STACK_DROP, 0);

   // Variable Get
   DO_INIT(GET_LITERAL,  1);
   DO_INIT(GET_REGISTER, 1);

   // Variable Set
   DO_INIT(SET_REGISTER, 1);

   // Variable Set Op
   DO_INIT(SETOP_ADD_REGISTER, 1);
   DO_INIT(SETOP_DEC_REGISTER, 1);
   DO_INIT(SETOP_DIV_REGISTER, 1);
   DO_INIT(SETOP_INC_REGISTER, 1);
   DO_INIT(SETOP_MOD_REGISTER, 1);
   DO_INIT(SETOP_MUL_REGISTER, 1);
   DO_INIT(SETOP_SUB_REGISTER, 1);

   // Miscellaneous
   DO_INIT(MISC_NEGATE, 0);

   // ACS
   DO_INIT(GAME_GET_PLAYERCOUNT,         0);
   DO_INIT(GAME_GET_SKILL,               0);
   DO_INIT(GAME_GET_THINGCOUNT_SID,      0);
   DO_INIT(GAME_GET_THINGCOUNT_SID_IMM,  2);
   DO_INIT(GAME_GET_TIME,                0);
   DO_INIT(GAME_GET_TYPE,                0);
   DO_INIT(GET_MAPREGISTER,              1);
   DO_INIT(GET_WORLDREGISTER,            1);
   DO_INIT(LINE_CLR_SPECIAL,             0);
   DO_INIT(LINE_GET_SIDE,                0);
   DO_INIT(LTAG_SET_BLOCK,               0);
   DO_INIT(LTAG_SET_SPECIAL,             0);
   DO_INIT(LTAG_SET_TEXTURE,             0);
   DO_INIT(MISC_RANDOM,                  0);
   DO_INIT(MISC_RANDOM_IMM,              2);
   DO_INIT(SCRIPT_RESTART,               0);
   DO_INIT(SCRIPT_SUSPEND,               0);
   DO_INIT(SCRIPT_TERMINATE,             0);
   DO_INIT(SET_MAPREGISTER,              1);
   DO_INIT(SET_WORLDREGISTER,            1);
   DO_INIT(SETOP_ADD_MAPREGISTER,        1);
   DO_INIT(SETOP_ADD_WORLDREGISTER,      1);
   DO_INIT(SETOP_DEC_MAPREGISTER,        1);
   DO_INIT(SETOP_DEC_WORLDREGISTER,      1);
   DO_INIT(SETOP_DIV_MAPREGISTER,        1);
   DO_INIT(SETOP_DIV_WORLDREGISTER,      1);
   DO_INIT(SETOP_INC_MAPREGISTER,        1);
   DO_INIT(SETOP_INC_WORLDREGISTER,      1);
   DO_INIT(SETOP_MOD_MAPREGISTER,        1);
   DO_INIT(SETOP_MOD_WORLDREGISTER,      1);
   DO_INIT(SETOP_MUL_MAPREGISTER,        1);
   DO_INIT(SETOP_MUL_WORLDREGISTER,      1);
   DO_INIT(SETOP_SUB_MAPREGISTER,        1);
   DO_INIT(SETOP_SUB_WORLDREGISTER,      1);
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
   DO_INIT(PRINT_STRING,    0);

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

   switch (output_type)
   {
   case OUTPUT_ACS0:
      output_ACS0(out, instructions);
      break;

   default:
      throw SourceException("unknown output type", SourcePosition::none(),
                            __func__);
   }
}

// EOF

