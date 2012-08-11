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

   // Operators
   DO_INIT(ADD_GBLREG, 1);
   DO_INIT(ADD_MAPARR, 1);
   DO_INIT(ADD_WLDARR, 1);
   DO_INIT(ADD_GBLARR, 1);
   DO_INIT(AND_REG,    1);
   DO_INIT(AND_MAPREG, 1);
   DO_INIT(AND_WLDREG, 1);
   DO_INIT(AND_GBLREG, 1);
   DO_INIT(AND_MAPARR, 1);
   DO_INIT(AND_WLDARR, 1);
   DO_INIT(AND_GBLARR, 1);
   DO_INIT(DEC_GBLREG, 1);
   DO_INIT(DEC_MAPARR, 1);
   DO_INIT(DEC_WLDARR, 1);
   DO_INIT(DEC_GBLARR, 1);
   DO_INIT(DIV_STK_X,  0);
   DO_INIT(DIV_GBLREG, 1);
   DO_INIT(DIV_MAPARR, 1);
   DO_INIT(DIV_WLDARR, 1);
   DO_INIT(DIV_GBLARR, 1);
   DO_INIT(INC_GBLREG, 1);
   DO_INIT(INC_MAPARR, 1);
   DO_INIT(INC_WLDARR, 1);
   DO_INIT(INC_GBLARR, 1);
   DO_INIT(IOR_REG,    1);
   DO_INIT(IOR_MAPREG, 1);
   DO_INIT(IOR_WLDREG, 1);
   DO_INIT(IOR_GBLREG, 1);
   DO_INIT(IOR_MAPARR, 1);
   DO_INIT(IOR_WLDARR, 1);
   DO_INIT(IOR_GBLARR, 1);
   DO_INIT(LSH_REG,    1);
   DO_INIT(LSH_MAPREG, 1);
   DO_INIT(LSH_WLDREG, 1);
   DO_INIT(LSH_GBLREG, 1);
   DO_INIT(LSH_MAPARR, 1);
   DO_INIT(LSH_WLDARR, 1);
   DO_INIT(LSH_GBLARR, 1);
   DO_INIT(MOD_GBLREG, 1);
   DO_INIT(MOD_MAPARR, 1);
   DO_INIT(MOD_WLDARR, 1);
   DO_INIT(MOD_GBLARR, 1);
   DO_INIT(MUL_STK_X,  0);
   DO_INIT(MUL_GBLREG, 1);
   DO_INIT(MUL_MAPARR, 1);
   DO_INIT(MUL_WLDARR, 1);
   DO_INIT(MUL_GBLARR, 1);
   DO_INIT(RSH_REG,    1);
   DO_INIT(RSH_MAPREG, 1);
   DO_INIT(RSH_WLDREG, 1);
   DO_INIT(RSH_GBLREG, 1);
   DO_INIT(RSH_MAPARR, 1);
   DO_INIT(RSH_WLDARR, 1);
   DO_INIT(RSH_GBLARR, 1);
   DO_INIT(SUB_GBLREG, 1);
   DO_INIT(SUB_MAPARR, 1);
   DO_INIT(SUB_WLDARR, 1);
   DO_INIT(SUB_GBLARR, 1);
   DO_INIT(XOR_REG,    1);
   DO_INIT(XOR_MAPREG, 1);
   DO_INIT(XOR_WLDREG, 1);
   DO_INIT(XOR_GBLREG, 1);
   DO_INIT(XOR_MAPARR, 1);
   DO_INIT(XOR_WLDARR, 1);
   DO_INIT(XOR_GBLARR, 1);
   DO_INIT(INV_STK,    0);

   // Jumps
   DO_INIT(JMP_CAL,         0);
   DO_INIT(JMP_CAL_IMM,     1);
   DO_INIT(JMP_CAL_NIL_IMM, 1);
   DO_INIT(JMP_RET,         0);
   DO_INIT(JMP_RET_NIL,     0);
   DO_INIT(JMP_TAB,        -1);

   // Stack-ops
   DO_INIT(STK_COPY, 0);
   DO_INIT(STK_SWAP, 0);

   // Trigonometry
   DO_INIT(TRIG_COS, 0);
   DO_INIT(TRIG_SIN, 0);

   // Variable Get
   DO_INIT(GET_FUNCP,  1);
   DO_INIT(GET_GBLREG, 1);
   DO_INIT(GET_MAPARR, 1);
   DO_INIT(GET_WLDARR, 1);
   DO_INIT(GET_GBLARR, 1);

   // Variable Set
   DO_INIT(SET_GBLREG, 1);
   DO_INIT(SET_MAPARR, 1);
   DO_INIT(SET_WLDARR, 1);
   DO_INIT(SET_GBLARR, 1);

   // Miscellaneous
   DO_INIT(NATIVE, 2);

   // ACS Extensions
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
   DO_INIT(PRINT_END_HUD,      0);
   DO_INIT(PRINT_END_HUD_BOLD, 0);
   DO_INIT(PRINT_END_LOG,      0);
   DO_INIT(PRINT_END_OPT,      0);
   DO_INIT(PRINT_END_STR,      0);
   DO_INIT(PRINT_KEYBIND,      0);
   DO_INIT(PRINT_NUM_BIN,      0);
   DO_INIT(PRINT_NUM_DEC_X,    0);
   DO_INIT(PRINT_NUM_HEX,      0);
   DO_INIT(PRINT_PLAYER_NAME,  0);
   DO_INIT(PRINT_SET_FONT,     0);
   DO_INIT(PRINT_SET_FONT_IMM, 1);
   DO_INIT(PRINT_START_OPT,    0);
   DO_INIT(PRINT_STR_GBLARR,   0);
   DO_INIT(PRINT_STR_GBLRNG,   0);
   DO_INIT(PRINT_STR_LOCALIZED,0);
   DO_INIT(PRINT_STR_MAPARR,   0);
   DO_INIT(PRINT_STR_MAPRNG,   0);
   DO_INIT(PRINT_STR_WLDARR,   0);
   DO_INIT(PRINT_STR_WLDRNG,   0);

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
   case BCODE_JMP_TAB:
      return args.size()*4 + 8;

   default:
      ERROR(position, "???");
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
   case BCODE_JMP_TAB:
      BinaryTokenACS::write_ACS0_32(out, BCODE_JMP_TAB);
      BinaryTokenACS::write_ACS0_32(out, args.size() / 2);
      for(size_t i = 0; i < args.size(); i += 2)
      {
         BinaryTokenACS::write_ACS0_32(out, *args[i+0]);
         BinaryTokenACS::write_ACS0_32(out, *args[i+1]);
      }
      break;

   default:
      ERROR(position, "???");
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

   switch(Output)
   {
   case OUTPUT_ACS0:
      BinaryTokenACS::output_ACS0(out, instructions);
      break;

   case OUTPUT_ACSE:
      output_ACSE(out, instructions);
      break;

   default:
      ERROR_p("unknown output type");
   }
}

// EOF

