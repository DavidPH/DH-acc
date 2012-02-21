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
// ZDACS (ZDoom ACS) handling.
//
//-----------------------------------------------------------------------------

#include "BinaryTokenZDACS.hpp"

#include "ObjectExpression.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

int BinaryTokenZDACS::arg_counts[BCODE_NONE];


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenZDACS::BinaryTokenZDACS
//
BinaryTokenZDACS::
BinaryTokenZDACS(BinaryCode _code, SourcePosition const &_position,
                 std::vector<std::string> const &_labels,
                 std::vector<ObjectExpression::Pointer> const &_args)
                 : args(_args), code(_code), labels(_labels),
                   position(_position)
{
}

//
// BinaryTokenZDACS::addLabel
//
void BinaryTokenZDACS::addLabel(std::string const &label)
{
   labels.push_back(label);
}

//
// BinaryTokenZDACS::init
//
void BinaryTokenZDACS::init()
{
   BinaryTokenACS::init(arg_counts);

   #define DO_INIT(NAME,ARGC) \
   arg_counts[BCODE_##NAME] = ARGC

   // Arithmetic
   DO_INIT(DIV_FIXED, 0);
   DO_INIT(MUL_FIXED, 0);

   // Bitwise
   DO_INIT(BITWISE_NOT, 0);

   // Branching
   DO_INIT(BRANCH_CASETABLE, -1);

   // Stack-ops
   DO_INIT(STACK_DUP,  0);
   DO_INIT(STACK_SWAP, 0);

   // Trigonometry
   DO_INIT(TRIG_COS, 0);
   DO_INIT(TRIG_SIN, 0);

   // Variable Set Op
   DO_INIT(SETOP_AND_REGISTER, 1);
   DO_INIT(SETOP_IOR_REGISTER, 1);
   DO_INIT(SETOP_LSH_REGISTER, 1);
   DO_INIT(SETOP_RSH_REGISTER, 1);
   DO_INIT(SETOP_XOR_REGISTER, 1);

   // Miscellaneous
   DO_INIT(MISC_NATIVE, 2);

   // ACS Extensions
   DO_INIT(FUNC_CALL_IMM,                      1);
   DO_INIT(FUNC_CALLVOID_IMM,                  1);
   DO_INIT(FUNC_RETN,                          0);
   DO_INIT(FUNC_RETNVOID,                      0);
   DO_INIT(GAME_EXEC,                          0);
   DO_INIT(GAME_EXEC_IMM,                      3);
   DO_INIT(GAME_GET_CVAR,                      0);
   DO_INIT(GAME_GET_INVASIONSTATE,             0);
   DO_INIT(GAME_GET_INVASIONWAVE,              0);
   DO_INIT(GAME_GET_LEVELINFO,                 0);
   DO_INIT(GAME_GET_TEAMINFO_PLAYERCOUNT_BLUE, 0);
   DO_INIT(GAME_GET_TEAMINFO_PLAYERCOUNT_RED,  0);
   DO_INIT(GAME_GET_TEAMINFO_SCORE_BLUE,       0);
   DO_INIT(GAME_GET_TEAMINFO_SCORE_RED,        0);
   DO_INIT(GAME_GET_THINGCOUNT_STR,            0);
   DO_INIT(GAME_GET_TYPE_ONEFLAGCTF,           0);
   DO_INIT(GAME_GET_TYPE_SINGLEPLAYER,         0);
   DO_INIT(GAME_REPLACETEXTURES,               0);
   DO_INIT(GAME_SET_AIRCONTROL,                0);
   DO_INIT(GAME_SET_AIRCONTROL_IMM,            1);
   DO_INIT(GAME_SET_GRAVITY,                   0);
   DO_INIT(GAME_SET_GRAVITY_IMM,               1);
   DO_INIT(GAME_SET_LEVEL,                     0);
   DO_INIT(GAME_SET_MUSIC,                     0);
   DO_INIT(GAME_SET_MUSIC_IMM,                 3);
   DO_INIT(GAME_SET_MUSICLOCAL,                0);
   DO_INIT(GAME_SET_MUSICLOCAL_IMM,            3);
   DO_INIT(GAME_SET_MUSICST,                   0);
   DO_INIT(GAME_SET_SKY,                       0);
   DO_INIT(GET_GLOBALARRAY,                    1);
   DO_INIT(GET_GLOBALREGISTER,                 1);
   DO_INIT(GET_MAPARRAY,                       1);
   DO_INIT(GET_WORLDARRAY,                     1);
   DO_INIT(LINE_GET_OFFSETY,                   0);
   DO_INIT(LTAG_SET_BLOCKMONSTER,              0);
   DO_INIT(MISC_PLAYMOVIE,                     0);
   DO_INIT(MTAG_ADD_INVENTORY,                 0);
   DO_INIT(MTAG_CHK_TEXTURE_CEILING,           0);
   DO_INIT(MTAG_CHK_TEXTURE_FLOOR,             0);
   DO_INIT(MTAG_CLR_INVENTORY,                 0);
   DO_INIT(MTAG_DAMAGE,                        0);
   DO_INIT(MTAG_GET,                           0);
   DO_INIT(MTAG_GET_ANGLE,                     0);
   DO_INIT(MTAG_GET_CEILINGZ,                  0);
   DO_INIT(MTAG_GET_CLASSIFICATION,            0);
   DO_INIT(MTAG_GET_FLOORZ,                    0);
   DO_INIT(MTAG_GET_INVENTORY,                 0);
   DO_INIT(MTAG_GET_LIGHTLEVEL,                0);
   DO_INIT(MTAG_GET_PITCH,                     0);
   DO_INIT(MTAG_GET_X,                         0);
   DO_INIT(MTAG_GET_Y,                         0);
   DO_INIT(MTAG_GET_Z,                         0);
   DO_INIT(MTAG_MORPH,                         0);
   DO_INIT(MTAG_SET,                           0);
   DO_INIT(MTAG_SET_ANGLE,                     0);
   DO_INIT(MTAG_SET_CAMERATEXTURE,             0);
   DO_INIT(MTAG_SET_MARINESPRITE,              0);
   DO_INIT(MTAG_SET_MARINEWEAPON,              0);
   DO_INIT(MTAG_SET_PITCH,                     0);
   DO_INIT(MTAG_SET_SPECIAL,                   0);
   DO_INIT(MTAG_SET_STATE,                     0);
   DO_INIT(MTAG_SET_XYZ,                       0);
   DO_INIT(MTAG_SUB_INVENTORY,                 0);
   DO_INIT(MTAG_UNMORPH,                       0);
   DO_INIT(MTAG_USEINVENTORY,                  0);
   DO_INIT(PLAYER_GET_CAMERA,                  0);
   DO_INIT(PLAYER_GET_CLASS,                   0);
   DO_INIT(PLAYER_GET_INFO,                    0);
   DO_INIT(PLAYER_GET_INGAME,                  0);
   DO_INIT(PLAYER_GET_INPUT,                   0);
   DO_INIT(PLAYER_GET_ISBOT,                   0);
   DO_INIT(SCREEN_FADE_RANGE,                  0);
   DO_INIT(SCREEN_FADE_START,                  0);
   DO_INIT(SCREEN_FADE_STOP,                   0);
   DO_INIT(SCREEN_GET_HEIGHT,                  0);
   DO_INIT(SCREEN_GET_WIDTH,                   0);
   DO_INIT(SCREEN_SET_HUDSIZE,                 0);
   DO_INIT(SCRIPT_SETRETURN,                   0);
   DO_INIT(SET_GLOBALARRAY,                    1);
   DO_INIT(SET_GLOBALREGISTER,                 1);
   DO_INIT(SET_MAPARRAY,                       1);
   DO_INIT(SET_WORLDARRAY,                     1);
   DO_INIT(SETOP_ADD_GLOBALARRAY,              1);
   DO_INIT(SETOP_ADD_GLOBALREGISTER,           1);
   DO_INIT(SETOP_ADD_MAPARRAY,                 1);
   DO_INIT(SETOP_ADD_WORLDARRAY,               1);
   DO_INIT(SETOP_AND_GLOBALARRAY,              1);
   DO_INIT(SETOP_AND_GLOBALREGISTER,           1);
   DO_INIT(SETOP_AND_MAPARRAY,                 1);
   DO_INIT(SETOP_AND_MAPREGISTER,              1);
   DO_INIT(SETOP_AND_WORLDARRAY,               1);
   DO_INIT(SETOP_AND_WORLDREGISTER,            1);
   DO_INIT(SETOP_DEC_GLOBALARRAY,              1);
   DO_INIT(SETOP_DEC_GLOBALREGISTER,           1);
   DO_INIT(SETOP_DEC_MAPARRAY,                 1);
   DO_INIT(SETOP_DEC_WORLDARRAY,               1);
   DO_INIT(SETOP_DIV_GLOBALARRAY,              1);
   DO_INIT(SETOP_DIV_GLOBALREGISTER,           1);
   DO_INIT(SETOP_DIV_MAPARRAY,                 1);
   DO_INIT(SETOP_DIV_WORLDARRAY,               1);
   DO_INIT(SETOP_INC_GLOBALARRAY,              1);
   DO_INIT(SETOP_INC_GLOBALREGISTER,           1);
   DO_INIT(SETOP_INC_MAPARRAY,                 1);
   DO_INIT(SETOP_INC_WORLDARRAY,               1);
   DO_INIT(SETOP_IOR_GLOBALARRAY,              1);
   DO_INIT(SETOP_IOR_GLOBALREGISTER,           1);
   DO_INIT(SETOP_IOR_MAPARRAY,                 1);
   DO_INIT(SETOP_IOR_MAPREGISTER,              1);
   DO_INIT(SETOP_IOR_WORLDARRAY,               1);
   DO_INIT(SETOP_IOR_WORLDREGISTER,            1);
   DO_INIT(SETOP_LSH_GLOBALARRAY,              1);
   DO_INIT(SETOP_LSH_GLOBALREGISTER,           1);
   DO_INIT(SETOP_LSH_MAPARRAY,                 1);
   DO_INIT(SETOP_LSH_MAPREGISTER,              1);
   DO_INIT(SETOP_LSH_WORLDARRAY,               1);
   DO_INIT(SETOP_LSH_WORLDREGISTER,            1);
   DO_INIT(SETOP_MOD_GLOBALARRAY,              1);
   DO_INIT(SETOP_MOD_GLOBALREGISTER,           1);
   DO_INIT(SETOP_MOD_MAPARRAY,                 1);
   DO_INIT(SETOP_MOD_WORLDARRAY,               1);
   DO_INIT(SETOP_MUL_GLOBALARRAY,              1);
   DO_INIT(SETOP_MUL_GLOBALREGISTER,           1);
   DO_INIT(SETOP_MUL_MAPARRAY,                 1);
   DO_INIT(SETOP_MUL_WORLDARRAY,               1);
   DO_INIT(SETOP_RSH_GLOBALARRAY,              1);
   DO_INIT(SETOP_RSH_GLOBALREGISTER,           1);
   DO_INIT(SETOP_RSH_MAPARRAY,                 1);
   DO_INIT(SETOP_RSH_MAPREGISTER,              1);
   DO_INIT(SETOP_RSH_WORLDARRAY,               1);
   DO_INIT(SETOP_RSH_WORLDREGISTER,            1);
   DO_INIT(SETOP_SUB_GLOBALARRAY,              1);
   DO_INIT(SETOP_SUB_GLOBALREGISTER,           1);
   DO_INIT(SETOP_SUB_MAPARRAY,                 1);
   DO_INIT(SETOP_SUB_WORLDARRAY,               1);
   DO_INIT(SETOP_XOR_GLOBALARRAY,              1);
   DO_INIT(SETOP_XOR_GLOBALREGISTER,           1);
   DO_INIT(SETOP_XOR_MAPARRAY,                 1);
   DO_INIT(SETOP_XOR_MAPREGISTER,              1);
   DO_INIT(SETOP_XOR_WORLDARRAY,               1);
   DO_INIT(SETOP_XOR_WORLDREGISTER,            1);
   DO_INIT(SOUND_AMBIENTLOCAL,                 0);
   DO_INIT(SOUND_THING,                        0);
   DO_INIT(SPAWN_POINT,                        0);
   DO_INIT(SPAWN_POINT_IMM,                    6);
   DO_INIT(SPAWN_PROJECTILE_SID,               0);
   DO_INIT(SPAWN_PROJECTILE_STR,               0);
   DO_INIT(SPAWN_SPOT_ANGLE,                   0);
   DO_INIT(SPAWN_SPOT_ANGLE_IMM,               4);
   DO_INIT(SPAWN_SPOT,                         0);
   DO_INIT(SPECIAL_EXEC5_RETN1,                1);
   DO_INIT(STAG_DAMAGE,                        0);
   DO_INIT(STAG_GET_LIGHTLEVEL,                0);
   DO_INIT(STAG_GET_THINGCOUNT_SID,            0);
   DO_INIT(STAG_GET_THINGCOUNT_STR,            0);
   DO_INIT(STAG_GET_Z_CEILING,                 0);
   DO_INIT(STAG_GET_Z_FLOOR,                   0);
   DO_INIT(STAG_SET_TRIGGER_CEILING,           0);
   DO_INIT(STAG_SET_TRIGGER_FLOOR,             0);
   DO_INIT(STRING_COPY_GLOBALRANGE,            0);
   DO_INIT(STRING_COPY_MAPRANGE,               0);
   DO_INIT(STRING_COPY_WORLDRANGE,             0);
   DO_INIT(STRING_GET_LENGTH,                  0);
   DO_INIT(STRING_TAG,                         0);
   DO_INIT(THING_ADD_INVENTORY,                0);
   DO_INIT(THING_ADD_INVENTORY_IMM,            2);
   DO_INIT(THING_CHK_WEAPON,                   0);
   DO_INIT(THING_CLR_INVENTORY,                0);
   DO_INIT(THING_GET_AMMOCAP,                  0);
   DO_INIT(THING_GET_ARMOR,                    0);
   DO_INIT(THING_GET_FRAGS,                    0);
   DO_INIT(THING_GET_HEALTH,                   0);
   DO_INIT(THING_GET_INVENTORY,                0);
   DO_INIT(THING_GET_INVENTORY_IMM,            1);
   DO_INIT(THING_GET_MTAG,                     0);
   DO_INIT(THING_GET_PLAYERNUMBER,             0);
   DO_INIT(THING_GET_SIGIL,                    0);
   DO_INIT(THING_GET_TEAM,                     0);
   DO_INIT(THING_SET_AMMOCAP,                  0);
   DO_INIT(THING_SET_MUGSHOT,                  0);
   DO_INIT(THING_SET_WEAPON,                   0);
   DO_INIT(THING_SUB_INVENTORY,                0);
   DO_INIT(THING_SUB_INVENTORY_IMM,            2);
   DO_INIT(THING_USEINVENTORY,                 0);
   DO_INIT(TRANSLATION_END,                    0);
   DO_INIT(TRANSLATION_PALETTE,                0);
   DO_INIT(TRANSLATION_RGB,                    0);
   DO_INIT(TRANSLATION_START,                  0);
   DO_INIT(TRIG_VECTORANGLE,                   0);

   // ACS Printing
   DO_INIT(PRINT_END_HUD,            0);
   DO_INIT(PRINT_END_HUD_BOLD,       0);
   DO_INIT(PRINT_END_LOG,            0);
   DO_INIT(PRINT_END_OPT,            0);
   DO_INIT(PRINT_END_STRING,         0);
   DO_INIT(PRINT_FIXED,              0);
   DO_INIT(PRINT_KEYBIND,            0);
   DO_INIT(PRINT_NUM_BIN,            0);
   DO_INIT(PRINT_NUM_HEX,            0);
   DO_INIT(PRINT_PLAYER_NAME,        0);
   DO_INIT(PRINT_SET_FONT,           0);
   DO_INIT(PRINT_SET_FONT_IMM,       1);
   DO_INIT(PRINT_START_OPT,          0);
   DO_INIT(PRINT_STRING_GLOBALARRAY, 0);
   DO_INIT(PRINT_STRING_GLOBALRANGE, 0);
   DO_INIT(PRINT_STRING_LOCALIZED,   0);
   DO_INIT(PRINT_STRING_MAPARRAY,    0);
   DO_INIT(PRINT_STRING_MAPRANGE,    0);
   DO_INIT(PRINT_STRING_WORLDARRAY,  0);
   DO_INIT(PRINT_STRING_WORLDRANGE,  0);

   // Other
   DO_INIT(_BRANCH_TABLE, -1);

   // Unsorted

   #undef DO_INIT
}

//
// BinaryTokenZDACS::label_begin
//
BinaryTokenZDACS::label_iterator BinaryTokenZDACS::label_begin() const
{
   return labels.begin();
}

//
// BinaryTokenZDACS::label_end
//
BinaryTokenZDACS::label_iterator BinaryTokenZDACS::label_end() const
{
   return labels.end();
}

//
// BinaryTokenZDACS::size
//
size_t BinaryTokenZDACS::size() const
{
   if (arg_counts[code] < 0) switch (code)
   {
   case BCODE__BRANCH_TABLE:
      return args.size()*6 + 4;

   default:
      throw SourceException("???", SourcePosition::none(), __func__);
   }
   else
      return arg_counts[code]*4 + 4;
}

//
// BinaryTokenZDACS::writeACS0
//
void BinaryTokenZDACS::writeACS0(std::ostream * out) const
{
   if (arg_counts[code] < 0) switch (code)
   {
   case BCODE__BRANCH_TABLE:
      // TODO: BCODE_BRANCHCASESORTED

      for (size_t i(0); i < args.size(); i += 2)
      {
         BinaryTokenACS::write_ACS0_32(out, BCODE_BRANCH_CASE);
         BinaryTokenACS::write_ACS0_32(out, *args[i+0]);
         BinaryTokenACS::write_ACS0_32(out, *args[i+1]);
      }

      BinaryTokenACS::write_ACS0_32(out, BCODE_STACK_DROP);

      break;

   default:
      throw SourceException("???", SourcePosition::none(), __func__);
   }
   else
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
}

//
// BinaryTokenZDACS::write_all
//
void BinaryTokenZDACS::
write_all(std::ostream *out, std::vector<BinaryTokenZDACS> const &instructions)
{
   BinaryTokenACS::output_prep(instructions);

   switch (output_type)
   {
   case OUTPUT_ACS0:
      BinaryTokenACS::output_ACS0(out, instructions);
      break;

   case OUTPUT_ACSE:
      output_ACSE(out, instructions);
      break;

   default:
      throw SourceException("unknown output type", SourcePosition::none(),
                            __func__);
   }
}

// EOF

