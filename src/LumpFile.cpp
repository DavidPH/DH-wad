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
// Lump Handling: --file
//
//-----------------------------------------------------------------------------

#include "Lump.hpp"

#include "Except.hpp"
#include "option.hpp"
#include "Wad.hpp"

#include <cstring>

#ifdef TARGET_OS_POSIX
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// LumpFile
//
class LumpFile : public Lump
{
public:
   LumpFile(LumpName _name, char const *_filename) : Lump(_name), filename(_filename) {}

   virtual std::size_t size();

   virtual void writeData(std::FILE *out);

private:
   char const *filename;
};


//----------------------------------------------------------------------------|
// Options                                                                    |
//

//
// option: -f, --file
//
static int HandleFile(char const *opt, int optf, int argc, char const *const *argv)
{
   if(!argc) option::exception::error(opt, optf, "requires argument");

   char const *filename = strchr(argv[0], '=');
   LumpName name;

   if(filename)
      name = Lump::name_from_string(argv[0], filename++);
   else
   {
      filename = argv[0];
      name = Lump::name_from_file(filename);
   }

   if(!*filename)
      Lump::CreateNull(&Wad::RootWad, name);
   else if(optf & option::OPTF_KEEPA)
      Lump::CreateFileKeep(&Wad::RootWad, name, filename);
   else
      Lump::CreateFile(&Wad::RootWad, name, filename);

   return 1;
}
static option::option_call OptionFile('f', "file", "input",
   "Adds a lump=file pair.", NULL, HandleFile);


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Lump::CreateFile
//
void Lump::CreateFile(Wad *wad, LumpName name, char const *filename)
{
   std::size_t len = std::strlen(filename) + 1;
   CreateFileKeep(wad, name, static_cast<char *>(memcpy(new char[len], filename, len)));
}

//
// Lump::CreateFile
//
void Lump::CreateFile(Wad *wad, LumpName name, char const *filename, size_t len)
{
   char *str = new char[len + 1];
   memcpy(str, filename, len); str[len] = '\0';
   CreateFileKeep(wad, name, str);
}

//
// Lump::CreateFileKeep
//
void Lump::CreateFileKeep(Wad *wad, LumpName name, char const *filename)
{
   wad->insert(new LumpFile(name, filename));
}

//
// LumpFile::size
//
std::size_t LumpFile::size()
{
   #ifdef TARGET_OS_POSIX
   struct stat s;

   if(stat(filename, &s))
      Exception::error("unable to stat '%s'", filename);

   return s.st_size;
   #else
   std::FILE *file = std::fopen(filename, "rb");
   if(!file) Exception::error("unable to fopen '%s'", filename);

   if(std::fseek(file, 0, SEEK_END))
      Exception::error("unable to fseek '%s'", filename);

   long pos = std::ftell(file);

   if(pos < 0)
      Exception::error("unable to ftell '%s'", filename);

   return static_cast<std::size_t>(pos);
   #endif
}

//
// LumpFile::writeData
//
void LumpFile::writeData(FILE *out)
{
   int buf;
   std::FILE *file = fopen(filename, "rb");
   if(!file) Exception::error("unable to fopen '%s'", filename);

   while((buf = std::fgetc(file)) != EOF)
      std::fputc(buf, out);
}

// EOF

