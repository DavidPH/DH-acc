//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// Object to PPACS translation.
//
//-----------------------------------------------------------------------------

#include "../BinaryTokenPPACS.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectToken.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"

#include "../BinaryTokenACS/make_tokens.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenPPACS::make_tokens
//
void BinaryTokenPPACS::make_tokens
(ObjectVector const &objects, std::vector<BinaryTokenPPACS> *instructions)
{
   static ObjectExpression::Pointer const fracbits =
      ObjectExpression::CreateValueINT(16, SourcePosition::builtin());

   static std::vector<std::string> const nolabels;

   std::vector<ObjectExpression::Pointer> args;

   ObjectVector::const_iterator object;
   for (object = objects.begin(); object != objects.end(); ++object)
   {
   SourcePosition const &pos = object->pos;

   std::vector<std::string> const *labels = &object->labels;

   switch (object->code)
   {
   BINTOKACS_TOKENS_MAP_ALL();
   BINTOKACS_TOKENS_TRAN_ALL();

   //
   // Operators
   //
   CASE_REMAP(ADD_AUTO_I,   ADD_AUTO);
   CASE_REMAP(ADD_AUTO_U,   ADD_AUTO);
   CASE_REMAP(ADD_AUTO_X,   ADD_AUTO);
   CASE_REMAP(ADD_PTR_I,    ADD_PTR);
   CASE_REMAP(ADD_PTR_U,    ADD_PTR);
   CASE_REMAP(ADD_PTR_X,    ADD_PTR);
   CASE_REMAP(ADD_STATIC_I, ADD_STATIC);
   CASE_REMAP(ADD_STATIC_U, ADD_STATIC);
   CASE_REMAP(ADD_STATIC_X, ADD_STATIC);

   CASE_REMAP(AND_AUTO_I,   AND_AUTO);
   CASE_REMAP(AND_AUTO_U,   AND_AUTO);
   CASE_REMAP(AND_AUTO_X,   AND_AUTO);
   CASE_REMAP(AND_PTR_I,    AND_PTR);
   CASE_REMAP(AND_PTR_U,    AND_PTR);
   CASE_REMAP(AND_PTR_X,    AND_PTR);
   CASE_REMAP(AND_STATIC_I, AND_STATIC);
   CASE_REMAP(AND_STATIC_U, AND_STATIC);
   CASE_REMAP(AND_STATIC_X, AND_STATIC);

   CASE_REMAP(DEC_AUTO_I,   DEC_AUTO);
   CASE_REMAP(DEC_AUTO_U,   DEC_AUTO);
   CASE_REMAP(DEC_PTR_I,    DEC_PTR);
   CASE_REMAP(DEC_PTR_U,    DEC_PTR);
   CASE_REMAP(DEC_STATIC_I, DEC_STATIC);
   CASE_REMAP(DEC_STATIC_U, DEC_STATIC);

   CASE_REMAP(DIV_STK_X,    DIV_STK_X);
   CASE_REMAP(DIV_AUTO_I,   DIV_AUTO);
   CASE_REMAP(DIV_PTR_I,    DIV_PTR);
   CASE_REMAP(DIV_STATIC_I, DIV_STATIC);

   CASE_REMAP(INC_AUTO_I,   INC_AUTO);
   CASE_REMAP(INC_AUTO_U,   INC_AUTO);
   CASE_REMAP(INC_PTR_I,    INC_PTR);
   CASE_REMAP(INC_PTR_U,    INC_PTR);
   CASE_REMAP(INC_STATIC_I, INC_STATIC);
   CASE_REMAP(INC_STATIC_U, INC_STATIC);

   CASE_REMAP(IOR_AUTO_I,   IOR_AUTO);
   CASE_REMAP(IOR_AUTO_U,   IOR_AUTO);
   CASE_REMAP(IOR_AUTO_X,   IOR_AUTO);
   CASE_REMAP(IOR_PTR_I,    IOR_PTR);
   CASE_REMAP(IOR_PTR_U,    IOR_PTR);
   CASE_REMAP(IOR_PTR_X,    IOR_PTR);
   CASE_REMAP(IOR_STATIC_I, IOR_STATIC);
   CASE_REMAP(IOR_STATIC_U, IOR_STATIC);
   CASE_REMAP(IOR_STATIC_X, IOR_STATIC);

   CASE_REMAP(LSH_AUTO_I,   LSH_AUTO);
   CASE_REMAP(LSH_AUTO_U,   LSH_AUTO);
   CASE_REMAP(LSH_AUTO_X,   LSH_AUTO);
   CASE_REMAP(LSH_PTR_I,    LSH_PTR);
   CASE_REMAP(LSH_PTR_U,    LSH_PTR);
   CASE_REMAP(LSH_PTR_X,    LSH_PTR);
   CASE_REMAP(LSH_STATIC_I, LSH_STATIC);
   CASE_REMAP(LSH_STATIC_U, LSH_STATIC);
   CASE_REMAP(LSH_STATIC_X, LSH_STATIC);

   CASE_REMAP(MOD_AUTO_I,   MOD_AUTO);
   CASE_REMAP(MOD_AUTO_X,   MOD_AUTO);
   CASE_REMAP(MOD_PTR_I,    MOD_PTR);
   CASE_REMAP(MOD_PTR_X,    MOD_PTR);
   CASE_REMAP(MOD_STATIC_I, MOD_STATIC);
   CASE_REMAP(MOD_STATIC_X, MOD_STATIC);

   CASE_REMAP(MUL_STK_X,    MUL_STK_X);
   CASE_REMAP(MUL_AUTO_I,   MUL_AUTO);
   CASE_REMAP(MUL_AUTO_U,   MUL_AUTO);
   CASE_REMAP(MUL_PTR_I,    MUL_PTR);
   CASE_REMAP(MUL_PTR_U,    MUL_PTR);
   CASE_REMAP(MUL_STATIC_I, MUL_STATIC);
   CASE_REMAP(MUL_STATIC_U, MUL_STATIC);

   CASE_REMAP(RSH_AUTO_I,   RSH_AUTO);
   CASE_REMAP(RSH_AUTO_X,   RSH_AUTO);
   CASE_REMAP(RSH_PTR_I,    RSH_PTR);
   CASE_REMAP(RSH_PTR_X,    RSH_PTR);
   CASE_REMAP(RSH_STATIC_I, RSH_STATIC);
   CASE_REMAP(RSH_STATIC_X, RSH_STATIC);

   CASE_REMAP(SUB_AUTO_I,   SUB_AUTO);
   CASE_REMAP(SUB_AUTO_U,   SUB_AUTO);
   CASE_REMAP(SUB_AUTO_X,   SUB_AUTO);
   CASE_REMAP(SUB_PTR_I,    SUB_PTR);
   CASE_REMAP(SUB_PTR_U,    SUB_PTR);
   CASE_REMAP(SUB_PTR_X,    SUB_PTR);
   CASE_REMAP(SUB_STATIC_I, SUB_STATIC);
   CASE_REMAP(SUB_STATIC_U, SUB_STATIC);
   CASE_REMAP(SUB_STATIC_X, SUB_STATIC);

   CASE_REMAP(XOR_AUTO_I,   XOR_AUTO);
   CASE_REMAP(XOR_AUTO_U,   XOR_AUTO);
   CASE_REMAP(XOR_AUTO_X,   XOR_AUTO);
   CASE_REMAP(XOR_PTR_I,    XOR_PTR);
   CASE_REMAP(XOR_PTR_U,    XOR_PTR);
   CASE_REMAP(XOR_PTR_X,    XOR_PTR);
   CASE_REMAP(XOR_STATIC_I, XOR_STATIC);
   CASE_REMAP(XOR_STATIC_U, XOR_STATIC);
   CASE_REMAP(XOR_STATIC_X, XOR_STATIC);

   //
   // Jumps
   //
   CASE_REMAP(JMP, JMP);

   //
   // Variable Address
   //
   CASE_REMAP(GET_AUTPTR_IMM, GET_AUTPTR_IMM);

   CASE_REMAP(ADD_AUTPTR, ADD_AUTPTR);
   case OCODE_ADD_AUTPTR_IMM:
      PUSH_TOKEN_ARGS1(BCODE_GET_IMM, 1);
      PUSH_TOKEN(BCODE_ADD_AUTPTR);
      break;

   CASE_REMAP(SUB_AUTPTR, SUB_AUTPTR);
   case OCODE_SUB_AUTPTR_IMM:
      PUSH_TOKEN_ARGS1(BCODE_GET_IMM, 1);
      PUSH_TOKEN(BCODE_SUB_AUTPTR);
      break;

   //
   // Variable Get
   //
   CASE_REMAP(GET_STATIC, GET_STATIC);
   CASE_REMAP(GET_AUTO,   GET_AUTO);
   CASE_REMAP(GET_PTR,    GET_PTR);
   CASE_REMAP(GET_TEMP,   GET_AUTO);

   //
   // Variable Set
   //
   CASE_REMAP(SET_STATIC, SET_STATIC);
   CASE_REMAP(SET_AUTO,   SET_AUTO);
   CASE_REMAP(SET_PTR,    SET_PTR);
   CASE_REMAP(SET_TEMP,   SET_AUTO);

   //
   // ACS Extensions
   //
   CASE_MAP_ACSE(MID_GET);
   CASE_MAP_ACSE(MID_SET);

   //
   // ACS Printing
   //
   CASE_MAP_ACSP(END_ERROR);
   CASE_MAP_ACSP(END_LOG);
   CASE_REMAP_ACSP(NUM_DEC_X, NUM_DEC_X);

   case OCODE_NONE:
   default:
      Error_P("unknown OCODE: %s", make_string(object->code));
   }
   }
}

// EOF

