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
// Macros                                                                     |
//

#define TOKEN_CLASS BinaryTokenPPACS


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenPPACS::make_tokens
//
void BinaryTokenPPACS::
make_tokens(ObjectToken const &object,
            std::vector<BinaryTokenPPACS> *instructions)
{
   static ObjectExpression::Pointer const fracbits =
      ObjectExpression::create_value_int(16, SourcePosition::builtin());

   static ObjectExpression::Pointer const indexTemp =
      ObjectExpression::create_value_int
      (option_static_temp, SourcePosition::builtin());

   static std::vector<std::string> const nolabels;

   std::vector<ObjectExpression::Pointer> args;

   SourcePosition const &position = object.getPosition();

   std::vector<std::string> const *labels = &object.getLabels();

   switch (object.getCode())
   {
   // Direct Mappings


   BINTOKACS_TOKENS_MAP_ALL_ALL();

   // Arithmetic
   CASE_REMAP(DIV32F, DIV_FIXED);
   CASE_REMAP(MUL32F, MUL_FIXED);

   // Bitwise

   // Branching
   CASE_REMAP_PRE(BRANCH, GOTO,     GOTO);

   // Comparison

   // Logical

   // Stack-ops.

   // Variable Address
   CASE_REMAP_PRE(ADDR, AUTO, AUTO);
   CASE_REMAP_PRE(ADDR, STACK_ADD, STACK_ADD);
   CASE_REMAP_PRE(ADDR, STACK_SUB, STACK_SUB);

   // Variable Get
   CASE_REMAP_PRE(GET, AUTO32F, AUTO);
   CASE_REMAP_PRE(GET, AUTO32I, AUTO);

   CASE_REMAP_PRE(GET, POINTER32F, POINTER);
   CASE_REMAP_PRE(GET, POINTER32I, POINTER);

   CASE_REMAP_PRE(GET, STATIC32F, STATIC);
   CASE_REMAP_PRE(GET, STATIC32I, STATIC);

   // Variable Set
   CASE_REMAP_PRE(SET, AUTO32F, AUTO);
   CASE_REMAP_PRE(SET, AUTO32I, AUTO);

   CASE_REMAP_PRE(SET, POINTER32F, POINTER);
   CASE_REMAP_PRE(SET, POINTER32I, POINTER);

   CASE_REMAP_PRE(SET, STATIC32F, STATIC);
   CASE_REMAP_PRE(SET, STATIC32I, STATIC);

   // Variable Set Op
   CASE_REMAP_PRE(SETOP_ADD, AUTO32F,    AUTO);
   CASE_REMAP_PRE(SETOP_ADD, AUTO32I,    AUTO);
   CASE_REMAP_PRE(SETOP_ADD, AUTO32U,    AUTO);
   CASE_REMAP_PRE(SETOP_ADD, POINTER32F, POINTER);
   CASE_REMAP_PRE(SETOP_ADD, POINTER32I, POINTER);
   CASE_REMAP_PRE(SETOP_ADD, POINTER32U, POINTER);
   CASE_REMAP_PRE(SETOP_ADD, STATIC32F,  STATIC);
   CASE_REMAP_PRE(SETOP_ADD, STATIC32I,  STATIC);
   CASE_REMAP_PRE(SETOP_ADD, STATIC32U,  STATIC);

   CASE_REMAP_PRE(SETOP_AND, AUTO32,    AUTO);
   CASE_REMAP_PRE(SETOP_AND, POINTER32, POINTER);
   CASE_REMAP_PRE(SETOP_AND, STATIC32,  STATIC);

   CASE_REMAP_PRE(SETOP_DEC, AUTO32I,    AUTO);
   CASE_REMAP_PRE(SETOP_DEC, AUTO32U,    AUTO);
   CASE_REMAP_PRE(SETOP_DEC, POINTER32I, POINTER);
   CASE_REMAP_PRE(SETOP_DEC, POINTER32U, POINTER);
   CASE_REMAP_PRE(SETOP_DEC, STATIC32I,  STATIC);
   CASE_REMAP_PRE(SETOP_DEC, STATIC32U,  STATIC);

   CASE_REMAP_PRE(SETOP_DIV, AUTO32I,    AUTO);
   CASE_REMAP_PRE(SETOP_DIV, AUTO32U,    AUTO);    /* WARNING */
   CASE_REMAP_PRE(SETOP_DIV, POINTER32I, POINTER);
   CASE_REMAP_PRE(SETOP_DIV, POINTER32U, POINTER); /* WARNING */
   CASE_REMAP_PRE(SETOP_DIV, STATIC32I,  STATIC);
   CASE_REMAP_PRE(SETOP_DIV, STATIC32U,  STATIC);  /* WARNING */

   CASE_REMAP_PRE(SETOP_INC, AUTO32I,    AUTO);
   CASE_REMAP_PRE(SETOP_INC, AUTO32U,    AUTO);
   CASE_REMAP_PRE(SETOP_INC, POINTER32I, POINTER);
   CASE_REMAP_PRE(SETOP_INC, POINTER32U, POINTER);
   CASE_REMAP_PRE(SETOP_INC, STATIC32I,  STATIC);
   CASE_REMAP_PRE(SETOP_INC, STATIC32U,  STATIC);

   CASE_REMAP_PRE(SETOP_IOR, AUTO32,    AUTO);
   CASE_REMAP_PRE(SETOP_IOR, POINTER32, POINTER);
   CASE_REMAP_PRE(SETOP_IOR, STATIC32,  STATIC);

   CASE_REMAP_PRE(SETOP_LSH, AUTO32,    AUTO);
   CASE_REMAP_PRE(SETOP_LSH, POINTER32, POINTER);
   CASE_REMAP_PRE(SETOP_LSH, STATIC32,  STATIC);

   CASE_REMAP_PRE(SETOP_MOD, AUTO32F,    AUTO);
   CASE_REMAP_PRE(SETOP_MOD, AUTO32I,    AUTO);
   CASE_REMAP_PRE(SETOP_MOD, AUTO32U,    AUTO);    /* WARNING */
   CASE_REMAP_PRE(SETOP_MOD, POINTER32F, POINTER);
   CASE_REMAP_PRE(SETOP_MOD, POINTER32I, POINTER);
   CASE_REMAP_PRE(SETOP_MOD, POINTER32U, POINTER); /* WARNING */
   CASE_REMAP_PRE(SETOP_MOD, STATIC32F,  STATIC);
   CASE_REMAP_PRE(SETOP_MOD, STATIC32I,  STATIC);
   CASE_REMAP_PRE(SETOP_MOD, STATIC32U,  STATIC);  /* WARNING */

   CASE_REMAP_PRE(SETOP_MUL, AUTO32I,    AUTO);
   CASE_REMAP_PRE(SETOP_MUL, AUTO32U,    AUTO);
   CASE_REMAP_PRE(SETOP_MUL, POINTER32I, POINTER);
   CASE_REMAP_PRE(SETOP_MUL, POINTER32U, POINTER);
   CASE_REMAP_PRE(SETOP_MUL, STATIC32I,  STATIC);
   CASE_REMAP_PRE(SETOP_MUL, STATIC32U,  STATIC);

   CASE_REMAP_PRE(SETOP_RSH, AUTO32,    AUTO);
   CASE_REMAP_PRE(SETOP_RSH, POINTER32, POINTER);
   CASE_REMAP_PRE(SETOP_RSH, STATIC32,  STATIC);

   CASE_REMAP_PRE(SETOP_SUB, AUTO32F,    AUTO);
   CASE_REMAP_PRE(SETOP_SUB, AUTO32I,    AUTO);
   CASE_REMAP_PRE(SETOP_SUB, AUTO32U,    AUTO);
   CASE_REMAP_PRE(SETOP_SUB, POINTER32F, POINTER);
   CASE_REMAP_PRE(SETOP_SUB, POINTER32I, POINTER);
   CASE_REMAP_PRE(SETOP_SUB, POINTER32U, POINTER);
   CASE_REMAP_PRE(SETOP_SUB, STATIC32F,  STATIC);
   CASE_REMAP_PRE(SETOP_SUB, STATIC32I,  STATIC);
   CASE_REMAP_PRE(SETOP_SUB, STATIC32U,  STATIC);

   CASE_REMAP_PRE(SETOP_XOR, AUTO32,    AUTO);
   CASE_REMAP_PRE(SETOP_XOR, POINTER32, POINTER);
   CASE_REMAP_PRE(SETOP_XOR, STATIC32,  STATIC);

   // Miscellaneous

   // ACS

   // ACS Common Extensions

   // ACS Extensions
   CASE_MAP_ACSE(MID_GET);
   CASE_MAP_ACSE(MID_SET);

   // ACS Printing
   CASE_MAP_ACSP(END_ERROR);
   CASE_MAP_ACSP(END_LOG);
   CASE_REMAP_ACSP(NUM_DEC32F, FIXED);



   // Translations


   BINTOKACS_TOKENS_TRAN_ALL_ALL();

   // Variable Address

   case OCODE_ADDR_STACK_ADD_IMM:
      PUSH_TOKEN_ARGS1(BCODE_GET_LITERAL, 1);
      PUSH_TOKEN(BCODE_ADDR_STACK_ADD);
      break;

   case OCODE_ADDR_STACK_SUB_IMM:
      PUSH_TOKEN_ARGS1(BCODE_GET_LITERAL, 1);
      PUSH_TOKEN(BCODE_ADDR_STACK_SUB);
      break;

   // Variable Get

   case OCODE_GET_TEMP:
      args.push_back(indexTemp);
      PUSH_TOKEN(BCODE_GET_WORLDREGISTER);
      break;

   // Variable Set

   case OCODE_SET_TEMP:
      args.push_back(indexTemp);
      PUSH_TOKEN(BCODE_SET_WORLDREGISTER);
      break;

   case OCODE_NONE:
   default:
      throw SourceException("unknown OCODE: " + std::string(make_string(object.getCode())),
                            position, __func__);
   }
}

//
// BinaryTokenPPACS::make_tokens
//
void BinaryTokenPPACS::
make_tokens(ObjectVector const &objects,
            std::vector<BinaryTokenPPACS> *instructions)
{
   for (bigsint index(0); index < objects.size(); ++index)
      make_tokens(objects[index], instructions);
}

// EOF

