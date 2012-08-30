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

#define BINTOKACS_TOKENS_MAP_OPER()      \
   CASE_REMAP(ADD_STK_I,    ADD_STK);    \
   CASE_REMAP(ADD_STK_U,    ADD_STK);    \
   CASE_REMAP(ADD_STK_X,    ADD_STK);    \
   CASE_REMAP(ADD_REG_I,    ADD_REG);    \
   CASE_REMAP(ADD_REG_U,    ADD_REG);    \
   CASE_REMAP(ADD_REG_X,    ADD_REG);    \
   CASE_REMAP(ADD_MAPREG_I, ADD_MAPREG); \
   CASE_REMAP(ADD_MAPREG_U, ADD_MAPREG); \
   CASE_REMAP(ADD_MAPREG_X, ADD_MAPREG); \
   CASE_REMAP(ADD_WLDREG_I, ADD_WLDREG); \
   CASE_REMAP(ADD_WLDREG_U, ADD_WLDREG); \
   CASE_REMAP(ADD_WLDREG_X, ADD_WLDREG); \
                                         \
   CASE_REMAP(AND_STK_I,    AND_STK);    \
   CASE_REMAP(AND_STK_U,    AND_STK);    \
   CASE_REMAP(AND_STK_X,    AND_STK);    \
                                         \
   CASE_REMAP(CMP_EQ_I,     CMP_EQ);     \
   CASE_REMAP(CMP_EQ_X,     CMP_EQ);     \
                                         \
   CASE_REMAP(CMP_GE_I,     CMP_GE);     \
   CASE_REMAP(CMP_GE_X,     CMP_GE);     \
                                         \
   CASE_REMAP(CMP_GT_I,     CMP_GT);     \
   CASE_REMAP(CMP_GT_X,     CMP_GT);     \
                                         \
   CASE_REMAP(CMP_LE_I,     CMP_LE);     \
   CASE_REMAP(CMP_LE_X,     CMP_LE);     \
                                         \
   CASE_REMAP(CMP_LT_I,     CMP_LT);     \
   CASE_REMAP(CMP_LT_X,     CMP_LT);     \
                                         \
   CASE_REMAP(CMP_NE_I,     CMP_NE);     \
   CASE_REMAP(CMP_NE_X,     CMP_NE);     \
                                         \
   CASE_REMAP(DEC_REG_I,    DEC_REG);    \
   CASE_REMAP(DEC_REG_U,    DEC_REG);    \
   CASE_REMAP(DEC_MAPREG_I, DEC_MAPREG); \
   CASE_REMAP(DEC_MAPREG_U, DEC_MAPREG); \
   CASE_REMAP(DEC_WLDREG_I, DEC_WLDREG); \
   CASE_REMAP(DEC_WLDREG_U, DEC_WLDREG); \
                                         \
   CASE_REMAP(DIV_STK_I,    DIV_STK);    \
   CASE_REMAP(DIV_REG_I,    DIV_REG);    \
   CASE_REMAP(DIV_MAPREG_I, DIV_MAPREG); \
   CASE_REMAP(DIV_WLDREG_I, DIV_WLDREG); \
                                         \
   CASE_REMAP(INC_REG_I,    INC_REG);    \
   CASE_REMAP(INC_REG_U,    INC_REG);    \
   CASE_REMAP(INC_MAPREG_I, INC_MAPREG); \
   CASE_REMAP(INC_MAPREG_U, INC_MAPREG); \
   CASE_REMAP(INC_WLDREG_I, INC_WLDREG); \
   CASE_REMAP(INC_WLDREG_U, INC_WLDREG); \
                                         \
   CASE_REMAP(IOR_STK_I,    IOR_STK);    \
   CASE_REMAP(IOR_STK_U,    IOR_STK);    \
   CASE_REMAP(IOR_STK_X,    IOR_STK);    \
                                         \
   CASE_REMAP(LSH_STK_I,    LSH_STK);    \
   CASE_REMAP(LSH_STK_U,    LSH_STK);    \
   CASE_REMAP(LSH_STK_X,    LSH_STK);    \
                                         \
   CASE_REMAP(MOD_STK_I,    MOD_STK);    \
   CASE_REMAP(MOD_STK_X,    MOD_STK);    \
   CASE_REMAP(MOD_REG_I,    MOD_REG);    \
   CASE_REMAP(MOD_REG_X,    MOD_REG);    \
   CASE_REMAP(MOD_MAPREG_I, MOD_MAPREG); \
   CASE_REMAP(MOD_MAPREG_X, MOD_MAPREG); \
   CASE_REMAP(MOD_WLDREG_I, MOD_WLDREG); \
   CASE_REMAP(MOD_WLDREG_X, MOD_WLDREG); \
                                         \
   CASE_REMAP(MUL_STK_I,    MUL_STK);    \
   CASE_REMAP(MUL_STK_U,    MUL_STK);    \
   CASE_REMAP(MUL_REG_I,    MUL_REG);    \
   CASE_REMAP(MUL_REG_U,    MUL_REG);    \
   CASE_REMAP(MUL_MAPREG_I, MUL_MAPREG); \
   CASE_REMAP(MUL_MAPREG_U, MUL_MAPREG); \
   CASE_REMAP(MUL_WLDREG_I, MUL_WLDREG); \
   CASE_REMAP(MUL_WLDREG_U, MUL_WLDREG); \
                                         \
   CASE_REMAP(RSH_STK_I,    RSH_STK);    \
   CASE_REMAP(RSH_STK_X,    RSH_STK);    \
                                         \
   CASE_REMAP(SUB_STK_I,    SUB_STK);    \
   CASE_REMAP(SUB_STK_U,    SUB_STK);    \
   CASE_REMAP(SUB_STK_X,    SUB_STK);    \
   CASE_REMAP(SUB_REG_I,    SUB_REG);    \
   CASE_REMAP(SUB_REG_U,    SUB_REG);    \
   CASE_REMAP(SUB_REG_X,    SUB_REG);    \
   CASE_REMAP(SUB_MAPREG_I, SUB_MAPREG); \
   CASE_REMAP(SUB_MAPREG_U, SUB_MAPREG); \
   CASE_REMAP(SUB_MAPREG_X, SUB_MAPREG); \
   CASE_REMAP(SUB_WLDREG_I, SUB_WLDREG); \
   CASE_REMAP(SUB_WLDREG_U, SUB_WLDREG); \
   CASE_REMAP(SUB_WLDREG_X, SUB_WLDREG); \
                                         \
   CASE_REMAP(XOR_STK_I,    XOR_STK);    \
   CASE_REMAP(XOR_STK_U,    XOR_STK);    \
   CASE_REMAP(XOR_STK_X,    XOR_STK);    \
                                         \
   CASE_REMAP(NEG_STK_I,    NEG_STK);    \
   CASE_REMAP(NEG_STK_U,    NEG_STK);    \
   CASE_REMAP(NEG_STK_X,    NEG_STK);    \
                                         \
   CASE_REMAP(NOT_STK_I,    NOT_STK);    \
   CASE_REMAP(NOT_STK_U,    NOT_STK);    \
   CASE_REMAP(NOT_STK_X,    NOT_STK);    \
                                         \
   CASE_REMAP(LOGAND_STK_I, LOGAND_STK); \
   CASE_REMAP(LOGAND_STK_U, LOGAND_STK); \
   CASE_REMAP(LOGAND_STK_X, LOGAND_STK); \
                                         \
   CASE_REMAP(LOGIOR_STK_I, LOGIOR_STK); \
   CASE_REMAP(LOGIOR_STK_U, LOGIOR_STK); \
   CASE_REMAP(LOGIOR_STK_X, LOGIOR_STK)


