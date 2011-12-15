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
// Program Start Point
//
//-----------------------------------------------------------------------------

#include "Except.hpp"
#include "io.hpp"
#include "Lump.hpp"
#include "option.hpp"

#include <cstdlib>
#include <cstring>

typedef option::option_args option_args;



//
// option: -h, --help
//
static int handle_help(char const *, int, int, char const *const *)
{
   option::print_help(stderr);
   exit(0);
}
static option::option_call
option_help('h', "help", NULL, "Prints usage and exits.", NULL, handle_help);

//
// option: -i, --iwad
//
static option::option_data<bool>
option_iwad('i', "iwad", "output", "Generates an IWAD instead of a PWAD.", NULL);

//
// option: -l, --list
//
static option::option_data<bool>
option_list('l', "list", "output", "Lists all loaded lumps to stdout.", NULL);

//
// option: -o, --output
//
static option::option_cstr
option_output('o', "output", "output",
"Designates output file. (Defaults to last loose arg.)", NULL);

//
// option: -u, --unwad
//
static option::option_data<bool>
option_unwad('u', "unwad", "output",
"Outputs lumps into a directory instead of a WAD archive.", NULL);

//
// output_dir
//
static inline void output_dir()
{
   enum
   {
      MODE_NORM,
      MODE_X,
      MODE_XX,
      MODE_BM,
      MODE_TM,
   } mode = MODE_NORM;

   size_t dirlen = strlen(option_output.data);
   char  *dirtmp = new char[dirlen + 25];
   char  *dirend = dirtmp + dirlen;
   char  *dirext;

   LumpName name    = 0;
   LumpName namealt = 0;

   memcpy(dirtmp, option_output.data, dirlen);

   // Ensure PATH_SEP-termination.
   if (dirtmp[dirlen-1] != PATH_SEP)
      *dirend++ = PATH_SEP;

   dirext = dirend;
   *dirext = '\0';
   make_dir(dirtmp);

   for (Lump *it = Lump::head; it != Lump::end; it = it->next)
   {
      switch (mode)
      {
      case MODE_NORM:
         if ((it->name & LNM_X_START) == LN_X_START)
         {
            mode = MODE_X;
            name = it->name & LNM_X;

            // A hack that's needed for some WADs?
            namealt = name | (name >> 8);

            it->writeNameFile(dirext);
            dirext += 7;
         }
         else if ((it->name & LNM_XX_START) == LN_XX_START)
         {
            mode = MODE_XX;
            name = it->name & LNM_XX;

            // A hack that's needed for some WADs?
            if ((name & LNM_X) == (name << 8))
               namealt = name & LNM_X;
            else
               namealt = 0;

            it->writeNameFile(dirext);
            dirext += 8;
         }
         else if (it->next != Lump::end && it->next->name == LN_THINGS)
         {
            mode = MODE_BM;

            *dirext++ = 'M';
            *dirext++ = 'A';
            *dirext++ = 'P';
            *dirext++ = 'S';
            *dirext++ = PATH_SEP;
            *dirext   = '\0';
            make_dir(dirtmp);

            it->writeNameFile(dirext);
            while (*dirext) dirext++;
         }
         else if (it->next != Lump::end && it->next->name == LN_TEXTMAP)
         {
            mode = MODE_TM;

            *dirext++ = 'M';
            *dirext++ = 'A';
            *dirext++ = 'P';
            *dirext++ = 'S';
            *dirext++ = PATH_SEP;
            *dirext   = '\0';
            make_dir(dirtmp);

            it->writeNameFile(dirext);
            while (*dirext) dirext++;
         }

         if (mode != MODE_NORM)
         {
            *dirext++ = PATH_SEP;
            *dirext   = '\0';
            make_dir(dirtmp);
            continue; // skip markers
         }

         break;

      case MODE_X:
         if (((it->name & LNM_X_END) == LN_X_END &&
              (it->name & LNM_X) == name) ||
              (namealt &&
              (it->name & LNM_XX_END) == LN_XX_END &&
              (it->name & LNM_XX) == namealt))
         {
            mode = MODE_NORM;
            dirext = dirend;
            continue;
         }
         break;

      case MODE_XX:
         if (((it->name & LNM_XX_END) == LN_XX_END &&
              (it->name & LNM_XX) == name) ||
              (namealt &&
              (it->name & LNM_X_END) == LN_X_END &&
              (it->name & LNM_X) == namealt))
         {
            mode = MODE_NORM;
            dirext = dirend;
            continue;
         }
         break;

      case MODE_BM:
         if (!Lump::name_is_binmap(it->name))
         {
            mode = MODE_NORM;
            dirext = dirend;
            break;
         }
         break;

      case MODE_TM:
         if (it->name == LN_ENDMAP)
         {
            mode = MODE_NORM;
            dirext = dirend;
            continue;
         }
         break;
      }

      it->writeNameFile(dirext);

      FILE *file = fopen(dirtmp, "wb");
      if (!file) Exception::error("unable to fopen '%s'", dirtmp);

      it->writeData(file);

      fclose(file);
   }

   delete[] dirtmp;
}

