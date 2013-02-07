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
// Option: -w, --wad
//
//-----------------------------------------------------------------------------

#include "Lump.hpp"

#include "Except.hpp"
#include "option.hpp"
#include "Wad.hpp"

#ifdef TARGET_OS_POSIX
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif


//----------------------------------------------------------------------------|
// Options                                                                    |
//

//
// option: -w, --wad
//
static int HandleWad(char const *opt, int optf, int argc,
                      char const *const *argv)
{
   if(!argc) option::exception::error(opt, optf, "requires argument");

   Lump::CreateWad(&Wad::RootWad, argv[0]);

   return 1;
}
static option::option_call OptionWad('w', "wad", "input",
   "Adds a WAD's lumps.", NULL, HandleWad);


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Lump::CreateWad
//
void Lump::CreateWad(Wad *wad, char const *filename)
{
   unsigned char *data;

   #ifdef TARGET_OS_POSIX
   // Try to load the data using mmap or read.
   {
      int fd = open(filename, O_RDONLY);
      if(fd == -1)
         Exception::error("unable to open '%s'", filename);

      struct stat s;

      if(fstat(fd, &s))
         Exception::error("unable to fstat '%s'", filename);

      void *map = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

      // If mmap fails, try read instead.
      if(map == MAP_FAILED)
      {
         data = new unsigned char[s.st_size];

         if(read(fd, data, s.st_size) != s.st_size)
            Exception::error("unable to mmap or read '%s'", filename);
      }
      else
         data = static_cast<unsigned char *>(map);

      close(fd);
   }
   #else
   // Try to load the data using fread.
   {
      std::FILE *file = std::fopen(filename, "rb");
      if(!file) Exception::error("unable to fopen '%s'", filename);

      if(fseek(file, 0, SEEK_END))
         Exception::error("unable to fseek '%s'", filename);

      long pos = std::ftell(file);

      if(pos < 0)
         Exception::error("unable to ftell '%s'", filename);

      std::size_t len = static_cast<std::size_t>(pos);

      data = new unsigned char[len];

      std::fseek(file, 0, SEEK_SET);
      if(std::fread(data, 1, len, file) != len)
         Exception::error("unable to fread '%s'", filename);

      std::fclose(file);
   }
   #endif

   // Now to load the contained lumps.
   std::size_t hcount = data[ 4] <<  0L |
                        data[ 5] <<  8L |
                        data[ 6] << 16L |
                        data[ 7] << 24L;
   std::size_t offset = data[ 8] <<  0L |
                        data[ 9] <<  8L |
                        data[10] << 16L |
                        data[11] << 24L;

   unsigned char *header = data+offset;
   unsigned char *headend = header + (hcount * 16);

   for(; header != headend; header += 16)
   {
      std::size_t start = header[0] <<  0L |
                          header[1] <<  8L |
                          header[2] << 16L |
                          header[3] << 24L;
      std::size_t len   = header[4] <<  0L |
                          header[5] <<  8L |
                          header[6] << 16L |
                          header[7] << 24L;
      LumpName name = name_from_string(reinterpret_cast<char *>(header + 8));

      CreateDataKeep(wad, name, reinterpret_cast<char *>(data + start), len);
   }

   // Don't free data, created lumps point to it.
}

// EOF

