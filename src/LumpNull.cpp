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
// Lump Handling: --null
//
//-----------------------------------------------------------------------------

#include "Lump.hpp"
#include "option.hpp"



class LumpNull : public Lump
{
public:
   LumpNull(LumpName name);

   virtual size_t size();

   virtual void writeData(FILE *out);
};



//
// option: -n, --null
//
static int handle_null(char const *opt, int optf, int argc,
                       char const *const *argv)
{
   if (!argc) option::exception::error(opt, optf, "requires argument");

   LumpName name = Lump::name_from_string(argv[0]);
   Lump::create_null(name);

   return 1;
}
static option::option_call
option_null('n', "null", "input", "Adds an empty lump.", NULL, handle_null);

//
// Lump::create_null
//
void Lump::create_null(LumpName name)
{
   new LumpNull(name);
}

//
// LumpNull::LumpNull
//
LumpNull::LumpNull(LumpName _name) : Lump(_name)
{
}

//
// LumpNull::size
//
size_t LumpNull::size()
{
   return 0;
}

//
// LumpNull::writeData
//
void LumpNull::writeData(FILE *)
{
}



// EOF

