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
// DS handling of store-types.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenizerDS.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::is_store
//
bool SourceExpressionDS::is_store(std::string const &data)
{
   if (data == "static")
      return true;

   if (data == "__staticregister")
      return true;

   if (data == "__staticarray")
      return true;

   if (data == "auto")
      return true;

   if (data == "__autoregister")
      return true;

   if (data == "__autoarray")
      return true;

   if (data == "register")
      return true;

   if (data == "__mapregister")
      return true;

   if (data == "__worldregister")
      return true;

   if (data == "__globalregister")
      return true;

   if (data == "__maparray")
      return true;

   if (data == "__worldarray")
      return true;

   if (data == "__globalarray")
      return true;

   return false;
}

//
// SourceExpressionDS::make_store
//
StoreType SourceExpressionDS::make_store(SourceTokenizerDS *in, Vector *blocks,
   SourceContext *context, ObjectExpression::Pointer *area)
{
   SourceTokenC typeTok = in->get(SourceTokenC::TT_NAM);

   if (in->peekType(SourceTokenC::TT_PAREN_O))
   {
      in->get(SourceTokenC::TT_PAREN_O);
      *area = make_expression(in, blocks, context)->makeObject();
      in->get(SourceTokenC::TT_PAREN_C);
   }

   if (typeTok.data == "static")
      return STORE_STATIC;

   if (typeTok.data == "__staticregister")
      return store_staticregister();

   if (typeTok.data == "__staticarray")
      return store_staticarray();

   if (typeTok.data == "auto")
      return STORE_AUTO;

   if (typeTok.data == "__autoregister")
      return store_autoregister();

   if (typeTok.data == "__autoarray")
      return store_autoarray();

   if (typeTok.data == "register")
      return STORE_REGISTER;

   if (typeTok.data == "__mapregister")
      return STORE_MAPREGISTER;

   if (typeTok.data == "__worldregister")
      return STORE_WORLDREGISTER;

   if (typeTok.data == "__globalregister")
      return STORE_GLOBALREGISTER;

   if (typeTok.data == "__maparray")
      return STORE_MAPARRAY;

   if (typeTok.data == "__worldarray")
      return STORE_WORLDARRAY;

   if (typeTok.data == "__globalarray")
      return STORE_GLOBALARRAY;

   ERROR(typeTok.pos, "unknown storage class '%s'", typeTok.data.c_str());
}

// EOF

