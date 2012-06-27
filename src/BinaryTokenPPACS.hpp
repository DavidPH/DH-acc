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
// PPACS (Hex++ ACS) handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_BinaryTokenPPACS_
#define HPP_BinaryTokenPPACS_

#include "BinaryTokenACS.hpp"
#include "bignum.hpp"
#include "Counter.hpp"
#include "SourcePosition.hpp"

#include <ostream>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

struct ObjectData_Auto;
struct ObjectData_Script;
struct ObjectData_Static;
struct ObjectData_String;
class ObjectExpression;
class ObjectToken;
class ObjectVector;

//
// BinaryTokenPPACS
//
class BinaryTokenPPACS
{
public:
   typedef BinaryTokenPPACS This;

   enum BinaryCode
   {
      BINTOKACS_BCODES,

      BCODE_SET_STATIC      = 102,
      BCODE_SET_AUTO        = 103,
      BCODE_SET_PTR         = 104,
      BCODE_GET_STATIC      = 105,
      BCODE_GET_AUTO        = 106,
      BCODE_GET_PTR         = 107,
      BCODE_ADD_STATIC      = 108,
      BCODE_ADD_AUTO        = 109,
      BCODE_ADD_PTR         = 110,
      BCODE_SUB_STATIC      = 111,
      BCODE_SUB_AUTO        = 112,
      BCODE_SUB_PTR         = 113,
      BCODE_MUL_STATIC      = 114,
      BCODE_MUL_AUTO        = 115,
      BCODE_MUL_PTR         = 116,
      BCODE_DIV_STATIC      = 117,
      BCODE_DIV_AUTO        = 118,
      BCODE_DIV_PTR         = 119,
      BCODE_MOD_STATIC      = 120,
      BCODE_MOD_AUTO        = 121,
      BCODE_MOD_PTR         = 122,
      BCODE_INC_STATIC      = 123,
      BCODE_INC_AUTO        = 124,
      BCODE_INC_PTR         = 125,
      BCODE_DEC_STATIC      = 126,
      BCODE_DEC_AUTO        = 127,
      BCODE_DEC_PTR         = 128,
      BCODE_AND_STATIC      = 129,
      BCODE_AND_AUTO        = 130,
      BCODE_AND_PTR         = 131,
      BCODE_IOR_STATIC      = 132,
      BCODE_IOR_AUTO        = 133,
      BCODE_IOR_PTR         = 134,
      BCODE_XOR_STATIC      = 135,
      BCODE_XOR_AUTO        = 136,
      BCODE_XOR_PTR         = 137,
      BCODE_LSH_STATIC      = 138,
      BCODE_LSH_AUTO        = 139,
      BCODE_LSH_PTR         = 140,
      BCODE_RSH_STATIC      = 141,
      BCODE_RSH_AUTO        = 142,
      BCODE_RSH_PTR         = 143,
      BCODE_GET_AUTPTR_IMM  = 144,
      BCODE_ADD_AUTPTR      = 145,
      BCODE_SUB_AUTPTR      = 146,
      BCODE_JMP             = 147,
      BCODE_THING_SET_MID   = 148,
      BCODE_THING_GET_MID   = 149,
      BCODE_MID_SET         = 150,
      BCODE_MID_GET         = 151,
      BCODE_MUL_STK_X       = 152,
      BCODE_DIV_STK_X       = 153,
      BCODE_PRINT_NUM_DEC_X = 154,
      BCODE_PRINT_END_LOG   = 155,
      BCODE_PRINT_END_ERROR = 156,

      BCODE_NONE
   };

   typedef std::vector<std::string>::const_iterator label_iterator;



   BinaryTokenPPACS(BinaryCode code, SourcePosition const &position,
                    std::vector<std::string> const &labels,
                    std::vector<CounterPointer<ObjectExpression> > const &args);

   void addLabel(std::string const &label);

   size_t getArgCount() const;

   label_iterator label_begin() const;
   label_iterator label_end() const;

   size_t size() const;

   void writeACS0(std::ostream *out) const;
   void writeACSP(std::ostream *out) const;



   static void init();

   static void make_tokens(ObjectToken const & object, std::vector<BinaryTokenPPACS> *instructions);
   static void make_tokens(ObjectVector const & objects, std::vector<BinaryTokenPPACS> *instructions);

   template<typename T>
   static void output_ACSP(std::ostream *out, std::vector<T> const &instructions);

   static void write_ACSP_label(std::ostream *out, std::string const &label);
   static void write_ACSP_script(std::ostream *out, ObjectData_Script const &s);
   static void write_ACSP_static(std::ostream *out, ObjectData_Static const &s);
   static void write_ACSP_string(std::ostream *out, ObjectData_String const &s);
   static void write_ACSP_string(std::ostream *out, std::string const &s);

   static void write_all(std::ostream *out, std::vector<BinaryTokenPPACS> const &instructions);

private:
   std::vector<CounterPointer<ObjectExpression> > args;
   BinaryCode code;
   std::vector<std::string> labels;
   SourcePosition position;



   static int arg_counts[BCODE_NONE];
};

#endif//HPP_BinaryTokenPPACS_

