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
// Object to ACS translation.
//
//-----------------------------------------------------------------------------

#ifndef HPP_make_tokens__BinaryTokenACS_
#define HPP_make_tokens__BinaryTokenACS_

#include "../BinaryToken/make_tokens.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define BINTOKACS_TOKENS_MAP_ARITHMETIC() \
   CASE_REMAP(ADD32F, ADD);               \
   CASE_REMAP(ADD32I, ADD);               \
   CASE_REMAP(ADD32U, ADD);               \
                                          \
   CASE_REMAP(DIV32I, DIV);               \
                                          \
   CASE_REMAP(MOD32F, MOD);               \
   CASE_REMAP(MOD32I, MOD);               \
                                          \
   CASE_REMAP(MUL32I, MUL);               \
   CASE_REMAP(MUL32U, MUL);               \
                                          \
   CASE_REMAP(SUB32F, SUB);               \
   CASE_REMAP(SUB32I, SUB);               \
   CASE_REMAP(SUB32U, SUB)

#define BINTOKACS_TOKENS_MAP_ARITHMETIC_ALL() \
   BINTOKACS_TOKENS_MAP_ARITHMETIC();         \
   CASE_REMAP(DIV32U, DIV); /* WARNING */     \
   CASE_REMAP(MOD32U, MOD)  /* WARNING */


#define BINTOKACS_TOKENS_MAP_BITWISE()  \
   CASE_REMAP_PRE(BITWISE, AND32, AND); \
   CASE_REMAP_PRE(BITWISE, IOR32, IOR); \
   CASE_REMAP_PRE(BITWISE, LSH32, LSH); \
   CASE_REMAP_PRE(BITWISE, RSH32, RSH); \
   CASE_REMAP_PRE(BITWISE, XOR32, XOR)

#define BINTOKACS_TOKENS_MAP_BITWISE_ALL() \
   BINTOKACS_TOKENS_MAP_BITWISE()


#define BINTOKACS_TOKENS_MAP_BRANCH()          \
   CASE_REMAP_PRE(BRANCH, CASE,     CASE);     \
   CASE_REMAP_PRE(BRANCH, GOTO_IMM, GOTO_IMM); \
   CASE_REMAP_PRE(BRANCH, TRUE,     TRUE);     \
   CASE_REMAP_PRE(BRANCH, ZERO,     ZERO)

#define BINTOKACS_TOKENS_MAP_BRANCH_ALL() \
   BINTOKACS_TOKENS_MAP_BRANCH()


#define BINTOKACS_TOKENS_MAP_CMP() \
   CASE_REMAP_PRE(CMP, EQ32F, EQ); \
   CASE_REMAP_PRE(CMP, EQ32I, EQ); \
                                   \
   CASE_REMAP_PRE(CMP, GE32F, GE); \
   CASE_REMAP_PRE(CMP, GE32I, GE); \
                                   \
   CASE_REMAP_PRE(CMP, GT32F, GT); \
   CASE_REMAP_PRE(CMP, GT32I, GT); \
                                   \
   CASE_REMAP_PRE(CMP, LE32F, LE); \
   CASE_REMAP_PRE(CMP, LE32I, LE); \
                                   \
   CASE_REMAP_PRE(CMP, LT32F, LT); \
   CASE_REMAP_PRE(CMP, LT32I, LT); \
                                   \
   CASE_REMAP_PRE(CMP, NE32F, NE); \
   CASE_REMAP_PRE(CMP, NE32I, NE)

#define BINTOKACS_TOKENS_MAP_CMP_ALL()           \
   BINTOKACS_TOKENS_MAP_CMP();                   \
   CASE_REMAP_PRE(CMP, GE32U, GE); /* WARNING */ \
   CASE_REMAP_PRE(CMP, GT32U, GT); /* WARNING */ \
   CASE_REMAP_PRE(CMP, LE32U, LE); /* WARNING */ \
   CASE_REMAP_PRE(CMP, LT32U, LT)  /* WARNING */


#define BINTOKACS_TOKENS_MAP_LOGICAL()   \
   CASE_REMAP_PRE(LOGICAL, AND32F, AND); \
   CASE_REMAP_PRE(LOGICAL, AND32I, AND); \
                                         \
   CASE_REMAP_PRE(LOGICAL, IOR32F, IOR); \
   CASE_REMAP_PRE(LOGICAL, IOR32I, IOR); \
                                         \
   CASE_REMAP_PRE(LOGICAL, NOT32F, NOT); \
   CASE_REMAP_PRE(LOGICAL, NOT32I, NOT)

