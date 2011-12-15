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
// Option: -s, --special
//
//-----------------------------------------------------------------------------

#include "Except.hpp"
#include "io.hpp"
#include "Lump.hpp"
#include "option.hpp"

#include <cstring>



static void iter_dir_maps(char const *filename);
static void iter_dir_spec(char const *filename);
static void iter_dir_x(char const *filename);
static void iter_dir_xx(char const *filename);

static LumpName current_name;



//
// option: -s, --special
//
static int handle_special(char const *opt, int optf, int argc,
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

   Lump::create_special(name, dirname);

   return 1;
}
static option::option_call
option_special('s', "special", "input", "Adds a lump=specialdir pair.", NULL,
handle_special);

//
// Lump::create_special
//
void Lump::create_special(LumpName name, char const *dirname)
{
   if (name == LN_MAPS)
   {
      iter_dir(dirname, iter_dir_maps);
      return;
   }

   if ((name & LNM_X_START) == LN_X_START)
   {
      current_name = name & LNM_X;

      create_null(name);
      iter_dir(dirname, iter_dir_x);
      create_null(current_name | LN_X_END);
      return;
   }

   if ((name & LNM_XX_START) == LN_XX_START)
   {
      current_name = name & LNM_XX;

      create_null(name);
      iter_dir(dirname, iter_dir_xx);
      create_null(current_name | LN_XX_END);
      return;
   }

   iter_dir(dirname, iter_dir_spec);
}

//
// iter_dir_maps
//
// Adds every directory as a map.
//
static void iter_dir_maps(char const *filename)
{
   Lump::create_map(Lump::name_from_file(filename), filename);
}

//
// iter_dir_spec
//
// Adds directories as specials and files as files.
//
static void iter_dir_spec(char const *filename)
{
   if (is_dir(filename))
      Lump::create_special(Lump::name_from_file(filename), filename);
   else
      Lump::create_file(Lump::name_from_file(filename), filename);
}

//
// iter_dir_x
//
// Adds every file recursively, unless the tags for current_name.
//
static void iter_dir_x(char const *filename)
{
   if (is_dir(filename))
   {
      iter_dir(filename, iter_dir_x);
      return;
   }

   LumpName name = Lump::name_from_file(filename);

   if (((name & LNM_X_START) == LN_X_START ||
        (name & LNM_X_END)   == LN_X_END)  &&
        (name & LNM_X)       == current_name)
   {
      return;
   }

   Lump::create_file(name, filename);
}

//
// iter_dir_xx
//
// Adds every file recursively, unless the tags for current_name.
//
static void iter_dir_xx(char const *filename)
{
   if (is_dir(filename))
   {
      iter_dir(filename, iter_dir_xx);
      return;
   }

   LumpName name = Lump::name_from_file(filename);

   if (((name & LNM_XX_START) == LN_XX_START ||
        (name & LNM_XX_END)   == LN_XX_END)  &&
        (name & LNM_XX)       == current_name)
   {
      return;
   }

   Lump::create_file(name, filename);
}



// EOF

