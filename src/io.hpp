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
// I/O Abstraction
//
//-----------------------------------------------------------------------------

#ifndef HPP_io_
#define HPP_io_



#ifdef TARGET_OS_WIN32
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif



typedef void (*FileHandler)(char const *filename);

bool is_dir(char const *dirname);
bool is_file(char const *filename);

void iter_dir(char const *dirname, FileHandler handler);

void make_dir(char const *dirname);



#endif//HPP_io_

