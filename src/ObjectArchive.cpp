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
// ReadInt
//
static biguint ReadInt(std::istream *in)
{
   biguint i = 0;

   for(int c; (c = in->get()) && *in;)
      i = i * 16 + SourceExpression::ParseNumber(c);

   return i;
}

//
// ReadRealFrac
//
static bigreal ReadRealFrac(std::istream *in)
{
   int c = in->get();

   if(c == '\0') return 0;

   return (SourceExpression::ParseNumber(c) + ReadRealFrac(in)) / 16;
}

//
// ReadRealInt
//
static bigreal ReadRealInt(std::istream *in)
{
   bigreal i = 0;

   for(int c; (c = in->get()) != '.' && *in;)
      i = i * 16 + SourceExpression::ParseNumber(c);

   return i;
}

//
// WriteInt
//
static void WriteInt(std::ostream *out, biguint i)
{
   if(!i) return;

   WriteInt(out, i / 16);
   out->put("0123456789ABCDEF"[i % 16]);
}

//
// WriteRealFrac
//
static void WriteRealFrac(std::ostream *out, bigreal f)
{
   for(; f; f = std::fmod(f, 1) * 16)
      out->put("0123456789ABCDEF"[static_cast<unsigned>(std::floor(f))]);
}

//
// WriteRealInt
//
static void WriteRealInt(std::ostream *out, bigreal i)
{
   if(!i) return;

   WriteRealInt(out, std::floor(i / 16));
   out->put("0123456789ABCDEF"[static_cast<unsigned>(std::floor(std::fmod(i, 16)))]);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectArchive::readBool
//
bool ObjectArchive::readBool()
{
   if(!in || !*in) return false;

   int c = in->get();

   while(*in && in->get()) {}

   return c != '0';
}

//
// ObjectArchive::readChar
//
char ObjectArchive::readChar()
{
   if(!in || !*in) return '\0';

   return static_cast<char>(in->get());
}

//
// ObjectArchive::readReal
//
bigreal ObjectArchive::readReal()
{
   if(!in || !*in) return 0;

   char sign = in->get();

   bigreal data = ReadRealInt(in);
   data += ReadRealFrac(in);

   return sign == '-' ? -data : data;
}

//
// ObjectArchive::readSInt
//
bigsint ObjectArchive::readSInt()
{
   if(!in || !*in) return 0;

   if(in->get() == '-')
      return -static_cast<bigsint>(ReadInt(in));
   else
      return static_cast<bigsint>(ReadInt(in));
}

//
// ObjectArchive::readUInt
//
biguint ObjectArchive::readUInt()
{
   if(!in || !*in) return 0;

   return ReadInt(in);
}

//
// ObjectArchive::writeBool
//
void ObjectArchive::writeBool(bool data)
{
   if(!out || !*out) return;

   out->put(data ? '1' : '0');
   out->put('\0');
}

//
// ObjectArchive::writeChar
//
void ObjectArchive::writeChar(char data)
{
   if(!out || !*out) return;

   out->put(data);
}

//
// ObjectArchive::writeReal
//
void ObjectArchive::writeReal(bigreal data)
{
   if(!out || !*out) return;

   if(data < 0) {data = -data; out->put('-');} else out->put('+');

   WriteRealInt(out, std::floor(data));
   out->put('.');
   WriteRealFrac(out, std::fmod(data, 1) * 16);

   out->put('\0');
}

//
// ObjectArchive::writeSInt
//
void ObjectArchive::writeSInt(bigsint data)
{
   if(!out || !*out) return;

   if(data < 0) {data = -data; out->put('-');} else out->put('+');

   WriteInt(out, data);

   out->put('\0');
}

//
// ObjectArchive::writeUInt
//
void ObjectArchive::writeUInt(biguint data)
{
   if(!out || !*out) return;

   WriteInt(out, data);

   out->put('\0');
}

// EOF

