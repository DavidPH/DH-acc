//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// Object-format I/O.
//
//-----------------------------------------------------------------------------

#include "ObjectArchive.hpp"

#include "SourceExpression.hpp" // SourceExpression::ParseNumber

#include <cmath>


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// LoadInt
//
static biguint LoadInt(std::istream &load)
{
   biguint i = 0;

   for(int c; (c = load.get()) && load;)
      i = i * 16 + SourceExpression::ParseNumber(c);

   return i;
}

//
// LoadRealFrac
//
static bigreal LoadRealFrac(std::istream &load)
{
   int c = load.get();

   if(c == '\0') return 0;

   return (SourceExpression::ParseNumber(c) + LoadRealFrac(load)) / 16;
}

//
// LoadRealInt
//
static bigreal LoadRealInt(std::istream &load)
{
   bigreal i = 0;

   for(int c; (c = load.get()) != '.' && load;)
      i = i * 16 + SourceExpression::ParseNumber(c);

   return i;
}

//
// SaveInt
//
static void SaveInt(std::ostream &save, biguint i)
{
   if(!i) return;

   SaveInt(save, i / 16);
   save.put("0123456789ABCDEF"[i % 16]);
}

//
// SaveRealFrac
//
static void SaveRealFrac(std::ostream &save, bigreal f)
{
   for(; f; f = std::fmod(f, 1) * 16)
      save.put("0123456789ABCDEF"[static_cast<unsigned>(std::floor(f))]);
}

//
// SaveRealInt
//
static void SaveRealInt(std::ostream &save, bigreal i)
{
   if(!i) return;

   SaveRealInt(save, std::floor(i / 16));
   save.put("0123456789ABCDEF"[static_cast<unsigned>(std::floor(std::fmod(i, 16)))]);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectLoad::loadPrimBool
//
bool ObjectLoad::loadPrimBool()
{
   if(!load) return false;

   int c = load.get();

   while(load && load.get()) {}

   return c != '0';
}

//
// ObjectLoad::loadPrimChar
//
char ObjectLoad::loadPrimChar()
{
   if(!load) return '\0';

   return static_cast<char>(load.get());
}

//
// ObjectLoad::loadPrimReal
//
bigreal ObjectLoad::loadPrimReal()
{
   if(!load) return 0.0;

   char sign = load.get();

   bigreal data = LoadRealInt(load);
   data += LoadRealFrac(load);

   return sign == '-' ? -data : data;
}

//
// ObjectLoad::loadPrimSInt
//
bigsint ObjectLoad::loadPrimSInt()
{
   if(!load) return 0;

   if(load.get() == '-')
      return -static_cast<bigsint>(LoadInt(load));
   else
      return static_cast<bigsint>(LoadInt(load));
}

//
// ObjectLoad::loadPrimUInt
//
biguint ObjectLoad::loadPrimUInt()
{
   if(!load) return 0;

   return LoadInt(load);
}

//
// ObjectSave::savePrimBool
//
void ObjectSave::savePrimBool(bool data)
{
   if(!save) return;

   save.put('0' + data);
   save.put('\0');
}

//
// ObjectSave::savePrimChar
//
void ObjectSave::savePrimChar(char data)
{
   if(!save) return;

   save.put(data);
}

//
// ObjectSave::savePrimReal
//
void ObjectSave::savePrimReal(bigreal data)
{
   if(!save) return;

   if(data < 0) {data = -data; save.put('-');} else save.put('+');

   SaveRealInt(save, std::floor(data));
   save.put('.');
   SaveRealFrac(save, std::fmod(data, 1) * 16);

   save.put('\0');
}

//
// ObjectSave::savePrimSInt
//
void ObjectSave::savePrimSInt(bigsint data)
{
   if(!save) return;

   if(data < 0) {data = -data; save.put('-');} else save.put('+');

   SaveInt(save, data);

   save.put('\0');
}

//
// ObjectSave::savePrimUInt
//
void ObjectSave::savePrimUInt(biguint data)
{
   if(!save) return;

   SaveInt(save, data);

   save.put('\0');
}

// EOF

