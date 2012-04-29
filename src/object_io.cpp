//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2012 David Hill
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

#include "object_io.hpp"

#include <cmath>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

// I hope to re-enable this some day.
#define USE_BIN_FLTS 0
#define USE_OBJ_TAGS 1


//----------------------------------------------------------------------------|
// Types                                                                      |
//

enum
{
   OBJ_BIT,
   OBJ_FLT,
   OBJ_INT,
   OBJ_STR,
   OBJ_UNS,
};


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

char const *const tag_names[] =
{
   "OBJ_BIT",
   "OBJ_FLT",
   "OBJ_INT",
   "OBJ_STR",
   "OBJ_UNS",
};


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

static void check_object_tag(std::istream *in, int tag)
{
   #if USE_OBJ_TAGS
   static std::string err;

   int intag = in->get();

   if (intag == tag) return;

   err = "expected ";
   err += tag_names[tag];
   err += " got ";

   if (intag < 0)
      err += "EOF";
   else if ((size_t)intag >= (sizeof(tag_names) / sizeof(tag_names[0])))
      err += "OBJ_???";
   else
      err += tag_names[intag];

   throw err.c_str();
   #else
   (void)in; (void)tag;
   #endif
}

#if USE_BIN_FLTS
//
// write_object_flt_part
//
static void write_object_flt_part(unsigned char *&p, bigreal in)
{
   if (!in) return;

   unsigned char &out = *p++;

   write_object_flt_part(p, std::floor(in / 256));

   out = static_cast<unsigned char>(in) & 0xFF;
}
#else
//
// write_object_flt_f
//
static void write_object_flt_f(std::string &out, bigreal in)
{
   while ((in = std::fmod(in, 1) * 10))
      out += static_cast<int>(in) + '0';
}

//
// write_object_flt_i
//
static void write_object_flt_i(std::string &out, bigreal in)
{
   if (!in) return;

   int i = static_cast<int>(std::fmod(in, 10));

   write_object_flt_i(out, std::floor(in / 10));
   out += i + '0';
}
#endif