#define BINTOKACS_TOKENS_MAP_LOGICAL_ALL() \
   BINTOKACS_TOKENS_MAP_LOGICAL()


#define BINTOKACS_TOKENS_MAP_STACK() \
   CASE_REMAP_PRE(STACK, DROP32, DROP)

#define BINTOKACS_TOKENS_MAP_STACK_ALL() \
   BINTOKACS_TOKENS_MAP_STACK()


#define BINTOKACS_TOKENS_MAP_GET()             \
   CASE_REMAP_PRE(GET, LITERAL32F, LITERAL);   \
   CASE_REMAP_PRE(GET, LITERAL32I, LITERAL);   \
                                               \
   CASE_REMAP_PRE(GET, REGISTER32F, REGISTER); \
   CASE_REMAP_PRE(GET, REGISTER32I, REGISTER)

#define BINTOKACS_TOKENS_MAP_GET_ALL() \
   BINTOKACS_TOKENS_MAP_GET()


#define BINTOKACS_TOKENS_MAP_SET()             \
   CASE_REMAP_PRE(SET, REGISTER32F, REGISTER); \
   CASE_REMAP_PRE(SET, REGISTER32I, REGISTER)

#define BINTOKACS_TOKENS_MAP_SET_ALL() \
   BINTOKACS_TOKENS_MAP_SET()


#define BINTOKACS_TOKENS_MAP_SETOP()                 \
   CASE_REMAP_PRE(SETOP_ADD, REGISTER32F, REGISTER); \
   CASE_REMAP_PRE(SETOP_ADD, REGISTER32I, REGISTER); \
   CASE_REMAP_PRE(SETOP_ADD, REGISTER32U, REGISTER); \
                                                     \
   CASE_REMAP_PRE(SETOP_DEC, REGISTER32I, REGISTER); \
   CASE_REMAP_PRE(SETOP_DEC, REGISTER32U, REGISTER); \
                                                     \
   CASE_REMAP_PRE(SETOP_DIV, REGISTER32I, REGISTER); \
                                                     \
   CASE_REMAP_PRE(SETOP_INC, REGISTER32I, REGISTER); \
   CASE_REMAP_PRE(SETOP_INC, REGISTER32U, REGISTER); \
                                                     \
   CASE_REMAP_PRE(SETOP_MOD, REGISTER32F, REGISTER); \
   CASE_REMAP_PRE(SETOP_MOD, REGISTER32I, REGISTER); \
                                                     \
   CASE_REMAP_PRE(SETOP_MUL, REGISTER32I, REGISTER); \
   CASE_REMAP_PRE(SETOP_MUL, REGISTER32U, REGISTER); \
                                                     \
   CASE_REMAP_PRE(SETOP_SUB, REGISTER32F, REGISTER); \
   CASE_REMAP_PRE(SETOP_SUB, REGISTER32I, REGISTER); \
   CASE_REMAP_PRE(SETOP_SUB, REGISTER32U, REGISTER)

#define BINTOKACS_TOKENS_MAP_SETOP_ALL()             \
   BINTOKACS_TOKENS_MAP_SETOP();                     \
                                                     \
   CASE_REMAP_PRE(SETOP_DIV, REGISTER32U, REGISTER); \
   CASE_REMAP_PRE(SETOP_MOD, REGISTER32U, REGISTER)


#define BINTOKACS_TOKENS_MAP_MISC()         \
   CASE_REMAP_PRE(MISC, NEGATE32F, NEGATE); \
   CASE_REMAP_PRE(MISC, NEGATE32I, NEGATE)

#define BINTOKACS_TOKENS_MAP_MISC_ALL() \
   BINTOKACS_TOKENS_MAP_MISC()


