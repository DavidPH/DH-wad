//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 David Hill
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
// Wad Handling
//
//-----------------------------------------------------------------------------

#include "Wad.hpp"

#include "Lump.hpp"
#include "option.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

//
// option: -i, --iwad
//
static option::option_data<bool> option_iwad('i', "iwad", "output",
   "Generates an IWAD instead of a PWAD.", NULL, false);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

Wad Wad::RootWad;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Wad::Wad
//
Wad::Wad() : count(0), size(0), head(NULL), tail(NULL)
{
}

//
// Wad::calculateOffsets
//
void Wad::calculateOffsets()
{
   if(size) return;

   size = (count + 1) * 16;

   for(Lump *lump = head; lump; lump = lump->next)
   {
      lump->offset = size;
      size += lump->size();
   }
}

//
// Wad::insert
//
void Wad::insert(Lump *lump)
{
   tail = (tail ? tail->next : head) = lump;

   ++count;
}

//
// Wad::sizeWad
//
std::size_t Wad::sizeWad()
{
   calculateOffsets();

   return size;
}

//
// Wad::writeList
//
void Wad::writeList(std::FILE *out)
{
   for(Lump *lump = head; lump; lump = lump->next)
   {
      lump->writeNameString(out);
      fputc('\n', out);
   }
}

//
// Wad::writeWad
//
void Wad::writeWad(std::FILE *out)
{
   calculateOffsets();

   std::fputs(option_iwad.data ? "IWAD" : "PWAD", out);
   Lump::write_32(out, count);
   Lump::write_32(out, 16);
   Lump::write_32(out, 0);

   for(Lump *lump = head; lump; lump = lump->next)
      lump->writeHead(out);

   for(Lump *lump = head; lump; lump = lump->next)
      lump->writeData(out);
}

// EOF

