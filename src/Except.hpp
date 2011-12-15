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

#ifndef HPP_Except_
#define HPP_Except_

#include <stdexcept>



class Exception : public std::exception
{
public:
   Exception();
   Exception(Exception const &e);
   Exception(char const *msg);
   Exception(char const *msg, size_t len);
   ~Exception() throw();

   char const *what() const throw() {return whatmsg;}


   static void error(char const *format, ...);

private:
   char *whatmsg;
   size_t whatlen;
};



#endif//HPP_Except_

