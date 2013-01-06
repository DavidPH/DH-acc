//-----------------------------------------------------------------------------
//
// Copyright(C) 2012-2013 David Hill
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

#include "ObjectArchive.hpp"
#include "ObjectExpression.hpp"
#include "option.hpp"
#include "SourcePosition.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bool Option_UseChunkATAG = false;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_dptr<bool> Option_UseChunkATAG_Handler('\0',
   "use-chunk-ATAG", "features", "Enables generation of the ATAG chunk for "
   "static initialization of arrays with disparate tagging requirements. On by "
   "default.", NULL, &Option_UseChunkATAG);


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace ObjectData
{

//
// SetInit
//
// This one recursively scans through type, consuming dataVec and writing to
// initData and initType.
//
static void SetInit(InitDataVector::iterator &initData, InitTypeVector::iterator &initType,
   ObjectExpression::Vector::iterator &dataVec, VariableType const *type, bool hasData)
{
   switch(type->getBasicType())
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
      if(hasData) *initData++ = *dataVec++;
      *initType++ = IT_INTEGER;
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
      if(hasData)
      {
         ++dataVec; // TODO
         *initData++ = NULL;
         *initData++ = NULL;
      }
      *initType++ = IT_INTEGER;
      *initType++ = IT_INTEGER;
      break;

   case VariableType::BT_LABEL:
      if(hasData) *initData++ = *dataVec++;
      *initType++ = IT_INTEGER;
      break;

   case VariableType::BT_STR:
      if(hasData) *initData++ = *dataVec++;
      *initType++ = IT_STRING;
      break;

   case VariableType::BT_ARR:
      for(bigsint i = type->getWidth(); i--;)
         SetInit(initData, initType, dataVec, type->getReturn(), hasData);
      break;

   case VariableType::BT_PTR:
      if(VariableType::IsTypeFunction(type->getReturn()->getBasicType()))
      {
         SetInit(initData, initType, dataVec, type->getReturn(), hasData);
         break;
      }

      switch(type->getReturn()->getStoreType())
      {
      case STORE_FAR:
         if(hasData)
         {
            ++dataVec; // TODO
            *initData++ = NULL;
            *initData++ = NULL;
         }
         *initType++ = IT_INTEGER;
         *initType++ = IT_INTEGER;
         break;

      case STORE_STRING:
         if(hasData)
         {
            ++dataVec; // TODO
            *initData++ = NULL;
            *initData++ = NULL;
         }
         *initType++ = IT_STRING;
         *initType++ = IT_INTEGER;
         break;

      default:
         if(hasData) *initData++ = *dataVec++;
         *initType++ = IT_INTEGER;
         break;
      }
      break;

   case VariableType::BT_PTR_NUL:
      if(hasData) *initData++ = *dataVec++;
      *initType++ = IT_INTEGER;
      break;

   case VariableType::BT_ENUM:
      if(hasData) *initData++ = *dataVec++;
      *initType++ = IT_INTEGER;
      break;

   case VariableType::BT_CLX:
   case VariableType::BT_CLX_IM:
   case VariableType::BT_SAT:
   case VariableType::BT_STRUCT:
   case VariableType::BT_BLOCK:
      for(VariableType::Vector::const_iterator end = type->getTypes().end(),
          itr = type->getTypes().begin(); itr != end; ++itr)
      {
         SetInit(initData, initType, dataVec, *itr, hasData);
      }
      break;

   case VariableType::BT_UNION:
      if(hasData) ++dataVec; // TODO
      for(bigsint i = type->getSize(SourcePosition::none()); i--;)
      {
         if(hasData) *initData++ = NULL;
         *initType++ = IT_INTEGER;
      }
      break;

   case VariableType::BT_FUN_ASM:
      if(hasData) ++dataVec; // TODO
      break;

   case VariableType::BT_FUN:
      if(hasData) *initData++ = *dataVec++;
      *initType++ = IT_FUNCTION;
      break;

   case VariableType::BT_FUN_LIN:
   case VariableType::BT_FUN_NAT:
   case VariableType::BT_FUN_SNU:
      if(hasData) *initData++ = *dataVec++;
      *initType++ = IT_INTEGER;
      break;

   case VariableType::BT_FUN_SNA:
      if(hasData) *initData++ = *dataVec++;
      *initType++ = IT_STRING;
      break;
   }
}

}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace ObjectData
{

//
// ObjectData::SetInit
//
void SetInit(Init &init, ObjectExpression *data, VariableType const *type)
{
   init.data.clear();
   init.type.clear();
   init.dataAll = false;
   init.typeAll = IT_UNKNOWN;

   ObjectExpression::Vector dataVec;
   if(data)
   {
      data->expand(&dataVec);
      init.data.resize(type->getSize(SourcePosition::none()));
      dataVec.resize(init.data.size());
   }
   init.type.resize(type->getSize(SourcePosition::none()));

   InitDataVector::iterator dataItr = init.data.begin();
   InitTypeVector::iterator typeItr = init.type.begin();
   ObjectExpression::Vector::iterator dataVecItr = dataVec.begin();

   SetInit(dataItr, typeItr, dataVecItr, type, !!data);

   // Check if the data is all there.
   init.dataAll = true;
   for(InitDataVector::iterator itr = init.data.begin(), end = init.data.end(); itr != end; ++itr)
   {
      if(!*itr)
      {
         init.dataAll = false;
         break;
      }
   }

   // Check if the types are all the same.
   for(InitTypeVector::iterator itr = init.type.begin(), end = init.type.end(); itr != end; ++itr)
   {
      if(*itr == IT_UNKNOWN || *itr == init.typeAll) continue;

      if(init.typeAll == IT_UNKNOWN)
      {
         init.typeAll = *itr;
      }
      else
      {
         init.typeAll = IT_MIXED;
         break;
      }
   }
}

}

//
// operator ObjectArchive << ObjectData::Init
//
ObjectArchive &operator << (ObjectArchive &arc, ObjectData::Init &data)
{
   return arc << data.data << data.type << data.dataAll << data.typeAll;
}

//
// operator ObjectArchive << ObjectData::InitType
//
ObjectArchive &operator << (ObjectArchive &arc, ObjectData::InitType &data)
{
   return arc.archiveEnum(data, ObjectData::IT_FUNCTION, ObjectData::IT_UNKNOWN);
}

// EOF

