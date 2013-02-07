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
// I/O Abstraction
//
//-----------------------------------------------------------------------------

#include "io.hpp"

#include "Except.hpp"

#ifdef TARGET_OS_WIN32
#define CINTERFACE
#include <windows.h>
#include <sys/stat.h>

#define stat _stat
#define S_IFDIR _S_IFDIR
#define S_IFMT  _S_IFMT
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#include <cstring>


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// is_dir
//
bool is_dir(char const * dirname)
{
	struct stat stat_data;
	int stat_res = stat(dirname, &stat_data);

	// 0 means success.
	if (stat_res != 0) return false;

	return (stat_data.st_mode & S_IFMT) == S_IFDIR;
}

//
// is_file
//
bool is_file(char const * filename)
{
	struct stat stat_data;
	int stat_res = stat(filename, &stat_data);

	// 0 means success.
	if (stat_res != 0) return false;

	return (stat_data.st_mode & S_IFMT) == S_IFREG;
}

//
// IterateDir
//
void IterateDir(char const *dirname, FileHandler handler, void *data)
{
   char *fileend;
   char *filetmp;

   size_t dirlen = strlen(dirname);

   #ifdef TARGET_OS_WIN32
   //
   // WIN32 directory iteration.
   //

   HANDLE          dir;
   WIN32_FIND_DATA diritr;

   filetmp = new char[dirlen+1 + MAX_PATH+1];
   fileend = filetmp+dirlen;

   memcpy(filetmp, dirname, dirlen);

   if (filetmp[dirlen-1] != PATH_SEP)
      *fileend++ = PATH_SEP;

   fileend[0] = '*';
   fileend[1] = '\0';

   dir = FindFirstFile(filetmp, &diritr);

   if (dir == INVALID_HANDLE_VALUE)
      Exception::error("unable to FindFirstFile '%s'", dirname);

   do
   {
      // Don't handle self or parent.
      if (diritr.cFileName[0] == '.' && (diritr.cFileName[1] == '\0' ||
         (diritr.cFileName[1] == '.' &&  diritr.cFileName[2] == '\0')))
      {
         continue;
      }

      strcpy(fileend, diritr.cFileName);
      handler(filetmp, data);
   }
   while (FindNextFile(dir, &diritr));

   if (GetLastError() != ERROR_NO_MORE_FILES)
      Exception::error("unable to FindNextFile '%s'", dirname);

   delete[] filetmp;
   FindClose(dir);

   #else
   //
   // POSIX directory iteration.
   //

   DIR*    dir;
   dirent  diritr;
   dirent *dirres;

   if (!(dir = opendir(dirname)))
      Exception::error("unable to opendir '%s'", dirname);

   filetmp = new char[dirlen+1 + NAME_MAX+1];
   fileend = filetmp+dirlen;

   memcpy(filetmp, dirname, dirlen);

   // Ensure PATH_SEP-termination.
   if (filetmp[dirlen-1] != PATH_SEP)
      *fileend++ = PATH_SEP;

   while (true)
   {
      if (readdir_r(dir, &diritr, &dirres) != 0)
         Exception::error("unable to readdir_r '%s'", dirname);

      if (!dirres) break;

      // Don't handle self or parent.
      if (diritr.d_name[0] == '.' && (diritr.d_name[1] == '\0' ||
         (diritr.d_name[1] == '.' &&  diritr.d_name[2] == '\0')))
      {
         continue;
      }

      strcpy(fileend, diritr.d_name);
      handler(filetmp, data);
   }

   delete[] filetmp;
   closedir(dir);
   #endif
}

//
// make_dir
//
void make_dir(char const *dirname)
{
   #ifdef TARGET_OS_WIN32
   CreateDirectory(dirname, NULL);
   #else
   mkdir(dirname, S_IRWXU);
   #endif
}

// EOF

