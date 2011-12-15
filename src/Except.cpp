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
// Exception Handling
//
//-----------------------------------------------------------------------------

#include "Except.hpp"

#include <cstdarg>
#include <cstdio>
#include <cstring>



//
// Exception::Exception
//
Exception::Exception() : whatmsg(NULL), whatlen(0)
{
}

//
// Exception::Exception
//
Exception::Exception(Exception const &e)
{
   whatlen = e.whatlen;
   whatmsg = new char[whatlen];
   memcpy(whatmsg, e.whatmsg, whatlen);
}

//
// Exception::Exception
//
Exception::Exception(char const *msg)
{
   whatlen = strlen(msg)+1;
   whatmsg = new char[whatlen];
   memcpy(whatmsg, msg, whatlen);
}

//
// Exception::Exception
//
Exception::Exception(char const *msg, size_t len)
{
   whatlen = len+1;
   whatmsg = new char[whatlen];
   memcpy(whatmsg, msg, whatlen);
}

//
// Exception::~Exception
//
Exception::~Exception() throw()
{
   delete[] whatmsg;
}

//
// Exception::error
//
void Exception::error(char const *format, ...)
{
   char msg[1024];
   int len;

   va_list args;
   va_start(args, format);
   len = vsnprintf(msg, 1024, format, args);
   va_end(args);

   if (len > 1024) len = 1024;

   throw Exception(msg, len);
}



// EOF