#define BINTOKACS_TOKENS_MAP_JUMP() \
   CASE_REMAP(JMP_HLT,     JMP_HLT); \
   CASE_REMAP(JMP_IMM,     JMP_IMM); \
   CASE_REMAP(JMP_NIL,     JMP_NIL); \
   CASE_REMAP(JMP_RET_SCR, JMP_RET_SCR); \
   CASE_REMAP(JMP_RST,     JMP_RST); \
   CASE_REMAP(JMP_TRU,     JMP_TRU); \
   CASE_REMAP(JMP_VAL,     JMP_VAL)



#define BINTOKACS_TOKENS_MAP_STACK() \
   CASE_REMAP(STK_DROP, STK_DROP)


#define BINTOKACS_TOKENS_MAP_GET()     \
   CASE_REMAP(GET_IMM,    GET_IMM);    \
   CASE_REMAP(GET_REG,    GET_REG);    \
   CASE_REMAP(GET_MAPREG, GET_MAPREG); \
   CASE_REMAP(GET_WLDREG, GET_WLDREG)


#define BINTOKACS_TOKENS_MAP_SET()     \
   CASE_REMAP(SET_REG,    SET_REG);    \
   CASE_REMAP(SET_MAPREG, SET_MAPREG); \
   CASE_REMAP(SET_WLDREG, SET_WLDREG)


