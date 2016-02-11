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
// Lump Handling
//
//-----------------------------------------------------------------------------

#ifndef HPP_Lump_
#define HPP_Lump_

#include <cstdio>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class Wad;

typedef unsigned long long int LumpName;

constexpr LumpName LN_BEHAVIOR = 0x4245484156494F52ull,
                   LN_BLOCKMAP = 0x424C4F434B4D4150ull,
                   LN_ENDMAP   = 0x454E444D41500000ull,
                   LN_LINEDEFS = 0x4C494E4544454653ull,
                   LN_MAPS     = 0x4D41505300000000ull,
                   LN_NODES    = 0x4E4F444553000000ull,
                   LN_REJECT   = 0x52454A4543540000ull,
                   LN_SECTORS  = 0x534543544F525300ull,
                   LN_SEGS     = 0x5345475300000000ull,
                   LN_SIDEDEFS = 0x5349444544454653ull,
                   LN_SSECTORS = 0x53534543544F5253ull,
                   LN_TEXTMAP  = 0x544558544D415000ull,
                   LN_THINGS   = 0x5448494E47530000ull,
                   LN_VERTEXES = 0x5645525445584553ull,
                   LN_X_END    = 0x005F454E44000000ull,
                   LN_XX_END   = 0x00005F454E440000ull,
                   LN_X_START  = 0x005F535441525400ull,
                   LN_XX_START = 0x00005F5354415254ull,

                   LNM_X        = 0xFF00000000000000ull,
                   LNM_XX       = 0xFFFF000000000000ull,
                   LNM_X_END    = 0x00FFFFFFFFFFFFFFull,
                   LNM_XX_END   = 0x0000FFFFFFFFFFFFull,
                   LNM_X_START  = 0x00FFFFFFFFFFFFFFull,
                   LNM_XX_START = 0x0000FFFFFFFFFFFFull;

//
// Lump
//
class Lump
{
public:
   virtual size_t size() = 0;

   virtual void writeData(FILE *out) = 0;
   void writeHead(FILE *out);
   void writeNameFile(char *out);
   void writeNameString(FILE *out);

   LumpName name;

   Lump *next;

   size_t offset;


   static void CreateData(Wad *wad, LumpName name, char const *data, size_t len);
   static void CreateDataKeep(Wad *wad, LumpName name, char const *data, size_t len);

   static void CreateDirectory(Wad *wad, char const *dirname);

   static void CreateFile(Wad *wad, LumpName name, char const *filename);
   static void CreateFile(Wad *wad, LumpName name, char const *filename, size_t len);
   static void CreateFileKeep(Wad *wad, LumpName name, char const *filename);

   static void CreateMap(Wad *wad, LumpName name, char const *dirname);

   static void CreateNull(Wad *wad, LumpName name);

   static void CreateSpecial(Wad *wad, LumpName name, char const *dirname);

   static void CreateWad(Wad *wad, LumpName name, Wad *subwad);
   static void CreateWad(Wad *wad, char const *filename);

   static LumpName name_from_file(char const *filename);
   static LumpName name_from_string(char const *s);
   static LumpName name_from_string(char const *s, char const *send);

   static bool name_is_binmap(LumpName name);

   static void write_32(FILE *out, unsigned long val);

   static void write_name(char *out, LumpName name);
   static void write_name(FILE *out, LumpName name);
   static void write_name_file(char *out, LumpName name);

   static bool trans_bslash;

protected:
   Lump(LumpName name);

private:
   Lump(Lump const &);
};



#endif//HPP_Lump_