//
// output_list
//
static inline void output_list()
{
   for (Lump *it = Lump::head; it != Lump::end; it = it->next)
   {
      it->writeNameString(stdout);
      fputc('\n', stdout);
   }
}

//
// output_wad
//
static inline void output_wad()
{
   FILE *wad = fopen(option_output.data, "wb");

   if (!wad)
      Exception::error("unable to fopen '%s'", option_output.data);

   Lump::calculate_offsets();

   fputs(option_iwad.data ? "IWAD" : "PWAD", wad);
   Lump::write_32(wad, Lump::count);
   Lump::write_32(wad, 16);
   Lump::write_32(wad, 0);

   for (Lump *it = Lump::head; it != Lump::end; it = it->next)
      it->writeHead(wad);

   for (Lump *it = Lump::head; it != Lump::end; it = it->next)
      it->writeData(wad);

   fclose(wad);
}

//
// _main
//
static inline int _main(int argc, char **argv)
{
   option::help_program = "DH-wad";

   option::help_usage = "[option]...\n[--output] OUTPUT\nSOURCE";

   option::help_desc_s = "If no output specified, defaults to using the last"
                        " loose arg. (If still none, no output is done.) All"
                        " remaining loose args are loaded as -d after all"
                        " options.\n\n"
                         "Inputs are added in the order specified on the"
                        " command line. Generally, the lump name may be omitted"
                        " from pairs (along with the '=') and it will be"
                        " guessed based on the filename.";

   // If no args, just print help and exit.
   if (argc <= 1)
   {
      option::print_help(stderr);
      return 0;
   }

   // Process the options.
   option::process_options(argc-1, argv+1, option::OPTF_KEEPA);

   // Output defaults to the last free arg.
   if (!option_output.data && option_args::arg_count)
      option_output.data = option_args::arg_vector[--option_args::arg_count];

   // Any remaining loose args are now -d.
   for (size_t i = 0; i < option_args::arg_count; ++i)
      Lump::create_directory(option_args::arg_vector[i]);

   // If no lumps, use output as a base for source.
   if (!Lump::count && option_output.data)
   {
      char const *out = option_output.data;
      size_t len = strlen(out);

      char *tmp = new char[len+5];
      char *end = tmp+len;
      memcpy(tmp, out, len);

      // If output ends in .wad, strip the extension and load as special.
      // Otherwise, load as special and add extension to output. For --unwad,
      // try to load the wad and output the special.
      if (len >= 4 && end[-4] == '.' && end[-3] == 'w' && end[-2] == 'a' &&
          end[-1] == 'd')
      {
         *(end -= 4) = '\0';

         if (option_unwad.data)
         {
            Lump::create_wad(out);
            option_output.data = tmp;
         }
         else
         {
            Lump::create_special(Lump::name_from_file(tmp), tmp);
            delete[] tmp;
         }
      }
      else
      {
         *end++ = '.'; *end++ = 'w'; *end++ = 'a'; *end++ = 'd'; *end++ = '\0';

         if (option_unwad.data)
         {
            Lump::create_wad(tmp);
            delete[] tmp;
         }
         else
         {
            Lump::create_special(Lump::name_from_file(out), out);
            option_output.data = tmp;
         }
      }
   }

   // Write the list of lumps to stdout.
   if (option_list.data)
      output_list();

   // If still no option_output, then not doing output.
   if (!option_output.data)
      return 0;

   // Do output.
   if (option_unwad.data)
      output_dir();
   else
      output_wad();

   return 0;
}

//
// main
//
int main(int argc, char **argv)
{
   try
   {
      return _main(argc, argv);
   }
   catch (std::exception const &e)
   {
      fputs(e.what(), stderr);
      fputc('\n', stderr);
   }

   return 1;
}



// EOF

