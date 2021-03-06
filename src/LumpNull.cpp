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
// Lump Handling: --null
//
//-----------------------------------------------------------------------------

#include "Lump.hpp"

#include "option.hpp"
#include "Wad.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// LumpNull
//
class LumpNull : public Lump
{
public:
   LumpNull(LumpName _name) : Lump(_name) {}

   virtual size_t size() {return 0;}

   virtual void writeData(FILE *) {}
};


//----------------------------------------------------------------------------|
// Options                                                                    |
//

//
// option: -n, --null
//
static int HandleNull(char const *opt, int optf, int argc,
                       char const *const *argv)
{
   if(!argc) option::exception::error(opt, optf, "requires argument");

   Lump::CreateNull(&Wad::RootWad, Lump::name_from_string(argv[0]));

   return 1;
}
static option::option_call OptionNull('n', "null", "input",
  "Adds an empty lump.", NULL, HandleNull);


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Lump::CreateNull
//
void Lump::CreateNull(Wad *wad, LumpName name)
{
   wad->insert(new LumpNull(name));
}

// EOF

