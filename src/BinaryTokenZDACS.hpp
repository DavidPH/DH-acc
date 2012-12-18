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

#ifndef HPP_BinaryTokenZDACS_
#define HPP_BinaryTokenZDACS_

#include "BinaryTokenACS.hpp"
#include "bignum.hpp"
#include "Counter.hpp"
#include "SourcePosition.hpp"

#include <ostream>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ObjectData
{
struct Array;
struct Function;
struct Label;
struct Register;
struct Script;
struct String;
}

class ObjectExpression;
class ObjectToken;
class ObjectVector;

//
// BinaryTokenZDACS
//
class BinaryTokenZDACS
{
public:
   typedef BinaryTokenZDACS This;

   enum BinaryCode
   {
      BINTOKACS_BCODES,

      BCODE_SOUND_THING                        = 102,
      BCODE_SOUND_AMBIENTLOCAL                 = 103,
      BCODE_LTAG_SET_BLOCKMONSTER              = 104,
      // Abyss of Abandoned Codes                     // ST
      BCODE_THING_GET_TEAM                     = 119, // ST
      BCODE_THING_GET_HEALTH                   = 120, // ST
      BCODE_THING_GET_ARMOR                    = 121, // ST
      BCODE_THING_GET_FRAGS                    = 122, // ST
      // Abyss of Abandoned Codes                     // ST
      BCODE_GAME_GET_TEAMINFO_PLAYERCOUNT_BLUE = 124, // ST
      BCODE_GAME_GET_TEAMINFO_PLAYERCOUNT_RED  = 125, // ST
      BCODE_GAME_GET_TEAMINFO_SCORE_BLUE       = 126, // ST
      BCODE_GAME_GET_TEAMINFO_SCORE_RED        = 127, // ST
      BCODE_GAME_GET_TYPE_ONEFLAGCTF           = 128, // ST
      BCODE_GAME_GET_INVASIONWAVE              = 129, // ST
      BCODE_GAME_GET_INVASIONSTATE             = 130, // ST
      BCODE_PRINT_PLAYER_NAME                  = 131, // ST
      BCODE_GAME_SET_MUSICST                   = 132, // ST
      BCODE_GAME_EXEC_IMM                      = 133, // ST
      BCODE_GAME_EXEC                          = 134, // ST
      BCODE_GAME_GET_TYPE_SINGLEPLAYER         = 135, // ST
      BCODE_MUL_STK_X                          = 136,
      BCODE_DIV_STK_X                          = 137,
      BCODE_GAME_SET_GRAVITY                   = 138,
      BCODE_GAME_SET_GRAVITY_IMM               = 139,
      BCODE_GAME_SET_AIRCONTROL                = 140,
      BCODE_GAME_SET_AIRCONTROL_IMM            = 141,
      BCODE_THING_CLR_INVENTORY                = 142,
      BCODE_THING_ADD_INVENTORY                = 143,
      BCODE_THING_ADD_INVENTORY_IMM            = 144,
      BCODE_THING_SUB_INVENTORY                = 145,
      BCODE_THING_SUB_INVENTORY_IMM            = 146,
      BCODE_THING_GET_INVENTORY                = 147,
      BCODE_THING_GET_INVENTORY_IMM            = 148,
      BCODE_SPAWN_POINT                        = 149,
      BCODE_SPAWN_POINT_IMM                    = 150,
      BCODE_SPAWN_SPOT_ANGLE                   = 151,
      BCODE_SPAWN_SPOT_ANGLE_IMM               = 152,
      BCODE_GAME_SET_MUSIC                     = 153,
      BCODE_GAME_SET_MUSIC_IMM                 = 154,
      BCODE_GAME_SET_MUSICLOCAL                = 155,
      BCODE_GAME_SET_MUSICLOCAL_IMM            = 156,
      BCODE_PRINT_NUM_DEC_X                    = 157,
      BCODE_PRINT_STR_LOCALIZED                = 158,
      BCODE_PRINT_START_OPT                    = 159,
      BCODE_PRINT_END_OPT                      = 160,
      BCODE_PRINT_END_HUD                      = 161,
      BCODE_PRINT_END_HUD_BOLD                 = 162,
      // Abyss of Abandoned Codes
      BCODE_PRINT_SET_FONT                     = 165,
      BCODE_PRINT_SET_FONT_IMM                 = 166,
      // Abyss of Unusable Codes
      BCODE_MTAG_SET_SPECIAL                   = 180,
      BCODE_SET_GBLREG                         = 181,
      BCODE_GET_GBLREG                         = 182,
      BCODE_ADD_GBLREG                         = 183,
      BCODE_SUB_GBLREG                         = 184,
      BCODE_MUL_GBLREG                         = 185,
      BCODE_DIV_GBLREG                         = 186,
      BCODE_MOD_GBLREG                         = 187,
      BCODE_INC_GBLREG                         = 188,
      BCODE_DEC_GBLREG                         = 189,
      BCODE_SCREEN_FADE_START                  = 190,
      BCODE_SCREEN_FADE_RANGE                  = 191,
      BCODE_SCREEN_FADE_STOP                   = 192,
      BCODE_MISC_PLAYMOVIE                     = 193,
      BCODE_STAG_SET_TRIGGER_FLOOR             = 194,
      BCODE_STAG_SET_TRIGGER_CEILING           = 195,
      BCODE_MTAG_GET_X                         = 196,
      BCODE_MTAG_GET_Y                         = 197,
      BCODE_MTAG_GET_Z                         = 198,
      BCODE_TRANSLATION_START                  = 199,
      BCODE_TRANSLATION_PALETTE                = 200,
      BCODE_TRANSLATION_RGB                    = 201,
      BCODE_TRANSLATION_END                    = 202,
      BCODE_JMP_CAL_IMM                        = 203,
      BCODE_JMP_CAL_NIL_IMM                    = 204,
      BCODE_JMP_RET_NIL                        = 205,
      BCODE_JMP_RET                            = 206,
      BCODE_GET_MAPARR                         = 207,
      BCODE_SET_MAPARR                         = 208,
      BCODE_ADD_MAPARR                         = 209,
      BCODE_SUB_MAPARR                         = 210,
      BCODE_MUL_MAPARR                         = 211,
      BCODE_DIV_MAPARR                         = 212,
      BCODE_MOD_MAPARR                         = 213,
      BCODE_INC_MAPARR                         = 214,
      BCODE_DEC_MAPARR                         = 215,
      BCODE_STK_COPY                           = 216,
      BCODE_STK_SWAP                           = 217,
      // Abyss of Abandoned Codes
      BCODE_TRIG_SIN                           = 220,
      BCODE_TRIG_COS                           = 221,
      BCODE_TRIG_VECTORANGLE                   = 222,
      BCODE_THING_CHK_WEAPON                   = 223,
      BCODE_THING_SET_WEAPON                   = 224,
      BCODE_STRING_TAG                         = 225,
      BCODE_GET_WLDARR                         = 226,
      BCODE_SET_WLDARR                         = 227,
      BCODE_ADD_WLDARR                         = 228,
      BCODE_SUB_WLDARR                         = 229,
      BCODE_MUL_WLDARR                         = 230,
      BCODE_DIV_WLDARR                         = 231,
      BCODE_MOD_WLDARR                         = 232,
      BCODE_INC_WLDARR                         = 233,
      BCODE_DEC_WLDARR                         = 234,
      BCODE_GET_GBLARR                         = 235,
      BCODE_SET_GBLARR                         = 236,
      BCODE_ADD_GBLARR                         = 237,
      BCODE_SUB_GBLARR                         = 238,
      BCODE_MUL_GBLARR                         = 239,
      BCODE_DIV_GBLARR                         = 240,
      BCODE_MOD_GBLARR                         = 241,
      BCODE_INC_GBLARR                         = 242,
      BCODE_DEC_GBLARR                         = 243,
      BCODE_MTAG_SET_MARINEWEAPON              = 244,
      BCODE_MTAG_SET                           = 245,
      BCODE_MTAG_GET                           = 246,
      BCODE_THING_GET_PLAYERNUMBER             = 247,
      BCODE_THING_GET_MTAG                     = 248,
      BCODE_MTAG_SET_MARINESPRITE              = 249,
      BCODE_SCREEN_GET_WIDTH                   = 250,
      BCODE_SCREEN_GET_HEIGHT                  = 251,
      BCODE_SPAWN_PROJECTILE_SID               = 252,
      BCODE_STRING_GET_LENGTH                  = 253,
      BCODE_SCREEN_SET_HUDSIZE                 = 254,
      BCODE_GAME_GET_CVAR                      = 255,
      BCODE_JMP_TAB                            = 256,
      BCODE_SET_SCRRET                         = 257,
      BCODE_LINE_GET_OFFSETY                   = 258,
      BCODE_MTAG_GET_FLOORZ                    = 259,
      BCODE_MTAG_GET_ANGLE                     = 260,
      BCODE_STAG_GET_Z_FLOOR                   = 261,
      BCODE_STAG_GET_Z_CEILING                 = 262,
      BCODE_SPECIAL_EXEC5_RETN1                = 263,
      BCODE_THING_GET_SIGIL                    = 264,
      BCODE_GAME_GET_LEVELINFO                 = 265,
      BCODE_GAME_SET_SKY                       = 266,
      BCODE_PLAYER_GET_INGAME                  = 267,
      BCODE_PLAYER_GET_ISBOT                   = 268,
      BCODE_MTAG_SET_CAMERATEXTURE             = 269,
      BCODE_PRINT_END_LOG                      = 270,
      BCODE_THING_GET_AMMOCAP                  = 271,
      BCODE_THING_SET_AMMOCAP                  = 272,
      BCODE_PRINT_STR_MAPARR                   = 273,
      BCODE_PRINT_STR_WLDARR                   = 274,
      BCODE_PRINT_STR_GBLARR                   = 275,
      BCODE_MTAG_SET_ANGLE                     = 276,
      // Abyss of Abandoned Codes
      BCODE_SPAWN_PROJECTILE_STR               = 280,
      BCODE_STAG_GET_LIGHTLEVEL                = 281,
      BCODE_MTAG_GET_CEILINGZ                  = 282,
      BCODE_MTAG_SET_XYZ                       = 283,
      BCODE_MTAG_CLR_INVENTORY                 = 284,
      BCODE_MTAG_ADD_INVENTORY                 = 285,
      BCODE_MTAG_SUB_INVENTORY                 = 286,
      BCODE_MTAG_GET_INVENTORY                 = 287,
      BCODE_GAME_GET_THINGCOUNT_STR            = 288,
      BCODE_SPAWN_SPOT                         = 289,
      BCODE_PLAYER_GET_CLASS                   = 290,
      BCODE_AND_REG                            = 291,
      BCODE_AND_MAPREG                         = 292,
      BCODE_AND_WLDREG                         = 293,
      BCODE_AND_GBLREG                         = 294,
      BCODE_AND_MAPARR                         = 295,
      BCODE_AND_WLDARR                         = 296,
      BCODE_AND_GBLARR                         = 297,
      BCODE_XOR_REG                            = 298,
      BCODE_XOR_MAPREG                         = 299,
      BCODE_XOR_WLDREG                         = 300,
      BCODE_XOR_GBLREG                         = 301,
      BCODE_XOR_MAPARR                         = 302,
      BCODE_XOR_WLDARR                         = 303,
      BCODE_XOR_GBLARR                         = 304,
      BCODE_IOR_REG                            = 305,
      BCODE_IOR_MAPREG                         = 306,
      BCODE_IOR_WLDREG                         = 307,
      BCODE_IOR_GBLREG                         = 308,
      BCODE_IOR_MAPARR                         = 309,
      BCODE_IOR_WLDARR                         = 310,
      BCODE_IOR_GBLARR                         = 311,
      BCODE_LSH_REG                            = 312,
      BCODE_LSH_MAPREG                         = 313,
      BCODE_LSH_WLDREG                         = 314,
      BCODE_LSH_GBLREG                         = 315,
      BCODE_LSH_MAPARR                         = 316,
      BCODE_LSH_WLDARR                         = 317,
      BCODE_LSH_GBLARR                         = 318,
      BCODE_RSH_REG                            = 319,
      BCODE_RSH_MAPREG                         = 320,
      BCODE_RSH_WLDREG                         = 321,
      BCODE_RSH_GBLREG                         = 322,
      BCODE_RSH_MAPARR                         = 323,
      BCODE_RSH_WLDARR                         = 324,
      BCODE_RSH_GBLARR                         = 325,
      BCODE_PLAYER_GET_INFO                    = 326,
      BCODE_GAME_SET_LEVEL                     = 327,
      BCODE_STAG_DAMAGE                        = 328,
      BCODE_GAME_REPLACETEXTURES               = 329,
      BCODE_INV_STK                            = 330,
      BCODE_MTAG_GET_PITCH                     = 331,
      BCODE_MTAG_SET_PITCH                     = 332,
      BCODE_PRINT_KEYBIND                      = 333,
      BCODE_MTAG_SET_STATE                     = 334,
      BCODE_MTAG_DAMAGE                        = 335,
      BCODE_THING_USEINVENTORY                 = 336,
      BCODE_MTAG_USEINVENTORY                  = 337,
      BCODE_MTAG_CHK_TEXTURE_CEILING           = 338,
      BCODE_MTAG_CHK_TEXTURE_FLOOR             = 339,
      BCODE_MTAG_GET_LIGHTLEVEL                = 340,
      BCODE_THING_SET_MUGSHOT                  = 341,
      BCODE_STAG_GET_THINGCOUNT_SID            = 342,
      BCODE_STAG_GET_THINGCOUNT_STR            = 343,
      BCODE_PLAYER_GET_CAMERA                  = 344,
      BCODE_MTAG_MORPH                         = 345,
      BCODE_MTAG_UNMORPH                       = 346,
      BCODE_PLAYER_GET_INPUT                   = 347,
      BCODE_MTAG_GET_CLASSIFICATION            = 348,
      BCODE_PRINT_NUM_BIN                      = 349,
      BCODE_PRINT_NUM_HEX                      = 350,
      BCODE_NATIVE                             = 351,
      BCODE_PRINT_END_STR                      = 352,
      BCODE_PRINT_STR_MAPRNG                   = 353,
      BCODE_PRINT_STR_WLDRNG                   = 354,
      BCODE_PRINT_STR_GBLRNG                   = 355,
      BCODE_STRING_COPY_MAPRANGE               = 356,
      BCODE_STRING_COPY_WORLDRANGE             = 357,
      BCODE_STRING_COPY_GLOBALRANGE            = 358,
      BCODE_GET_FUNCP                          = 359,
      BCODE_JMP_CAL                            = 360,
      BCODE_WAIT_SNAM                          = 361,
      BCODE_TRANSLATION_RGBA                   = 362,
      BCODE_JMP                                = 363,

