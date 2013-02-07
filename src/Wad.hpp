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

#ifndef HPP_Wad_
#define HPP_Wad_

#include <cstdio>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class Lump;

//
// Wad
//
class Wad
{
public:
   Wad();

   void insert(Lump *lump);

   std::size_t sizeWad();

   void writeList(std::FILE *out);
   void writeWad(std::FILE *out);

   std::size_t count, size;

   Lump *head, *tail;


   static Wad RootWad;

private:
   void calculateOffsets();
};

#endif//HPP_Wad_

