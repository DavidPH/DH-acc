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
// Object to ZDACS translation.
//
//-----------------------------------------------------------------------------

#include "../BinaryTokenZDACS.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectToken.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"

#include "../BinaryTokenACS/make_tokens.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// CASE_ADDR_BINOP
//
#define CASE_ADDR_BINOP(OP,TO,TB)      \
case OCODE_##OP##_STATIC##TO:          \
   PUSH_TOKEN_ARGS1(BCODE_GET_IMM, 1); \
   PUSH_TOKEN(BCODE_STK_SWAP);         \
   args.push_back(indexAddr);          \
   PUSH_TOKEN(BCODE_##OP##_GBLARR##TB);\
   break;                              \
                                       \
case OCODE_##OP##_AUTO##TO:            \
   args.push_back(indexStack);         \
   PUSH_TOKEN(BCODE_GET_WLDREG);       \
   PUSH_TOKEN_ADD_ARG0();              \
   PUSH_TOKEN(BCODE_STK_SWAP);         \
   args.push_back(indexAddr);          \
   PUSH_TOKEN(BCODE_##OP##_GBLARR##TB);\
   break;                              \
                                       \
case OCODE_##OP##_PTR##TO:             \
   PUSH_TOKEN_ADD_ARG0();              \
   PUSH_TOKEN(BCODE_STK_SWAP);         \
   args.push_back(indexAddr);          \
   PUSH_TOKEN(BCODE_##OP##_GBLARR##TB);\
   break

//
// CASE_ADDR_UNAOP
//
#define CASE_ADDR_UNAOP(OP,TO,TB)      \
case OCODE_##OP##_STATIC##TO:          \
   PUSH_TOKEN_ARGS1(BCODE_GET_IMM, 1); \
   args.push_back(indexAddr);          \
   PUSH_TOKEN(BCODE_##OP##_GBLARR##TB);\
   break;                              \
                                       \
case OCODE_##OP##_AUTO##TO:            \
   args.push_back(indexStack);         \
   PUSH_TOKEN(BCODE_GET_WLDREG);       \
   PUSH_TOKEN_ADD_ARG0();              \
   args.push_back(indexAddr);          \
   PUSH_TOKEN(BCODE_##OP##_GBLARR##TB);\
   break;                              \
                                       \
case OCODE_##OP##_PTR##TO:             \
   PUSH_TOKEN_ADD_ARG0();              \
   args.push_back(indexAddr);          \
   PUSH_TOKEN(BCODE_##OP##_GBLARR##TB);\
   break

//
// CASE_REMAP_REGS
//
#define CASE_REMAP_REGS(OP,TO,TB)             \
CASE_REMAP(OP##_GBLREG##TO, OP##_GBLREG##TB); \
CASE_REMAP(OP##_MAPARR##TO, OP##_MAPARR##TB); \
CASE_REMAP(OP##_WLDARR##TO, OP##_WLDARR##TB); \
CASE_REMAP(OP##_GBLARR##TO, OP##_GBLARR##TB); \
CASE_REMAP(OP##_TEMP##TO,   OP##_REG##TB)

//
// CAS_REMAP_REG2
//
#define CASE_REMAP_REG2(OP,TO,TB) \
CASE_REMAP_REGS(OP,TO,TB); \
CASE_REMAP(OP##_REG##TO,    OP##_REG##TB); \
CASE_REMAP(OP##_MAPREG##TO, OP##_MAPREG##TB); \
CASE_REMAP(OP##_WLDREG##TO, OP##_WLDREG##TB)

#define PUSH_TOKEN_ADD_ARG0()          \
if (object->getArg(0)->resolveInt())   \
{                                      \
   PUSH_TOKEN_ARGS1(BCODE_GET_IMM, 1); \
   PUSH_TOKEN(BCODE_ADD_STK);          \
}                                      \
else (void)0


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenZDACS::make_tokens
//
void BinaryTokenZDACS::make_tokens
(ObjectVector const &objects, std::vector<BinaryTokenZDACS> *instructions)
{
   static ObjectExpression::Pointer const fracbits =
      ObjectExpression::create_value_int(16, SourcePosition::builtin());

   static ObjectExpression::Pointer const indexAddr =
      ObjectExpression::create_value_int
      (option_addr_array, SourcePosition::builtin());

   static ObjectExpression::Pointer const indexStack =
      ObjectExpression::create_value_int
      (option_addr_stack, SourcePosition::builtin());

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

   // Operators
   CASE_ADDR_BINOP(ADD, _I,);
   CASE_ADDR_BINOP(ADD, _U,);
   CASE_ADDR_BINOP(ADD, _X,);
   CASE_REMAP_REGS(ADD, _I,);
   CASE_REMAP_REGS(ADD, _U,);
   CASE_REMAP_REGS(ADD, _X,);

   CASE_ADDR_BINOP(AND, _I,);
   CASE_ADDR_BINOP(AND, _U,);
   CASE_ADDR_BINOP(AND, _X,);
   CASE_REMAP_REG2(AND, _I,);
   CASE_REMAP_REG2(AND, _U,);
   CASE_REMAP_REG2(AND, _X,);

   CASE_ADDR_UNAOP(DEC, _I,);
   CASE_ADDR_UNAOP(DEC, _U,);
   CASE_REMAP_REGS(DEC, _I,);
   CASE_REMAP_REGS(DEC, _U,);

   CASE_REMAP(DIV_STK_X, DIV_STK_X);
   CASE_ADDR_BINOP(DIV, _I,);
   CASE_REMAP_REGS(DIV, _I,);

   CASE_ADDR_UNAOP(INC, _I,);
   CASE_ADDR_UNAOP(INC, _U,);
   CASE_REMAP_REGS(INC, _I,);
   CASE_REMAP_REGS(INC, _U,);

   CASE_ADDR_BINOP(IOR, _I,);
   CASE_ADDR_BINOP(IOR, _U,);
   CASE_ADDR_BINOP(IOR, _X,);
   CASE_REMAP_REG2(IOR, _I,);
   CASE_REMAP_REG2(IOR, _U,);
   CASE_REMAP_REG2(IOR, _X,);

   CASE_ADDR_BINOP(LSH, _I,);
   CASE_ADDR_BINOP(LSH, _U,);
   CASE_ADDR_BINOP(LSH, _X,);
   CASE_REMAP_REG2(LSH, _I,);
   CASE_REMAP_REG2(LSH, _U,);
   CASE_REMAP_REG2(LSH, _X,);

   CASE_ADDR_BINOP(MOD, _I,);
   CASE_ADDR_BINOP(MOD, _X,);
   CASE_REMAP_REGS(MOD, _I,);
   CASE_REMAP_REGS(MOD, _X,);

   CASE_REMAP(MUL_STK_X, MUL_STK_X);
   CASE_ADDR_BINOP(MUL, _I,);
   CASE_ADDR_BINOP(MUL, _U,);
   CASE_REMAP_REGS(MUL, _I,);
   CASE_REMAP_REGS(MUL, _U,);

   CASE_ADDR_BINOP(RSH, _I,);
   CASE_ADDR_BINOP(RSH, _X,);
   CASE_REMAP_REG2(RSH, _I,);
   CASE_REMAP_REG2(RSH, _X,);

   CASE_ADDR_BINOP(SUB, _I,);
   CASE_ADDR_BINOP(SUB, _U,);
   CASE_ADDR_BINOP(SUB, _X,);
   CASE_REMAP_REGS(SUB, _I,);
   CASE_REMAP_REGS(SUB, _U,);
   CASE_REMAP_REGS(SUB, _X,);

   CASE_ADDR_BINOP(XOR, _I,);
   CASE_ADDR_BINOP(XOR, _U,);
   CASE_ADDR_BINOP(XOR, _X,);
   CASE_REMAP_REG2(XOR, _I,);
   CASE_REMAP_REG2(XOR, _U,);
   CASE_REMAP_REG2(XOR, _X,);

   CASE_REMAP(INV_STK_I, INV_STK);
   CASE_REMAP(INV_STK_U, INV_STK);
   CASE_REMAP(INV_STK_X, INV_STK);

   // Jumps
   case OCODE_JMP_TAB:
      if ((args = object->args).size() % 2)
         ERROR_P("uneven OCODE_BRANCH_TABLE");
      PUSH_TOKEN(BCODE__JMP_TAB);
      break;
   CASE_REMAP(JMP_CAL,         JMP_CAL);
   CASE_REMAP(JMP_CAL_IMM,     JMP_CAL_IMM);
   case OCODE_JMP_CAL_NIL:
      PUSH_TOKEN(BCODE_JMP_CAL);
      PUSH_TOKEN(BCODE_STK_DROP);
      break;
   CASE_REMAP(JMP_CAL_NIL_IMM, JMP_CAL_NIL_IMM);
   CASE_REMAP(JMP_RET,         JMP_RET);
   CASE_REMAP(JMP_RET_NIL,     JMP_RET_NIL);

   // Stack-ops.
   CASE_REMAP(STK_COPY, STK_COPY);
   CASE_REMAP(STK_SWAP, STK_SWAP);

   // Trigonometry
   CASE_REMAP(TRIG_COS_X, TRIG_COS);
   CASE_REMAP(TRIG_SIN_X, TRIG_SIN);

   // Variable Address
   case OCODE_GET_AUTPTR:
      args.push_back(indexStack);
      PUSH_TOKEN(BCODE_GET_WLDREG);
      break;

   case OCODE_GET_AUTPTR_IMM:
      args.push_back(indexStack);
      PUSH_TOKEN(BCODE_GET_WLDREG);
      PUSH_TOKEN_ADD_ARG0();
      break;

   case OCODE_SET_AUTPTR_IMM:
      PUSH_TOKEN_ARGS1(BCODE_GET_IMM, 1);
   case OCODE_SET_AUTPTR:
      args.push_back(indexStack);
      PUSH_TOKEN(BCODE_SET_WLDREG);
      break;

   case OCODE_ADD_AUTPTR_IMM:
      PUSH_TOKEN_ARGS1(BCODE_GET_IMM, 1);
   case OCODE_ADD_AUTPTR:
      args.push_back(indexStack);
      PUSH_TOKEN(BCODE_ADD_WLDREG);
      break;

   case OCODE_SUB_AUTPTR_IMM:
      PUSH_TOKEN_ARGS1(BCODE_GET_IMM, 1);
   case OCODE_SUB_AUTPTR:
      args.push_back(indexStack);
      PUSH_TOKEN(BCODE_SUB_WLDREG);
      break;

   // Variable Get
   CASE_REMAP(GET_FUNCP, GET_FUNCP);

   case OCODE_GET_STRING:
      PUSH_TOKEN_ARGS1(BCODE_GET_IMM, 1);
      // Don't tag nullptr.
      if(option_string_tag && object->getArg(0)->resolveInt())
         PUSH_TOKEN(BCODE_STRING_TAG);
      break;

   CASE_ADDR_UNAOP(GET,,);
   CASE_REMAP_REGS(GET,,);

   // Variable Set
   CASE_ADDR_BINOP(SET,,);
   CASE_REMAP_REGS(SET,,);

   // Miscellaneous
   CASE_REMAP(NATIVE, NATIVE);

   // ACS

   // ACS Common Extensions

   // ACS Extensions
   CASE_MAP_ACSE(GAME_EXEC);
   CASE_MAP_ACSE(GAME_EXEC_IMM);
   CASE_MAP_ACSE(GAME_GET_CVAR);
   CASE_MAP_ACSE(GAME_GET_INVASIONSTATE);
   CASE_MAP_ACSE(GAME_GET_INVASIONWAVE);
   CASE_MAP_ACSE(GAME_GET_LEVELINFO);
   CASE_MAP_ACSE(GAME_GET_TEAMINFO_PLAYERCOUNT_BLUE);
   CASE_MAP_ACSE(GAME_GET_TEAMINFO_PLAYERCOUNT_RED);
   CASE_MAP_ACSE(GAME_GET_TEAMINFO_SCORE_BLUE);
   CASE_MAP_ACSE(GAME_GET_TEAMINFO_SCORE_RED);
   CASE_MAP_ACSE(GAME_GET_THINGCOUNT_STR);
   CASE_MAP_ACSE(GAME_GET_TYPE_ONEFLAGCTF);
   CASE_MAP_ACSE(GAME_GET_TYPE_SINGLEPLAYER);
   CASE_MAP_ACSE(GAME_REPLACETEXTURES);
   CASE_MAP_ACSE(GAME_SET_AIRCONTROL);
   CASE_MAP_ACSE(GAME_SET_AIRCONTROL_IMM);
   CASE_MAP_ACSE(GAME_SET_GRAVITY);
   CASE_MAP_ACSE(GAME_SET_GRAVITY_IMM);
   CASE_MAP_ACSE(GAME_SET_LEVEL);
   CASE_MAP_ACSE(GAME_SET_MUSIC);
   CASE_MAP_ACSE(GAME_SET_MUSIC_IMM);
   CASE_MAP_ACSE(GAME_SET_MUSICLOCAL);
   CASE_MAP_ACSE(GAME_SET_MUSICLOCAL_IMM);
   CASE_MAP_ACSE(GAME_SET_MUSICST);
   CASE_MAP_ACSE(GAME_SET_SKY);
   CASE_MAP_ACSE(LINE_GET_OFFSETY);
   CASE_MAP_ACSE(LTAG_SET_BLOCKMONSTER);
   CASE_MAP_ACSE(MISC_PLAYMOVIE);
   CASE_MAP_ACSE(MTAG_ADD_INVENTORY);
   CASE_MAP_ACSE(MTAG_CHK_TEXTURE_CEILING);
   CASE_MAP_ACSE(MTAG_CHK_TEXTURE_FLOOR);
   CASE_MAP_ACSE(MTAG_CLR_INVENTORY);
   CASE_MAP_ACSE(MTAG_DAMAGE);
   CASE_MAP_ACSE(MTAG_GET);
   CASE_MAP_ACSE(MTAG_GET_ANGLE);
   CASE_MAP_ACSE(MTAG_GET_CEILINGZ);
   CASE_MAP_ACSE(MTAG_GET_CLASSIFICATION);
   CASE_MAP_ACSE(MTAG_GET_FLOORZ);
   CASE_MAP_ACSE(MTAG_GET_INVENTORY);
   CASE_MAP_ACSE(MTAG_GET_LIGHTLEVEL);
   CASE_MAP_ACSE(MTAG_GET_PITCH);
   CASE_MAP_ACSE(MTAG_GET_X);
   CASE_MAP_ACSE(MTAG_GET_Y);
   CASE_MAP_ACSE(MTAG_GET_Z);
   CASE_MAP_ACSE(MTAG_MORPH);
   CASE_MAP_ACSE(MTAG_SET);
   CASE_MAP_ACSE(MTAG_SET_ANGLE);
   CASE_MAP_ACSE(MTAG_SET_CAMERATEXTURE);
   CASE_MAP_ACSE(MTAG_SET_MARINESPRITE);
   CASE_MAP_ACSE(MTAG_SET_MARINEWEAPON);
   CASE_MAP_ACSE(MTAG_SET_PITCH);
   CASE_MAP_ACSE(MTAG_SET_SPECIAL);
   CASE_MAP_ACSE(MTAG_SET_STATE);
   CASE_MAP_ACSE(MTAG_SET_XYZ);
   CASE_MAP_ACSE(MTAG_SUB_INVENTORY);
   CASE_MAP_ACSE(MTAG_UNMORPH);
   CASE_MAP_ACSE(MTAG_USEINVENTORY);
   CASE_MAP_ACSE(PLAYER_GET_CAMERA);
   CASE_MAP_ACSE(PLAYER_GET_CLASS);
   CASE_MAP_ACSE(PLAYER_GET_INFO);
   CASE_MAP_ACSE(PLAYER_GET_INGAME);
   CASE_MAP_ACSE(PLAYER_GET_INPUT);
   CASE_MAP_ACSE(PLAYER_GET_ISBOT);
   CASE_MAP_ACSE(SCREEN_FADE_RANGE);
   CASE_MAP_ACSE(SCREEN_FADE_START);
   CASE_MAP_ACSE(SCREEN_FADE_STOP);
   CASE_MAP_ACSE(SCREEN_GET_HEIGHT);
   CASE_MAP_ACSE(SCREEN_GET_WIDTH);
   CASE_MAP_ACSE(SCREEN_SET_HUDSIZE);
   CASE_MAP_ACSE(SCRIPT_SETRETURN);
   CASE_MAP_ACSE(SOUND_AMBIENTLOCAL);
   CASE_MAP_ACSE(SOUND_THING);
   CASE_MAP_ACSE(SPAWN_POINT);
   CASE_MAP_ACSE(SPAWN_POINT_IMM);
   CASE_MAP_ACSE(SPAWN_PROJECTILE_SID);
   CASE_MAP_ACSE(SPAWN_PROJECTILE_STR);
   CASE_MAP_ACSE(SPAWN_SPOT_ANGLE);
   CASE_MAP_ACSE(SPAWN_SPOT_ANGLE_IMM);
   CASE_MAP_ACSE(SPAWN_SPOT);
   CASE_MAP_ACSE(SPECIAL_EXEC5_RETN1);
   CASE_MAP_ACSE(STAG_DAMAGE);
   CASE_MAP_ACSE(STAG_GET_LIGHTLEVEL);
   CASE_MAP_ACSE(STAG_GET_THINGCOUNT_SID);
   CASE_MAP_ACSE(STAG_GET_THINGCOUNT_STR);
   CASE_MAP_ACSE(STAG_GET_Z_CEILING);
   CASE_MAP_ACSE(STAG_GET_Z_FLOOR);
   CASE_MAP_ACSE(STAG_SET_TRIGGER_CEILING);
   CASE_MAP_ACSE(STAG_SET_TRIGGER_FLOOR);
   CASE_MAP_ACSE(STRING_COPY_GLOBALRANGE);
   CASE_MAP_ACSE(STRING_COPY_MAPRANGE);
   CASE_MAP_ACSE(STRING_COPY_WORLDRANGE);
   CASE_MAP_ACSE(STRING_GET_LENGTH);
   CASE_MAP_ACSE(THING_ADD_INVENTORY);
   CASE_MAP_ACSE(THING_ADD_INVENTORY_IMM);
   CASE_MAP_ACSE(THING_CHK_WEAPON);
   CASE_MAP_ACSE(THING_CLR_INVENTORY);
   CASE_MAP_ACSE(THING_GET_AMMOCAP);
   CASE_MAP_ACSE(THING_GET_ARMOR);
   CASE_MAP_ACSE(THING_GET_FRAGS);
   CASE_MAP_ACSE(THING_GET_HEALTH);
   CASE_MAP_ACSE(THING_GET_INVENTORY);
   CASE_MAP_ACSE(THING_GET_INVENTORY_IMM);
   CASE_MAP_ACSE(THING_GET_MTAG);
   CASE_MAP_ACSE(THING_GET_PLAYERNUMBER);
   CASE_MAP_ACSE(THING_GET_SIGIL);
   CASE_MAP_ACSE(THING_GET_TEAM);
   CASE_MAP_ACSE(THING_SET_AMMOCAP);
   CASE_MAP_ACSE(THING_SET_MUGSHOT);
   CASE_MAP_ACSE(THING_SET_WEAPON);
   CASE_MAP_ACSE(THING_SUB_INVENTORY);
   CASE_MAP_ACSE(THING_SUB_INVENTORY_IMM);
   CASE_MAP_ACSE(THING_USEINVENTORY);
   CASE_MAP_ACSE(TRANSLATION_END);
   CASE_MAP_ACSE(TRANSLATION_PALETTE);
   CASE_MAP_ACSE(TRANSLATION_RGB);
   CASE_MAP_ACSE(TRANSLATION_START);
   CASE_MAP_ACSE(TRIG_VECTORANGLE);

   // ACS Printing
   CASE_MAP_ACSP(END_HUD);
   CASE_MAP_ACSP(END_HUD_BOLD);
   CASE_MAP_ACSP(END_LOG);
   CASE_MAP_ACSP(END_OPT);
   CASE_MAP_ACSP(END_STR);
   CASE_MAP_ACSP(KEYBIND);
   CASE_REMAP_ACSP(NUM_BIN_U, NUM_BIN);
   CASE_REMAP_ACSP(NUM_DEC_X, NUM_DEC_X);
   CASE_REMAP_ACSP(NUM_HEX_U, NUM_HEX);
   CASE_MAP_ACSP(PLAYER_NAME);
   CASE_MAP_ACSP(SET_FONT);
   CASE_MAP_ACSP(SET_FONT_IMM);
   CASE_MAP_ACSP(START_OPT);
   CASE_MAP_ACSP(STR_GBLARR);
   CASE_MAP_ACSP(STR_GBLRNG);
   CASE_MAP_ACSP(STR_LOCALIZED);
   CASE_MAP_ACSP(STR_MAPARR);
   CASE_MAP_ACSP(STR_MAPRNG);
   CASE_MAP_ACSP(STR_WLDARR);
   CASE_MAP_ACSP(STR_WLDRNG);

   case OCODE_NONE:
   default:
      ERROR_P("unknown OCODE: %s", make_string(object->code));
   }
   }
}

// EOF

