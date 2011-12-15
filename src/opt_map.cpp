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



#define DO_MAP_LUMP(LN,REQ)         \
Lump::write_name_file(fileend, LN); \
if (is_file(filetmp))               \
   Lump::create_file(LN, filetmp);  \
else if (REQ)                       \
   Lump::create_null(LN)

static void iter_dir_map(char const *filename);

static LumpName current_map;



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
   size_t dirlen;
   char *fileend;
   char *filetmp;
   bool textmap;

   current_map = name;

   dirlen = strlen(dirname);

   filetmp = new char[dirlen + 14];
   fileend = filetmp+dirlen;
   memcpy(filetmp, dirname, dirlen);

   if (fileend[-1] != PATH_SEP)
      *fileend++ = PATH_SEP;

   DO_MAP_LUMP(name, true);

   Lump::write_name_file(fileend, LN_TEXTMAP);
   if ((textmap = is_file(filetmp)) == true)
      Lump::create_file(LN_TEXTMAP, filetmp);

   DO_MAP_LUMP(LN_THINGS,   !textmap);
   DO_MAP_LUMP(LN_LINEDEFS, !textmap);
   DO_MAP_LUMP(LN_SIDEDEFS, !textmap);
   DO_MAP_LUMP(LN_VERTEXES, !textmap);
   DO_MAP_LUMP(LN_SEGS,     !textmap);
   DO_MAP_LUMP(LN_SSECTORS, !textmap);
   DO_MAP_LUMP(LN_NODES,    !textmap);
   DO_MAP_LUMP(LN_SECTORS,  !textmap);
   DO_MAP_LUMP(LN_REJECT,   !textmap);
   DO_MAP_LUMP(LN_BLOCKMAP, !textmap);
   DO_MAP_LUMP(LN_BEHAVIOR,  false  );
   if (textmap) iter_dir(dirname, iter_dir_map);
   DO_MAP_LUMP(LN_ENDMAP,    textmap);
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



// EOF

