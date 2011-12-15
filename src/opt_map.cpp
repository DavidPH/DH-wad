//-----------------------------------------------------------------------------
//
// Copyright(C) 2011 David Hill
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
// Option: -m, --map
//
//-----------------------------------------------------------------------------

#include "io.hpp"
#include "Lump.hpp"
#include "option.hpp"

#include <cstring>



#define DO_MAP_LUMP(NAME,REQ)                   \
if (map_lump[ML_##NAME])                        \
{                                               \
   create_file(LN_##NAME, map_lump[ML_##NAME]); \
   delete[] map_lump[ML_##NAME];                \
   map_lump[ML_##NAME] = NULL;                  \
}                                               \
else if (REQ)                                   \
   create_null(LN_##NAME)

enum
{
   ML_HEADER,
   ML_TEXTMAP,
   ML_THINGS,
   ML_LINEDEFS,
   ML_SIDEDEFS,
   ML_VERTEXES,
   ML_SEGS,
   ML_SSECTORS,
   ML_NODES,
   ML_SECTORS,
   ML_REJECT,
   ML_BLOCKMAP,
   ML_BEHAVIOR,
   ML_ENDMAP,
   ML_NONE
};

static void iter_dir_map(char const *filename);
static void iter_dir_ml(char const *filename);

static LumpName current_map;
static char *map_lump[ML_NONE] = {NULL};



//
// option: -m, --map
//
static int handle_map(char const *opt, int optf, int argc,
                      char const *const *argv)
{
   char const *dirname;
   LumpName name;

   if (!argc) option::exception::error(opt, optf, "requires argument");

   dirname = strchr(argv[0], '=');

   if (dirname)
      name = Lump::name_from_string(argv[0], dirname++);
   else
   {
      dirname = argv[0];
      name = Lump::name_from_file(dirname);
   }

   if (!*dirname)
      option::exception::error(opt, optf, "requires dirname");

   Lump::create_map(name, dirname);

   return 1;
}
static option::option_call
option_map('m', "map", "input", "Adds a lump=mapdir pair.", NULL, handle_map);

//
// Lump::create_map
//
void Lump::create_map(LumpName name, char const *dirname)
{
   current_map = name;

   iter_dir(dirname, iter_dir_ml);

   bool textmap = map_lump[ML_TEXTMAP];

   if (map_lump[ML_HEADER])
      Lump::create_file(name, map_lump[ML_HEADER]);
   else
      Lump::create_null(name);

   DO_MAP_LUMP(TEXTMAP,   textmap);
   DO_MAP_LUMP(THINGS,   !textmap);
   DO_MAP_LUMP(LINEDEFS, !textmap);
   DO_MAP_LUMP(SIDEDEFS, !textmap);
   DO_MAP_LUMP(VERTEXES, !textmap);
   DO_MAP_LUMP(SEGS,     !textmap);
   DO_MAP_LUMP(SSECTORS, !textmap);
   DO_MAP_LUMP(NODES,    !textmap);
   DO_MAP_LUMP(SECTORS,  !textmap);
   DO_MAP_LUMP(REJECT,   !textmap);
   DO_MAP_LUMP(BLOCKMAP, !textmap);
   DO_MAP_LUMP(BEHAVIOR,  false  );
   if (textmap) iter_dir(dirname, iter_dir_map);
   DO_MAP_LUMP(ENDMAP,    textmap);
}

//
// iter_file_map
//
// Adds any file that doesn't have the same lump name as a known map lump.
// Directories are handled recursively.
//
static void iter_dir_map(char const *filename)
{
   if (is_dir(filename))
   {
      iter_dir(filename, iter_dir_map);
      return;
   }

   LumpName name = Lump::name_from_file(filename);

   if (Lump::name_is_binmap(name) || name == LN_TEXTMAP || name == LN_ENDMAP ||
       name == current_map)
   {
      return;
   }

   Lump::create_file(name, filename);
}

//
// iter_file_ml
//
// Searches for standard map lumps.
//
static void iter_dir_ml(char const *filename)
{
   if (is_dir(filename))
   {
      iter_dir(filename, iter_dir_ml);
      return;
   }

   LumpName name = Lump::name_from_file(filename);

   int index = -1;

   if (name == current_map)
      index = ML_HEADER;
   else switch (name)
   {
   case LN_TEXTMAP:  index = ML_TEXTMAP;  break;
   case LN_THINGS:   index = ML_THINGS;   break;
   case LN_LINEDEFS: index = ML_LINEDEFS; break;
   case LN_SIDEDEFS: index = ML_SIDEDEFS; break;
   case LN_VERTEXES: index = ML_VERTEXES; break;
   case LN_SEGS:     index = ML_SEGS;     break;
   case LN_SSECTORS: index = ML_SSECTORS; break;
   case LN_NODES:    index = ML_NODES;    break;
   case LN_SECTORS:  index = ML_SECTORS;  break;
   case LN_REJECT:   index = ML_REJECT;   break;
   case LN_BLOCKMAP: index = ML_BLOCKMAP; break;
   case LN_BEHAVIOR: index = ML_BEHAVIOR; break;
   case LN_ENDMAP:   index = ML_ENDMAP;   break;
   }

   if (index == -1) return;

   size_t len = strlen(filename)+1;
   map_lump[index] = new char[len];
   memcpy(map_lump[index], filename, len);
}



// EOF

