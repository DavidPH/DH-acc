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

/* BinaryTokenACS/make_tokens.hpp
**
** Defines macros for making tokens for BinaryTokenACS.
*/

#ifndef HPP_make_tokens__BinaryTokenACS_
#define HPP_make_tokens__BinaryTokenACS_

#include "../BinaryToken/make_tokens_macros.hpp"



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


#define BINTOKACS_TOKENS_MAP_BITWISE()        \
   CASE_REMAP_PRE(BITWISE, AND32,    AND);    \
   CASE_REMAP_PRE(BITWISE, IOR32,    IOR);    \
   CASE_REMAP_PRE(BITWISE, SHIFTL32, SHIFTL); \
   CASE_REMAP_PRE(BITWISE, SHIFTR32, SHIFTR); \
   CASE_REMAP_PRE(BITWISE, XOR32,    XOR)

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


#define BINTOKACS_TOKENS_MAP_GET()                               \
   CASE_REMAP_PRE(GET, LITERAL32F, LITERAL);                     \
   CASE_REMAP_PRE(GET, LITERAL32I, LITERAL);                     \
                                                                 \
   CASE_REMAP_PRE(GET, MAPREGISTER_VAR32F, MAPREGISTER_VAR);     \
   CASE_REMAP_PRE(GET, MAPREGISTER_VAR32I, MAPREGISTER_VAR);     \
                                                                 \
   CASE_REMAP_PRE(GET, REGISTER_VAR32F, REGISTER_VAR);           \
   CASE_REMAP_PRE(GET, REGISTER_VAR32I, REGISTER_VAR);           \
                                                                 \
   CASE_REMAP_PRE(GET, WORLDREGISTER_VAR32F, WORLDREGISTER_VAR); \
   CASE_REMAP_PRE(GET, WORLDREGISTER_VAR32I, WORLDREGISTER_VAR)

#define BINTOKACS_TOKENS_MAP_GET_ALL() \
   BINTOKACS_TOKENS_MAP_GET()


#define BINTOKACS_TOKENS_MAP_SET()                               \
   CASE_REMAP_PRE(SET, MAPREGISTER_VAR32F, MAPREGISTER_VAR);     \
   CASE_REMAP_PRE(SET, MAPREGISTER_VAR32I, MAPREGISTER_VAR);     \
                                                                 \
   CASE_REMAP_PRE(SET, REGISTER_VAR32F, REGISTER_VAR);           \
   CASE_REMAP_PRE(SET, REGISTER_VAR32I, REGISTER_VAR);           \
                                                                 \
   CASE_REMAP_PRE(SET, WORLDREGISTER_VAR32F, WORLDREGISTER_VAR); \
   CASE_REMAP_PRE(SET, WORLDREGISTER_VAR32I, WORLDREGISTER_VAR)

#define BINTOKACS_TOKENS_MAP_SET_ALL() \
   BINTOKACS_TOKENS_MAP_SET()


#define BINTOKACS_TOKENS_MAP_MISC()                 \
   CASE_REMAP_PRE(MISC, DELAY32,     DELAY);        \
   CASE_REMAP_PRE(MISC, DELAY32_IMM, DELAY_IMM);    \
                                                    \
   CASE_REMAP_PRE(MISC, NEGATE32F, NEGATE);         \
   CASE_REMAP_PRE(MISC, NEGATE32I, NEGATE);         \
                                                    \
   CASE_REMAP_PRE(MISC, RANDOM32F,     RANDOM);     \
   CASE_REMAP_PRE(MISC, RANDOM32F_IMM, RANDOM_IMM); \
   CASE_REMAP_PRE(MISC, RANDOM32I,     RANDOM);     \
   CASE_REMAP_PRE(MISC, RANDOM32I_IMM, RANDOM_IMM); \

#define BINTOKACS_TOKENS_MAP_MISC_ALL()                           \
   BINTOKACS_TOKENS_MAP_MISC()                                    \
   CASE_REMAP_PRE(MISC, RANDOM32U,     RANDOM);     /* WARNING */ \
   CASE_REMAP_PRE(MISC, RANDOM32U_IMM, RANDOM_IMM)  /* WARNING */


#define BINTOKACS_TOKENS_MAP_ACS() \
   CASE_MAP_ACS(LINESPEC1);        \
   CASE_MAP_ACS(LINESPEC1_IMM);    \
   CASE_MAP_ACS(LINESPEC2);        \
   CASE_MAP_ACS(LINESPEC2_IMM);    \
   CASE_MAP_ACS(LINESPEC3);        \
   CASE_MAP_ACS(LINESPEC3_IMM);    \
   CASE_MAP_ACS(LINESPEC4);        \
   CASE_MAP_ACS(LINESPEC4_IMM);    \
   CASE_MAP_ACS(LINESPEC5);        \
   CASE_MAP_ACS(LINESPEC5_IMM);    \
   CASE_MAP_ACS(SCRIPT_RESTART);   \
   CASE_MAP_ACS(SCRIPT_SUSPEND);   \
   CASE_MAP_ACS(SCRIPT_TERMINATE); \
   CASE_MAP_ACS(THINGCOUNT);       \
   CASE_MAP_ACS(THINGCOUNT_IMM);   \
   CASE_MAP_ACS(TIMER)

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
   BINTOKACS_TOKENS_MAP_MISC_ALL();       \
   BINTOKACS_TOKENS_MAP_ACS_ALL();        \
   BINTOKACS_TOKENS_MAP_ACSP_ALL()



#define BINTOKACS_TOKENS_TRAN_CONVERT() \
   case OCODE_CONVERT_32F_32I:          \
      args.push_back(fracbits);         \
      PUSH_TOKEN(BCODE_GET_LITERAL);    \
      PUSH_TOKEN(BCODE_BITWISE_SHIFTR); \
      break;                            \
                                        \
   case OCODE_CONVERT_32I_32F:          \
      args.push_back(fracbits);         \
      PUSH_TOKEN(BCODE_GET_LITERAL);    \
      PUSH_TOKEN(BCODE_BITWISE_SHIFTL); \
      break

#define BINTOKACS_TOKENS_TRAN_CONVERT_ALL() \
   BINTOKACS_TOKENS_TRAN_CONVERT()


#define BINTOKACS_TOKENS_TRAN_ALL() \
   BINTOKACS_TOKENS_TRAN_CONVERT()

#define BINTOKACS_TOKENS_TRAN_ALL_ALL() \
   BINTOKACS_TOKENS_TRAN_CONVERT_ALL()



#endif//HPP_make_tokens__BinaryTokenACS_

