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
// Option: -d, --directory
//
//-----------------------------------------------------------------------------

#include "Lump.hpp"

#include "io.hpp"
#include "option.hpp"
#include "Wad.hpp"

#include <cstring>


//----------------------------------------------------------------------------|
// Options                                                                    |
//

//
// option: -d, --directory
//
static int HandleDirectory(char const *opt, int optf, int argc, char const *const *argv)
{
   if(!argc) option::exception::error(opt, optf, "requires argument");

   Lump::CreateDirectory(&Wad::RootWad, argv[0]);

   return 1;
}
static option::option_call OptionDirectory('d', "directory", "input",
   "Adds a directory of loose files.", NULL, HandleDirectory);

//
// option: -r, --recurse
//
static option::option_data<bool> OptionRecurse('r', "recurse", "output",
   "Enables recursive WAD generation.", NULL, false);


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// IterateDirDir
//
static void IterateDirDir(char const *filename, void *data)
{
   Wad *wad = static_cast<Wad *>(data);

   if(is_dir(filename))
   {
      if(OptionRecurse.data)
      {
         Wad *subwad = new Wad;

         Lump::CreateDirectory(subwad, filename);

         Lump::CreateWad(wad, Lump::name_from_file(filename), subwad);
      }
      else
         Lump::CreateDirectory(wad, filename);
   }
   else
      Lump::CreateFile(wad, Lump::name_from_file(filename), filename);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Lump::CreateDirectory
//
void Lump::CreateDirectory(Wad *wad, char const *dirname)
{
   IterateDir(dirname, IterateDirDir, wad);
}

// EOF