#define BINTOKACS_TOKENS_MAP_ACS()             \
   CASE_MAP_ACS(GAME_GET_PLAYERCOUNT);         \
   CASE_MAP_ACS(GAME_GET_SKILL);               \
   CASE_MAP_ACS(GAME_GET_THINGCOUNT_SID);      \
   CASE_MAP_ACS(GAME_GET_THINGCOUNT_SID_IMM);  \
   CASE_MAP_ACS(GAME_GET_TIME);                \
   CASE_MAP_ACS(GAME_GET_TYPE);                \
   CASE_MAP_ACS(GET_MAPREGISTER);              \
   CASE_MAP_ACS(GET_WORLDREGISTER);            \
   CASE_MAP_ACS(LINE_CLR_SPECIAL);             \
   CASE_MAP_ACS(LINE_GET_SIDE);                \
   CASE_MAP_ACS(LTAG_SET_BLOCK);               \
   CASE_MAP_ACS(LTAG_SET_SPECIAL);             \
   CASE_MAP_ACS(LTAG_SET_TEXTURE);             \
   CASE_MAP_ACS(MISC_RANDOM);                  \
   CASE_MAP_ACS(SCRIPT_RESTART);               \
   CASE_MAP_ACS(SCRIPT_SUSPEND);               \
   CASE_MAP_ACS(SCRIPT_TERMINATE);             \
   CASE_MAP_ACS(SET_MAPREGISTER);              \
   CASE_MAP_ACS(SET_WORLDREGISTER);            \
   CASE_MAP_ACS(SETOP_ADD_MAPREGISTER);        \
   CASE_MAP_ACS(SETOP_ADD_WORLDREGISTER);      \
   CASE_MAP_ACS(SETOP_DEC_MAPREGISTER);        \
   CASE_MAP_ACS(SETOP_DEC_WORLDREGISTER);      \
   CASE_MAP_ACS(SETOP_DIV_MAPREGISTER);        \
   CASE_MAP_ACS(SETOP_DIV_WORLDREGISTER);      \
   CASE_MAP_ACS(SETOP_INC_MAPREGISTER);        \
   CASE_MAP_ACS(SETOP_INC_WORLDREGISTER);      \
   CASE_MAP_ACS(SETOP_MOD_MAPREGISTER);        \
   CASE_MAP_ACS(SETOP_MOD_WORLDREGISTER);      \
   CASE_MAP_ACS(SETOP_MUL_MAPREGISTER);        \
   CASE_MAP_ACS(SETOP_MUL_WORLDREGISTER);      \
   CASE_MAP_ACS(SETOP_SUB_MAPREGISTER);        \
   CASE_MAP_ACS(SETOP_SUB_WORLDREGISTER);      \
   CASE_MAP_ACS(SPECIAL_EXEC1);                \
   CASE_MAP_ACS(SPECIAL_EXEC1_IMM);            \
   CASE_MAP_ACS(SPECIAL_EXEC2);                \
   CASE_MAP_ACS(SPECIAL_EXEC2_IMM);            \
   CASE_MAP_ACS(SPECIAL_EXEC3);                \
   CASE_MAP_ACS(SPECIAL_EXEC3_IMM);            \
   CASE_MAP_ACS(SPECIAL_EXEC4);                \
   CASE_MAP_ACS(SPECIAL_EXEC4_IMM);            \
   CASE_MAP_ACS(SPECIAL_EXEC5);                \
   CASE_MAP_ACS(SPECIAL_EXEC5_IMM);            \
   CASE_MAP_ACS(SOUND_AMBIENT);                \
   CASE_MAP_ACS(SOUND_MTAG);                   \
   CASE_MAP_ACS(SOUND_SECTOR);                 \
   CASE_MAP_ACS(SOUND_SEQUENCE);               \
   CASE_MAP_ACS(STAG_SET_TEXTURE_CEILING);     \
   CASE_MAP_ACS(STAG_SET_TEXTURE_CEILING_IMM); \
   CASE_MAP_ACS(STAG_SET_TEXTURE_FLOOR);       \
   CASE_MAP_ACS(STAG_SET_TEXTURE_FLOOR_IMM);   \
   CASE_MAP_ACS(WAIT_POLYOBJECT);              \
   CASE_MAP_ACS(WAIT_POLYOBJECT_IMM);          \
   CASE_MAP_ACS(WAIT_SCRIPT);                  \
   CASE_MAP_ACS(WAIT_SCRIPT_IMM);              \
   CASE_MAP_ACS(WAIT_SECTOR);                  \
   CASE_MAP_ACS(WAIT_SECTOR_IMM);              \
   CASE_MAP_ACS(WAIT_TICS);                    \
   CASE_MAP_ACS(WAIT_TICS_IMM)

#define BINTOKACS_TOKENS_MAP_ACS_ALL() \
   BINTOKACS_TOKENS_MAP_ACS()


