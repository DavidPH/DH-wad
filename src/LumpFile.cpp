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
// Lump Handling: --file
//
//-----------------------------------------------------------------------------

#include "Except.hpp"
#include "Lump.hpp"
#include "option.hpp"

#include <cstring>

#ifdef TARGET_OS_POSIX
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif



class LumpFile : public Lump
{
public:
   LumpFile(LumpName name, char const *filename);

   virtual size_t size();

   virtual void writeData(FILE *out);

private:
   char const *filename;
};



//
// option: -f, --file
//
static int handle_file(char const *opt, int optf, int argc,
                       char const *const *argv)
{
   if (!argc) option::exception::error(opt, optf, "requires argument");

   char const *filename = strchr(argv[0], '=');
   LumpName name;

   if (filename)
      name = Lump::name_from_string(argv[0], filename++);
   else
   {
      filename = argv[0];
      name = Lump::name_from_file(filename);
   }

   if (!*filename)
      Lump::create_null(name);
   else if (optf & option::OPTF_KEEPA)
      Lump::create_file_keep(name, filename);
   else
      Lump::create_file(name, filename);

   return 1;
}
static option::option_call
option_file('f', "file", "input", "Adds a lump=file pair.", NULL, handle_file);

//
// Lump::create_file
//
void Lump::create_file(LumpName name, char const *filename)
{
   create_file(name, filename, strlen(filename));
}

//
// Lump::create_file
//
void Lump::create_file(LumpName name, char const *filename, size_t len)
{
   char *str = new char[len+1];
   memcpy(str, &filename[0], len);
   str[len] = '\0';
   new LumpFile(name, str);
}

//
// Lump::create_file_keep
//
void Lump::create_file_keep(LumpName name, char const *filename)
{
   new LumpFile(name, filename);
}

//
// LumpFile::LumpFile
//
LumpFile::LumpFile(LumpName _name, char const *_filename)
                   : Lump(_name), filename(_filename)
{
}

//
// LumpFile::size
//
size_t LumpFile::size()
{
   #ifdef TARGET_OS_POSIX
   struct stat s;

   if (stat(filename, &s))
      Exception::error("unable to stat '%s'", filename);

   return s.st_size;
   #else
   FILE *file = fopen(filename, "rb");
   if (!file) Exception::error("unable to fopen '%s'", filename);

   if (fseek(file, 0, SEEK_END))
      Exception::error("unable to fseek '%s'", filename);

   long pos = ftell(file);

   if (pos == -1L)
      Exception::error("unable to ftell '%s'", filename);

   return static_cast<size_t>(pos);
   #endif
}

//
// LumpFile::writeData
//
void LumpFile::writeData(FILE *out)
{
   int buf;
   FILE *file = fopen(filename, "rb");
   if (!file) Exception::error("unable to fopen '%s'", filename);

   while ((buf = getc(file)) != EOF)
      fputc(buf, out);
}



// EOF

