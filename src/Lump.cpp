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

#include "io.hpp"
#include "Lump.hpp"
#include "option.hpp"

#include <cstring>


bool Lump::trans_bslash = false;

//
// option: -b, --translate-backslash
//
static option::option_dptr<bool>
option_trans_bslash('b', "translate-backslash", "translation",
"Translates '\\' to '^' when writing file names and '^' to '\\' when writing"
" lump names.", NULL, &Lump::trans_bslash);

//
// Lump::Lump
//
Lump::Lump(LumpName _name) : name(_name), next(NULL), offset(0)
{
}

//
// Lump::name_from_file
//
LumpName Lump::name_from_file(char const *filename)
{
   char const *fileend;
   size_t      filelen = strlen(filename);
   char const *filepart;
   bool        foundExt = false;
   int         i;
   LumpName    name;

   if (filelen == 0) return 0;

   fileend = filename + filelen;

   // If file ends in a pathsep, discard it.
   if (fileend[-1] == PATH_SEP)
   {
      fileend--;

      // And if that reduces length to 0, return early.
      if (filelen == 1) return 0;
   }

   // Search the string looking for...
   for (filepart = fileend-1; filepart != filename; --filepart)
   {
      // ... PATH_SEP, in which case we're done searching.
      if (*filepart == PATH_SEP)
      {
         filepart++;
         break;
      }

      // ... file extension, but only once.
      if (!foundExt && *filepart == '.')
      {
         foundExt = true;
         fileend = filepart;
      }
   }

   // Convert the filename to a lumpname, performing any required translation.
   for (i = 64, name = 0; i && filepart != fileend; ++filepart)
   {
      i -= 8;

      if (trans_bslash && *filepart == '^')
         name |= static_cast<LumpName>('\\') << i;
      else
         name |= static_cast<LumpName>(*filepart) << i;
   }

   return name;
}

//
// Lump::name_from_string
//
LumpName Lump::name_from_string(char const *s)
{
   LumpName name = 0;

   for (int i = 64; i && *s; ++s)
   {
      i -= 8;
      name |= static_cast<LumpName>(*s) << i;
   }

   return name;
}

//
// Lump::name_from_string
//
LumpName Lump::name_from_string(char const *s, char const *send)
{
   LumpName name = 0;

   for (int i = 64; i && s != send; ++s)
   {
      i -= 8;
      name |= static_cast<LumpName>(*s) << i;
   }

   return name;
}

//
// Lump::name_is_binmap
//
bool Lump::name_is_binmap(LumpName name)
{
   return name == LN_THINGS   ||
          name == LN_LINEDEFS ||
          name == LN_SIDEDEFS ||
          name == LN_VERTEXES ||
          name == LN_SEGS     ||
          name == LN_SSECTORS ||
          name == LN_NODES    ||
          name == LN_SECTORS  ||
          name == LN_REJECT   ||
          name == LN_BLOCKMAP ||
          name == LN_BEHAVIOR;
}

//
// Lump::write_32
//
void Lump::write_32(FILE *out, unsigned long val)
{
   putc((val >>  0) & 0xFF, out);
   putc((val >>  8) & 0xFF, out);
   putc((val >> 16) & 0xFF, out);
   putc((val >> 24) & 0xFF, out);
}

//
// Lump::write_name
//
void Lump::write_name(char *out, LumpName name)
{
   out[0] = (name >> 56) & 0xFF;
   out[1] = (name >> 48) & 0xFF;
   out[2] = (name >> 40) & 0xFF;
   out[3] = (name >> 32) & 0xFF;
   out[4] = (name >> 24) & 0xFF;
   out[5] = (name >> 16) & 0xFF;
   out[6] = (name >>  8) & 0xFF;
   out[7] = (name >>  0) & 0xFF;
}

//
// Lump::write_name
//
void Lump::write_name(FILE *out, LumpName name)
{
   putc((name >> 56) & 0xFF, out);
   putc((name >> 48) & 0xFF, out);
   putc((name >> 40) & 0xFF, out);
   putc((name >> 32) & 0xFF, out);
   putc((name >> 24) & 0xFF, out);
   putc((name >> 16) & 0xFF, out);
   putc((name >>  8) & 0xFF, out);
   putc((name >>  0) & 0xFF, out);
}

//
// Lump::write_name_file
//
void Lump::write_name_file(char *out, LumpName name)
{
   out[0] = (name >> 56) & 0xFF;
   out[1] = (name >> 48) & 0xFF;
   out[2] = (name >> 40) & 0xFF;
   out[3] = (name >> 32) & 0xFF;
   out[4] = (name >> 24) & 0xFF;
   out[5] = (name >> 16) & 0xFF;
   out[6] = (name >>  8) & 0xFF;
   out[7] = (name >>  0) & 0xFF;
   out[8] = '\0';

   if (trans_bslash)
   {
      for (int i = 0; i < 8; ++i)
      {
         if (out[i] == '\\')
            out[i] = '^';
      }
   }
}

//
// Lump::writeHead
//
void Lump::writeHead(FILE *out)
{
   write_32(out, offset);
   write_32(out, size());
   write_name(out, name);
}

//
// Lump::writeName
//
void Lump::writeNameFile(char *out)
{
   write_name_file(out, name);
}

//
// Lump::writeName
//
void Lump::writeNameString(FILE *out)
{
   char namestr[9];

   namestr[0] = (name >> 56) & 0xFF;
   namestr[1] = (name >> 48) & 0xFF;
   namestr[2] = (name >> 40) & 0xFF;
   namestr[3] = (name >> 32) & 0xFF;
   namestr[4] = (name >> 24) & 0xFF;
   namestr[5] = (name >> 16) & 0xFF;
   namestr[6] = (name >>  8) & 0xFF;
   namestr[7] = (name >>  0) & 0xFF;
   namestr[8] = '\0';

   fputs(namestr, out);
}



// EOF

