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
// Lump Handling: --data
//
//-----------------------------------------------------------------------------

#include "Lump.hpp"
#include "option.hpp"

#include <cstring>



class LumpData : public Lump
{
public:
   LumpData(LumpName name, char const *data, size_t len);

   virtual size_t size();

   virtual void writeData(FILE *out);

private:
   char const *data;
   size_t len;
};



//
// option: --data
//
static int handle_data(char const *opt, int optf, int argc,
                       char const *const *argv)
{
   if (!argc) option::exception::error(opt, optf, "requires argument");

   char const *data = strchr(argv[0], '=');
   LumpName name;

   if (data)
      name = Lump::name_from_string(argv[0], data++);
   else
   {
      data = argv[0];
      name = Lump::name_from_string(data);
   }

   if (!*data)
      Lump::create_null(name);
   else if (optf & option::OPTF_KEEPA)
      Lump::create_data_keep(name, data, strlen(data));
   else
      Lump::create_data(name, data, strlen(data));

   return 1;
}
static option::option_call
option_data('\0', "data", "input", "Adds a lump=data pair.", NULL, handle_data);

//
// Lump::create_data
//
void Lump::create_data(LumpName name, char const *data, size_t len)
{
   char *d = new char[len];
   memcpy(d, data, len);
   new LumpData(name, d, len);
}

//
// Lump::create_data_keep
//
void Lump::create_data_keep(LumpName name, char const *data, size_t len)
{
   new LumpData(name, data, len);
}

//
// LumpData::LumpData
//
LumpData::LumpData(LumpName _name, char const *_data, size_t _len)
                   : Lump(_name), data(_data), len(_len)
{
}

//
// LumpData::size
//
size_t LumpData::size()
{
   return len;
}

//
// LumpData::writeData
//
void LumpData::writeData(FILE *out)
{
   fwrite(data, 1, len, out);
}



// EOF

