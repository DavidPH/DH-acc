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
// Deferred memory/index allocation.
//
//-----------------------------------------------------------------------------

#include "../ObjectExpression.hpp"

#include "iter.hpp"

#include "../option.hpp"
#include "../ost_type.hpp"
#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

struct Used
{
   //
   // ::Used
   //
   Used(bigsint _begin, bigsint _size)
    : begin(_begin), size(_size), end(begin+size)
   {
   }

   //
   // ::contains
   //
   bool contains(Used const &used) const
   {
      if (used.begin >= begin && used.begin < end) return true;
      if (used.end >= begin && used.end < end) return true;
      return false;
   }

   //
   // ::operator ++
   //
   Used &operator ++ () {++begin; ++end; return *this;}

   //
   // ::operator <
   //
   bool operator < (Used const &used) const {return begin < used.begin;}

   bigsint begin;
   bigsint size;
   bigsint end;
};

typedef std::set<Used> UsedMap;
typedef UsedMap::iterator UsedIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_dptr<int> option_addr_array_handler
('\0', "addr-array", "features",
 "Selects which global array to use for addressable variables. 0 by default.",
 NULL, &option_addr_array);

static option::option_dptr<int> option_addr_stack_handler
('\0', "addr-stack", "features",
 "Selects which world register to use for the addressable stack. 0 by default.",
 NULL, &option_addr_stack);

extern int option_static_offset;
static option::option_dptr<int> option_static_offset_handler
('\0', "static-offset", "features",
 "Sets the allocation offset for static variables. 8192 by default.",
 NULL, &option_static_offset);

static option::option_dptr<int> option_static_temp_handler
('\0', "static-temp", "features",
 "Selects which world register to use for temporary storage. 1 by default.",
 NULL, &option_static_offset);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

int option_addr_array = 0;
int option_addr_stack = 0;
int option_static_offset = 8192;
int option_static_temp = 1;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// is_used
//
// Returns true is a section of memory is unavailable for allocation.
//
static bool is_used(UsedMap *usedMap, Used const &used)
{
   for (UsedIter iter = usedMap->begin(); iter != usedMap->end(); ++iter)
      if (iter->contains(used)) return true;

   return false;
}

//
// set_used
//
// Marks a section of memory as unavailable for allocation.
//
static void set_used(UsedMap *usedMap, Used const &used)
{
   usedMap->insert(used);
}

//
// get_used
//
// Allocates a section of memory.
//
static Used get_used(UsedMap *usedMap, bigsint size)
{
   Used used(0, size);

   while (is_used(usedMap, used) && used.end >= 0) ++used;

   if (used.end < 0)
      throw SourceException("no more used", SourcePosition::none(), __func__);

   set_used(usedMap, used);

   return used;
}

ObjectExpression::Pointer create_int(bigsint i)
{
   return ObjectExpression::create_value_int(i, SourcePosition::none());
}

//
// allocate_pre
//
template<typename T> static void allocate_pre(UsedMap *usedMap, T &data)
{
   if (data.number != -1)
      set_used(usedMap, Used(data.number, 1));
}

//
// allocate_pre_size
//
template<typename T> static void allocate_pre_size(UsedMap *usedMap, T &data)
{
   if (data.number != -1)
      set_used(usedMap, Used(data.number, data.size));
}

//
// allocate
//
template<typename T> static void allocate(UsedMap *usedMap, T &data)
{
   if (data.number == -1)
      data.number = get_used(usedMap, 1).begin;

   ObjectExpression::add_symbol(data.name, create_int(data.number));
}

//
// allocate_size
//
template<typename T> static void allocate_size(UsedMap *usedMap, T &data)
{
   if (data.number == -1)
      data.number = get_used(usedMap, data.size).begin;

   ObjectExpression::add_symbol(data.name, create_int(data.number));
}

//
// allocate_Function
//
static void allocate_Function(bigsint *number, ObjectData_Function &f)
{
   f.number = (*number)++;

   ObjectExpression::add_symbol(f.name, create_int(f.number));
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::do_deferred_allocation
//
void ObjectExpression::do_deferred_allocation()
{
   UsedMap usedMap;


   // mapregisters and maparrays
   if (output_type == OUTPUT_ACSE)
   {
      // ACSE's MEXP uses the same addresses for both.
      // TODO: !externVis registers/arrays should not reserve the
      // corresponding arrays/registers.
      usedMap.clear();

      _iterator_map(_register_map_table, allocate_pre_size<ObjectData_Register>, &usedMap);
      _iterator_map(_registerarray_map_table, allocate_pre<ObjectData_RegisterArray>, &usedMap);
      _iterator_map(_register_map_table, allocate_size<ObjectData_Register>, &usedMap);
      _iterator_map(_registerarray_map_table, allocate<ObjectData_RegisterArray>, &usedMap);
   }
   else
   {
      // mapregisters
      usedMap.clear();

      _iterator_map(_register_map_table, allocate_pre_size<ObjectData_Register>, &usedMap);
      _iterator_map(_register_map_table, allocate_size<ObjectData_Register>, &usedMap);

      // maparrays
      usedMap.clear();

      _iterator_map(_registerarray_map_table, allocate_pre<ObjectData_RegisterArray>, &usedMap);
      _iterator_map(_registerarray_map_table, allocate<ObjectData_RegisterArray>, &usedMap);
   }


   // worldregisters
   usedMap.clear();
   set_used(&usedMap, Used(option_addr_stack, 1));
   set_used(&usedMap, Used(option_static_temp, 1));

   _iterator_map(_register_world_table, allocate_pre_size<ObjectData_Register>, &usedMap);
   _iterator_map(_register_world_table, allocate_size<ObjectData_Register>, &usedMap);


   // globalregisters
   usedMap.clear();

   _iterator_map(_register_global_table, allocate_pre_size<ObjectData_Register>, &usedMap);
   _iterator_map(_register_global_table, allocate_size<ObjectData_Register>, &usedMap);


   // worldarrays
   usedMap.clear();

   _iterator_map(_registerarray_world_table, allocate_pre<ObjectData_RegisterArray>, &usedMap);
   _iterator_map(_registerarray_world_table, allocate<ObjectData_RegisterArray>, &usedMap);


   // globalarrays
   usedMap.clear();
   set_used(&usedMap, Used(option_addr_array, 1));

   _iterator_map(_registerarray_global_table, allocate_pre<ObjectData_RegisterArray>, &usedMap);
   _iterator_map(_registerarray_global_table, allocate<ObjectData_RegisterArray>, &usedMap);


   // For ACS+, all the following allocation is done by the linker.
   if (output_type == OUTPUT_ACSP) return;


   // functions
   if (target_type == TARGET_ZDoom)
   {
      bigsint number(0);

      _iterator_map(_function_table, allocate_Function, &number);
   }


   // scripts
   ObjectData_Script::generate_symbols();


   // statics
   usedMap.clear();
     set_used(&usedMap, Used(0, option_static_offset));

   _iterator_map(_static_table, allocate_pre<ObjectData_Static>, &usedMap);
   _iterator_map(_static_table, allocate<ObjectData_Static>, &usedMap);


   // strings
   ObjectData_String::generate_symbols();
}

// EOF