#define BINTOKACS_TOKENS_MAP_ACS()             \
   CASE_MAP_ACS(GAME_GET_PLAYERCOUNT);         \
   CASE_MAP_ACS(GAME_GET_SKILL);               \
   CASE_MAP_ACS(GAME_GET_THINGCOUNT_SID);      \
   CASE_MAP_ACS(GAME_GET_THINGCOUNT_SID_IMM);  \
   CASE_MAP_ACS(GAME_GET_TIME);                \
   CASE_MAP_ACS(GAME_GET_TYPE);                \
   CASE_MAP_ACS(LINE_CLR_SPECIAL);             \
   CASE_MAP_ACS(LINE_GET_SIDE);                \
   CASE_MAP_ACS(LTAG_SET_BLOCK);               \
   CASE_MAP_ACS(LTAG_SET_SPECIAL);             \
   CASE_MAP_ACS(LTAG_SET_TEXTURE);             \
   CASE_MAP_ACS(MISC_RANDOM);                  \
   CASE_MAP_ACS(MISC_RANDOM_IMM);              \
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
   CASE_MAP_ACS(WAIT_STAG);                    \
   CASE_MAP_ACS(WAIT_STAG_IMM);                \
   CASE_MAP_ACS(WAIT_TICS);                    \
   CASE_MAP_ACS(WAIT_TICS_IMM)


#define BINTOKACS_TOKENS_MAP_ACSP()     \
   CASE_MAP_ACSP(CHARACTER);            \
   CASE_MAP_ACSP(END);                  \
   CASE_MAP_ACSP(END_BOLD);             \
   CASE_REMAP_ACSP(NUM_DEC_I, NUM_DEC); \
   CASE_MAP_ACSP(START);                \
   CASE_MAP_ACSP(STR)


#define BINTOKACS_TOKENS_MAP_ALL() \
   CASE_REMAP(NOP, NOP);           \
   BINTOKACS_TOKENS_MAP_OPER();    \
   BINTOKACS_TOKENS_MAP_JUMP();    \
   BINTOKACS_TOKENS_MAP_STACK();   \
   BINTOKACS_TOKENS_MAP_GET();     \
   BINTOKACS_TOKENS_MAP_SET();     \
   BINTOKACS_TOKENS_MAP_ACS();     \
   BINTOKACS_TOKENS_MAP_ACSP()



#define BINTOKACS_TOKENS_TRAN_CAST() \
   case OCODE_CAST_STK_X2I:          \
      args.push_back(fracbits);      \
      PUSH_TOKEN(BCODE_GET_IMM);     \
      PUSH_TOKEN(BCODE_RSH_STK);     \
      break;                         \
                                     \
   case OCODE_CAST_STK_I2X:          \
      args.push_back(fracbits);      \
      PUSH_TOKEN(BCODE_GET_IMM);     \
      PUSH_TOKEN(BCODE_LSH_STK);     \
      break


#define BINTOKACS_TOKENS_TRAN_ALL() \
   BINTOKACS_TOKENS_TRAN_CAST()

#endif//HPP_make_tokens__BinaryTokenACS_