#define BINTOKACS_TOKENS_MAP_ACSP()      \
   CASE_MAP_ACSP(CHARACTER);             \
   CASE_MAP_ACSP(END);                   \
   CASE_MAP_ACSP(END_BOLD);              \
   CASE_REMAP_ACSP(NUM_DEC32I, NUM_DEC); \
   CASE_MAP_ACSP(START);                 \
   CASE_MAP_ACSP(STRING)

#define BINTOKACS_TOKENS_MAP_ACSP_ALL() \
   BINTOKACS_TOKENS_MAP_ACSP();         \
   CASE_REMAP_ACSP(NUM_DEC32U, NUM_DEC)  /* WARNING */


#define BINTOKACS_TOKENS_MAP_ALL()    \
   CASE_REMAP(NOP, NOP);              \
   BINTOKACS_TOKENS_MAP_ARITHMETIC(); \
   BINTOKACS_TOKENS_MAP_BITWISE();    \
   BINTOKACS_TOKENS_MAP_BRANCH();     \
   BINTOKACS_TOKENS_MAP_CMP();        \
   BINTOKACS_TOKENS_MAP_LOGICAL();    \
   BINTOKACS_TOKENS_MAP_STACK();      \
   BINTOKACS_TOKENS_MAP_GET();        \
   BINTOKACS_TOKENS_MAP_SET();        \
   BINTOKACS_TOKENS_MAP_SETOP();      \
   BINTOKACS_TOKENS_MAP_MISC();       \
   BINTOKACS_TOKENS_MAP_ACS();        \
   BINTOKACS_TOKENS_MAP_ACSP()

#define BINTOKACS_TOKENS_MAP_ALL_ALL()    \
   CASE_REMAP(NOP, NOP);                  \
   BINTOKACS_TOKENS_MAP_ARITHMETIC_ALL(); \
   BINTOKACS_TOKENS_MAP_BITWISE_ALL();    \
   BINTOKACS_TOKENS_MAP_BRANCH_ALL();     \
   BINTOKACS_TOKENS_MAP_CMP_ALL();        \
   BINTOKACS_TOKENS_MAP_LOGICAL_ALL();    \
   BINTOKACS_TOKENS_MAP_STACK_ALL();      \
   BINTOKACS_TOKENS_MAP_GET_ALL();        \
   BINTOKACS_TOKENS_MAP_SET_ALL();        \
   BINTOKACS_TOKENS_MAP_SETOP_ALL();      \
   BINTOKACS_TOKENS_MAP_MISC_ALL();       \
   BINTOKACS_TOKENS_MAP_ACS_ALL();        \
   BINTOKACS_TOKENS_MAP_ACSP_ALL()



#define BINTOKACS_TOKENS_TRAN_CONVERT() \
   case OCODE_CONVERT_32F_32I:          \
      args.push_back(fracbits);         \
      PUSH_TOKEN(BCODE_GET_LITERAL);    \
      PUSH_TOKEN(BCODE_BITWISE_RSH);    \
      break;                            \
                                        \
   case OCODE_CONVERT_32I_32F:          \
      args.push_back(fracbits);         \
      PUSH_TOKEN(BCODE_GET_LITERAL);    \
      PUSH_TOKEN(BCODE_BITWISE_LSH);    \
      break

#define BINTOKACS_TOKENS_TRAN_CONVERT_ALL() \
   BINTOKACS_TOKENS_TRAN_CONVERT()


#define BINTOKACS_TOKENS_TRAN_STACK()

#define BINTOKACS_TOKENS_TRAN_STACK_ALL()  \
   BINTOKACS_TOKENS_TRAN_STACK();          \
                                           \
   case OCODE_STACK_DUP32:                 \
      args.push_back(indexTemp);           \
      PUSH_TOKEN(BCODE_SET_WORLDREGISTER); \
      args.push_back(indexTemp);           \
      PUSH_TOKEN(BCODE_GET_WORLDREGISTER); \
      args.push_back(indexTemp);           \
      PUSH_TOKEN(BCODE_GET_WORLDREGISTER); \
      break


#define BINTOKACS_TOKENS_TRAN_ALL() \
   BINTOKACS_TOKENS_TRAN_CONVERT(); \
   BINTOKACS_TOKENS_TRAN_STACK()

#define BINTOKACS_TOKENS_TRAN_ALL_ALL() \
   BINTOKACS_TOKENS_TRAN_CONVERT_ALL(); \
   BINTOKACS_TOKENS_TRAN_STACK_ALL()

#endif//HPP_make_tokens__BinaryTokenACS_

