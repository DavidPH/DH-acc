//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// Object-level data.
//
//-----------------------------------------------------------------------------

#include "ObjectData.hpp"

#include "SourcePosition.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// odata_set_strings
//
void odata_set_strings(std::vector<int> &strings, VariableType const *type)
{
   switch (type->getBasicType())
   {
   case VariableType::BT_VOID:
      break;

   case VariableType::BT_BIT_HRD:
   case VariableType::BT_BIT_SFT:
   case VariableType::BT_CHR:
   case VariableType::BT_ACC_HH:
   case VariableType::BT_ACC_H:
   case VariableType::BT_ACC:
   case VariableType::BT_ANG_HH:
   case VariableType::BT_ANG_H:
   case VariableType::BT_ANG:
   case VariableType::BT_ANG_L:
   case VariableType::BT_ANG_LL:
   case VariableType::BT_FIX_HH:
   case VariableType::BT_FIX_H:
   case VariableType::BT_FIX:
   case VariableType::BT_FLT_HH:
   case VariableType::BT_FLT_H:
   case VariableType::BT_FLT:
   case VariableType::BT_FRA_HH:
   case VariableType::BT_FRA_H:
   case VariableType::BT_FRA:
   case VariableType::BT_FRA_L:
   case VariableType::BT_FRA_LL:
   case VariableType::BT_INT_HH:
   case VariableType::BT_INT_H:
   case VariableType::BT_INT:
   case VariableType::BT_UNS_HH:
   case VariableType::BT_UNS_H:
   case VariableType::BT_UNS:
      strings.push_back(false);
      break;

   case VariableType::BT_ACC_L:
   case VariableType::BT_ACC_LL:
   case VariableType::BT_FIX_L:
   case VariableType::BT_FIX_LL:
   case VariableType::BT_FLT_L:
   case VariableType::BT_FLT_LL:
   case VariableType::BT_INT_L:
   case VariableType::BT_INT_LL:
   case VariableType::BT_UNS_L:
   case VariableType::BT_UNS_LL:
      strings.push_back(false);
      strings.push_back(false);
      break;

   case VariableType::BT_LABEL:
      strings.push_back(false);
      break;

   case VariableType::BT_STR:
      strings.push_back(true);
      break;

   case VariableType::BT_ARR:
      for (bigsint i = type->getWidth(); i--;)
         odata_set_strings(strings, type->getReturn());
      break;

   case VariableType::BT_PTR:
      if(VariableType::is_bt_function(type->getReturn()->getBasicType()))
      {
         odata_set_strings(strings, type->getReturn());
         break;
      }

      switch(type->getReturn()->getStoreType())
      {
      case STORE_NONE:
         strings.push_back(false);
         strings.push_back(false);
         break;

      case STORE_STRING:
         strings.push_back(true);
         strings.push_back(false);
         break;

      default:
         strings.push_back(false);
         break;
      }
      break;

   case VariableType::BT_PTR_NUL:
      strings.push_back(false);
      break;

   case VariableType::BT_ENUM:
      strings.push_back(false);
      break;

   case VariableType::BT_CLX:
   case VariableType::BT_CLX_IM:
   case VariableType::BT_SAT:
   case VariableType::BT_STRUCT:
   case VariableType::BT_BLOCK:
      for (VariableType::Vector::const_iterator end = type->getTypes().end(),
           itr = type->getTypes().begin(); itr != end; ++itr)
         odata_set_strings(strings, *itr);
      break;

   case VariableType::BT_UNION:
      for (bigsint i = type->getSize(SourcePosition::none()); i--;)
         strings.push_back(false);
      break;

   case VariableType::BT_FUN_ASM:
      break;

   case VariableType::BT_FUN:
   case VariableType::BT_FUN_LIN:
   case VariableType::BT_FUN_NAT:
   case VariableType::BT_FUN_SNU:
      strings.push_back(false);
      break;

   case VariableType::BT_FUN_SNA:
      strings.push_back(true);
      break;
   }
}

// EOF

