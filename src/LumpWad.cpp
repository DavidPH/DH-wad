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
// Lump Handling: -r, --recurse
//
//-----------------------------------------------------------------------------

#include "Lump.hpp"

#include "Wad.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// LumpWad
//
class LumpWad : public Lump
{
public:
   LumpWad(LumpName _name, Wad *_wad) : Lump(_name), wad(_wad) {}

   virtual std::size_t size() {return wad->sizeWad();}

   virtual void writeData(std::FILE *out) {wad->writeWad(out);}

   Wad *wad;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Lump::CreateWad
//
void Lump::CreateWad(Wad *wad, LumpName name, Wad *subwad)
{
   wad->insert(new LumpWad(name, subwad));
}

// EOF

