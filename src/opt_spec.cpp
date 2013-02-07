//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2013 David Hill
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
// Option: -s, --special
//
//-----------------------------------------------------------------------------

#include "Lump.hpp"

#include "Except.hpp"
#include "io.hpp"
#include "option.hpp"
#include "Wad.hpp"

#include <cstring>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define DO_HEAD_LUMP(HL,LN)                     \
if (head_lump[HL])                              \
{                                               \
   CreateFile(wad, current_name|LN, head_lump[HL]); \
   delete[] head_lump[HL];                      \
   head_lump[HL] = NULL;                        \
}                                               \
else                                            \
   CreateNull(wad, current_name|LN)


//----------------------------------------------------------------------------|
// Types                                                                      |
//

enum
{
   HL_START,
   HL_END,
   HL_NONE
};


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static LumpName current_name;
static char *head_lump[HL_NONE] = {NULL};


//----------------------------------------------------------------------------|
// Options                                                                    |
//

//
// option: -s, --special
//
static int HandleSpecial(char const *opt, int optf, int argc, char const *const *argv)
{
   char const *dirname;
   LumpName name;

   if(!argc) option::exception::error(opt, optf, "requires argument");

   dirname = strchr(argv[0], '=');

   if(dirname)
      name = Lump::name_from_string(argv[0], dirname++);
   else
   {
      dirname = argv[0];
      name = Lump::name_from_file(dirname);
   }

   if(!*dirname)
      option::exception::error(opt, optf, "requires dirname");

   Lump::CreateSpecial(&Wad::RootWad, name, dirname);

   return 1;
}
static option::option_call OptionSpecial('s', "special", "input",
   "Adds a lump=specialdir pair.", NULL, HandleSpecial);


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// IterateDirMaps
//
// Adds every directory as a map.
//
static void IterateDirMaps(char const *filename, void *data)
{
   Wad *wad = static_cast<Wad *>(data);

   Lump::CreateMap(wad, Lump::name_from_file(filename), filename);
}

//
// IterateDirSpec
//
// Adds directories as specials and files as files.
//
static void IterateDirSpec(char const *filename, void *data)
{
   Wad *wad = static_cast<Wad *>(data);

   if(is_dir(filename))
      Lump::CreateSpecial(wad, Lump::name_from_file(filename), filename);
   else
      Lump::CreateFile(wad, Lump::name_from_file(filename), filename);
}

//
// IterateDirX
//
// Adds every file recursively, unless the tags for current_name.
//
static void IterateDirX(char const *filename, void *data)
{
   Wad *wad = static_cast<Wad *>(data);

   if(is_dir(filename))
   {
      IterateDir(filename, IterateDirX, wad);
      return;
   }

   LumpName name = Lump::name_from_file(filename);

   if(((name & LNM_X_START) == LN_X_START ||
       (name & LNM_X_END)   == LN_X_END)  &&
       (name & LNM_X)       == current_name)
   {
      return;
   }

   Lump::CreateFile(wad, name, filename);
}

//
// IterateDirXHead
//
// Searches recursively for the tags for current_name.
//
static void IterateDirXHead(char const *filename, void *data)
{
   Wad *wad = static_cast<Wad *>(data);

   if(is_dir(filename))
   {
      IterateDir(filename, IterateDirXHead, wad);
      return;
   }

   LumpName name = Lump::name_from_file(filename);

   int index = -1;

   if((name & LNM_X) == current_name)
   {
      if((name & LNM_X_START) == LN_X_START)
         index = HL_START;
      else if((name & LNM_X_END) == LN_X_END)
         index = HL_END;
   }

   if(index == -1) return;

   std::size_t len = std::strlen(filename) + 1;
   head_lump[index] = new char[len];
   std::memcpy(head_lump[index], filename, len);
}

//
// IterateDirXX
//
// Adds every file recursively, unless the tags for current_name.
//
static void IterateDirXX(char const *filename, void *data)
{
   Wad *wad = static_cast<Wad *>(data);

   if(is_dir(filename))
   {
      IterateDir(filename, IterateDirXX, wad);
      return;
   }

   LumpName name = Lump::name_from_file(filename);

   if(((name & LNM_XX_START) == LN_XX_START ||
       (name & LNM_XX_END)   == LN_XX_END)  &&
       (name & LNM_XX)       == current_name)
   {
      return;
   }

   Lump::CreateFile(wad, name, filename);
}

//
// IterateDirXXHead
//
// Searches recursively for the tags for current_name.
//
static void IterateDirXXHead(char const *filename, void *data)
{
   Wad *wad = static_cast<Wad *>(data);

   if(is_dir(filename))
   {
      IterateDir(filename, IterateDirXXHead, wad);
      return;
   }

   LumpName name = Lump::name_from_file(filename);

   int index = -1;

   if((name & LNM_XX) == current_name)
   {
      if((name & LNM_XX_START) == LN_XX_START)
         index = HL_START;
      else if((name & LNM_XX_END) == LN_XX_END)
         index = HL_END;
   }

   if(index == -1) return;

   std::size_t len = std::strlen(filename) + 1;
   head_lump[index] = new char[len];
   std::memcpy(head_lump[index], filename, len);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Lump::CreateSpecial
//
void Lump::CreateSpecial(Wad *wad, LumpName name, char const *dirname)
{
   if(name == LN_MAPS)
   {
      IterateDir(dirname, IterateDirMaps, wad);
      return;
   }

   if((name & LNM_X_START) == LN_X_START)
   {
      current_name = name & LNM_X;

      IterateDir(dirname, IterateDirXHead, wad);
      DO_HEAD_LUMP(HL_START, LN_X_START);
      IterateDir(dirname, IterateDirX, wad);
      DO_HEAD_LUMP(HL_END,   LN_X_END);

      return;
   }

   if((name & LNM_XX_START) == LN_XX_START)
   {
      current_name = name & LNM_XX;

      IterateDir(dirname, IterateDirXXHead, wad);
      DO_HEAD_LUMP(HL_START, LN_XX_START);
      IterateDir(dirname, IterateDirXX, wad);
      DO_HEAD_LUMP(HL_END,   LN_XX_END);

      return;
   }

   IterateDir(dirname, IterateDirSpec, wad);
}

// EOF

