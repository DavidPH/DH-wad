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
// Lump Handling: --data
//
//-----------------------------------------------------------------------------

#include "Lump.hpp"

#include "option.hpp"
#include "Wad.hpp"

#include <cstring>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// LumpData
//
class LumpData : public Lump
{
public:
   LumpData(LumpName _name, char const *_data, std::size_t _len) : Lump(_name),
      data(_data), len(_len) {}

   virtual std::size_t size() {return len;}

   virtual void writeData(std::FILE *out) {std::fwrite(data, 1, len, out);}

private:
   char const *data;
   std::size_t len;
};


//----------------------------------------------------------------------------|
// Options                                                                    |
//

//
// option: --data
//
static int HandleData(char const *opt, int optf, int argc, char const *const *argv)
{
   if(!argc) option::exception::error(opt, optf, "requires argument");

   char const *data = strchr(argv[0], '=');
   LumpName name;

   if(data)
      name = Lump::name_from_string(argv[0], data++);
   else
   {
      data = argv[0];
      name = Lump::name_from_string(data);
   }

   if(!*data)
      Lump::CreateNull(&Wad::RootWad, name);
   else if(optf & option::OPTF_KEEPA)
      Lump::CreateDataKeep(&Wad::RootWad, name, data, strlen(data));
   else
      Lump::CreateData(&Wad::RootWad, name, data, strlen(data));

   return 1;
}
static option::option_call OptionData('\0', "data", "input",
   "Adds a lump=data pair.", NULL, HandleData);


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Lump::CreateData
//
void Lump::CreateData(Wad *wad, LumpName name, char const *data, std::size_t len)
{
   char *d = new char[len];
   std::memcpy(d, data, len);
   CreateDataKeep(wad, name, d, len);
}

//
// Lump::CreateDataKeep
//
void Lump::CreateDataKeep(Wad *wad, LumpName name, char const *data, std::size_t len)
{
   wad->insert(new LumpData(name, data, len));
}

// EOF

