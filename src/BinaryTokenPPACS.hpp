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
   enum BinaryCode
   {
      BINTOKACS_BCODES,

      BCODE_SET_STATIC        = 102,
      BCODE_SET_AUTO          = 103,
      BCODE_SET_POINTER       = 104,
      BCODE_GET_STATIC        = 105,
      BCODE_GET_AUTO          = 106,
      BCODE_GET_POINTER       = 107,
      BCODE_SETOP_ADD_STATIC  = 108,
      BCODE_SETOP_ADD_AUTO    = 109,
      BCODE_SETOP_ADD_POINTER = 110,
      BCODE_SETOP_SUB_STATIC  = 111,
      BCODE_SETOP_SUB_AUTO    = 112,
      BCODE_SETOP_SUB_POINTER = 113,
      BCODE_SETOP_MUL_STATIC  = 114,
      BCODE_SETOP_MUL_AUTO    = 115,
      BCODE_SETOP_MUL_POINTER = 116,
      BCODE_SETOP_DIV_STATIC  = 117,
      BCODE_SETOP_DIV_AUTO    = 118,
      BCODE_SETOP_DIV_POINTER = 119,
      BCODE_SETOP_MOD_STATIC  = 120,
      BCODE_SETOP_MOD_AUTO    = 121,
      BCODE_SETOP_MOD_POINTER = 122,
      BCODE_SETOP_INC_STATIC  = 123,
      BCODE_SETOP_INC_AUTO    = 124,
      BCODE_SETOP_INC_POINTER = 125,
      BCODE_SETOP_DEC_STATIC  = 126,
      BCODE_SETOP_DEC_AUTO    = 127,
      BCODE_SETOP_DEC_POINTER = 128,
      BCODE_ADDR_AUTO         = 129,
      BCODE_ADDR_STACK_ADD    = 130,
      BCODE_ADDR_STACK_SUB    = 131,
      BCODE_BRANCH_GOTO       = 132,
      BCODE_THING_SET_MID     = 133,
      BCODE_THING_GET_MID     = 134,
      BCODE_MID_SET           = 135,
      BCODE_MID_GET           = 136,
      BCODE_MUL_FIXED         = 137,
      BCODE_DIV_FIXED         = 138,
      BCODE_PRINT_FIXED       = 139,
      BCODE_PRINT_END_LOG     = 140,
      BCODE_PRINT_END_ERROR   = 141,

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