//
// write_object_tag
//
static void write_object_tag(std::ostream *out, int tag)
{
   #if USE_OBJ_TAGS
   out->put(static_cast<char>(tag));
   #else
   (void)out; (void)tag;
   #endif
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// read_object<bool>
//
void read_object(std::istream *in, bool *out)
{
   *out = read_object_bit(in);
}

//
// read_object<long double>
//
void read_object(std::istream *in, long double *out)
{
   *out = read_object_flt(in);
}

//
// read_object<signed int>
//
void read_object(std::istream *in, signed int *out)
{
   *out = read_object_int(in);
}

//
// read_object<signed long int>
//
void read_object(std::istream *in, long *out)
{
   *out = read_object_int(in);
}

//
// read_object<signed long long int>
//
void read_object(std::istream *in, signed long long int *out)
{
   *out = read_object_int(in);
}

//
// read_object<unsigned int>
//
void read_object(std::istream *in, unsigned int *out)
{
   *out = read_object_uns(in);
}

//
// read_object<unsigned long int>
//
void read_object(std::istream *in, unsigned long int *out)
{
   *out = read_object_uns(in);
}

//
// read_object<unsigned long long int>
//
void read_object(std::istream *in, unsigned long long int *out)
{
   *out = read_object_uns(in);
}

//
// read_object<std::string>
//
void read_object(std::istream *in, std::string *out)
{
   check_object_tag(in, OBJ_STR);

   std::string::iterator iter, end;

   out->resize(read_object_uns(in));
   end = out->end();

   for (iter = out->begin(); iter != end; ++iter)
      *iter = static_cast<char>(in->get());
}

//
// read_object_bit
//
bool read_object_bit(std::istream *in)
{
   check_object_tag(in, OBJ_BIT);

   return static_cast<bool>(in->get());
}

//
// read_object_flt
//
bigreal read_object_flt(std::istream *in)
{
   check_object_tag(in, OBJ_FLT);

   #if USE_BIN_FLTS
   unsigned char len, neg;
   bigreal out;

   if ((neg = (len = in->get() & 0xFF) & 0x80)) len &= 0x7F;
   out = in->get() & 0xFF;
   while (len--) out = (out * 256) + (in->get() & 0xFF);

   if ((len = in->get() & 0xFF) & 0x80)
   {
      len &= 0x7F;
      while (--len) out /= 256;
   }
   else if (len)
      while (--len) out *= 256;

   return neg ? -out : out;
   #else
   std::string::iterator itr, end;
   std::string buf;
   bigreal out = 0, outf = 0;
   bool neg;

   read_object(in, &buf);
   end = buf.end();
   itr = buf.begin();

   if (*itr == '-') {neg = true; ++itr;} else neg = false;

   while (itr != end && *itr != '.')
   {
      out *= 10;
      out += *itr++ - '0';
   }

   if (itr != end) while (itr != --end)
   {
      outf += *end - '0';
      outf /= 10;
   }

   out += outf;

   return neg ? -out : out;
   #endif
}

//
// read_object_int
//
bigsint read_object_int(std::istream *in)
{
   check_object_tag(in, OBJ_INT);

   unsigned char len, neg;
   bigsint out;

   if ((neg = (len = in->get() & 0xFF) & 0x80)) len &= 0x7F;
   out = in->get() & 0xFF;
   while (len--) out = (out << 8) | (in->get() & 0xFF);

   return neg ? -out : out;
}

//
// read_object_uns
//
biguint read_object_uns(std::istream *in)
{
   check_object_tag(in, OBJ_UNS);

   unsigned char len;
   biguint out;

   len = in->get() & 0x7F; // Discard sign bit.
   out = in->get() & 0xFF;
   while (len--) out = (out << 8) | (in->get() & 0xFF);

   return out;
}

//
// write_object<bool>
//
void write_object(std::ostream *out, bool const *in)
{
   write_object_bit(out, *in);
}

//
// write_object<long double>
//
void write_object(std::ostream *out, long double const *in)
{
   write_object_flt(out, *in);
}

//
// write_object<signed int>
//
void write_object(std::ostream *out, signed int const *in)
{
   write_object_int(out, *in);
}

//
// write_object<signed long int>
//
void write_object(std::ostream *out, signed long int const *in)
{
   write_object_int(out, *in);
}

//
// write_object<signed long long int>
//
void write_object(std::ostream *out, signed long long int const *in)
{
   write_object_int(out, *in);
}

//
// write_object<unsigned int>
//
void write_object(std::ostream *out, unsigned int const *in)
{
   write_object_uns(out, *in);
}

//
// write_object<unsigned long int>
//
void write_object(std::ostream *out, unsigned long int const *in)
{
   write_object_uns(out, *in);
}

//
// write_object<unsigned long long int>
//
void write_object(std::ostream *out, unsigned long long int const *in)
{
   write_object_uns(out, *in);
}

//
// write_object<std::string>
//
void write_object(std::ostream *out, std::string const *in)
{
   write_object_tag(out, OBJ_STR);

   std::string::const_iterator iter, end = in->end();

   write_object_uns(out, in->size());

   for (iter = in->begin(); iter != end; ++iter)
      out->put(*iter);
}

//
// write_object_bit
//
void write_object_bit(std::ostream *out, bool in)
{
   write_object_tag(out, OBJ_BIT);

   out->put(static_cast<char>(in));
}

//
// write_object_flt
//
void write_object_flt(std::ostream *out, bigreal in)
{
   write_object_tag(out, OBJ_FLT);

   #if USE_BIN_FLTS
   // Handle 0.      len          buf          exp
   if (!in) {out->put(0); out->put(0); out->put(0); return;}

   unsigned char buf[128];
   unsigned char *p = buf, neg, exp = 0;

   if (in < 0) {in = -in; neg = 0x80;} else neg = 0;

   write_object_flt_part(p, in);

   for (; (in = std::fmod(in, 1) * 256); ++exp)
      *p++ = static_cast<unsigned char>(in) & 0xFF;

   out->put((p-buf-1) | neg);
   while (p != buf) out->put(*--p);
   out->put(exp ? (exp | 0x80) : 0);
   #else
   std::string buf;

   if (in < 0) {in = -in; buf = "-";}

   write_object_flt_i(buf, in);
   buf += '.';
   write_object_flt_f(buf, in);

   write_object(out, &buf);
   #endif
}

//
// write_object_int
//
void write_object_int(std::ostream *out, bigsint in)
{
   write_object_tag(out, OBJ_INT);

   unsigned char buf[128];
   unsigned char *p = buf, neg;

   if (in < 0) {in = -in; neg = 0x80;} else neg = 0;

   do *p++ = in & 0xFF; while (in >>= 8);

   out->put((p-buf-1) | neg);
   while (p != buf) out->put(*--p);
}

//
// write_object_uns
//
void write_object_uns(std::ostream *out, biguint in)
{
   write_object_tag(out, OBJ_UNS);

   unsigned char buf[128];
   unsigned char *p = buf;

   do *p++ = in & 0xFF; while (in >>= 8);

   out->put(p-buf-1);
   while (p != buf) out->put(*--p);
}

// EOF

