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
// Option: -d, --directory
//
//-----------------------------------------------------------------------------

#include "io.hpp"
#include "Lump.hpp"
#include "option.hpp"

#include <cstring>



static void iter_dir_dir(char const *filename);



//
// option: -d, --directory
//
static int handle_directory(char const *opt, int optf, int argc,
                      char const *const *argv)
{
   if (!argc) option::exception::error(opt, optf, "requires argument");

   Lump::create_directory(argv[0]);

   return 1;
}
static option::option_call
option_directory('d', "directory", "input", "Adds a directory of loose files.",
NULL, handle_directory);

//
// iter_dir_dir
//
static void iter_dir_dir(char const *filename)
{
   if (is_dir(filename))
      Lump::create_directory(filename);
   else
      Lump::create_file(Lump::name_from_file(filename), filename);
}

//
// Lump::create_directory
//
void Lump::create_directory(char const *dirname)
{
   iter_dir(dirname, iter_dir_dir);
}



// EOF