      BCODE_NONE
   };

   typedef std::vector<std::string>::const_iterator label_iterator;



   BinaryTokenZDACS(BinaryCode code, SourcePosition const &position,
                    std::vector<std::string> const &labels,
                    std::vector<CounterPointer<ObjectExpression> > const &args);

   void addLabel(std::string const & label);

   size_t getArgCount() const;

   label_iterator label_begin() const;
   label_iterator label_end() const;

   size_t size() const;

   void writeACS0(std::ostream * const out) const;



   static void init();

   static void make_tokens(ObjectToken const & object, std::vector<BinaryTokenZDACS> *instructions);
   static void make_tokens(ObjectVector const & objects, std::vector<BinaryTokenZDACS> *instructions);

   template<typename T>
   static void output_ACSE(std::ostream *out, std::vector<T> const &instructions);

   static void write_ACSE_array_ARAY(std::ostream *out, ObjectData::Array const &a);
   static void write_ACSE_array_AIMP(std::ostream *out, ObjectData::Array const &a);
   static void write_ACSE_array_AIMP_counter(std::ostream *out, ObjectData::Array const &a);
   static void write_ACSE_array_AINI(std::ostream *out, ObjectData::Array const &a);
   static void write_ACSE_array_ASTR(std::ostream *out, ObjectData::Array const &a);
   static void write_ACSE_array_ATAG(std::ostream *out, ObjectData::Array const &a);
   static void write_ACSE_array_MEXP(std::ostream *out, ObjectData::Array const &a);
   static void write_ACSE_chunk(std::ostream *out, std::ostringstream *chunkout, char const *chunkname);
   static void write_ACSE_counter(std::ostream *out);
   static void write_ACSE_function_FUNC(std::ostream *out, ObjectData::Function const &f);
   static void write_ACSE_function_FNAM(std::ostream *out, ObjectData::Function const &f);
   static void write_ACSE_label_JUMP(std::ostream *out, ObjectData::Label const &l);
   static void write_ACSE_library(std::ostream *out, std::string const &lib);
   static void write_ACSE_register_MEXP(std::ostream *out, ObjectData::Register const &r);
   static void write_ACSE_register_MIMP(std::ostream *out, ObjectData::Register const &r);
   static void write_ACSE_register_MINI(std::ostream *out, ObjectData::Register const &r);
   static void write_ACSE_register_MSTR(std::ostream *out, ObjectData::Register const &r);
   static void write_ACSE_script(std::ostream *out, ObjectData::Script const &s);
   static void write_ACSE_script_SFLG(std::ostream *out, ObjectData::Script const &s);
   static void write_ACSE_script_SNAM(std::ostream *out, ObjectData::Script const &s);
   static void write_ACSE_script_SVCT(std::ostream *out, ObjectData::Script const &s);
   static void write_ACSE_string_STRL(std::ostream *out, ObjectData::String const &s);
   static void write_ACSE_stringtable(std::ostream *out, bool junk);

   static void write_all(std::ostream *out, std::vector<BinaryTokenZDACS> const &instructions);

private:
   std::vector<CounterPointer<ObjectExpression> > args;
   BinaryCode code;
   std::vector<std::string> labels;
   SourcePosition position;



   static int arg_counts[BCODE_NONE];
};

#endif//HPP_BinaryTokenZDACS_

