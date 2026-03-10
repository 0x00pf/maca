/*
  MACA
  Copyright (c) 2026 pico

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MACA_UTIL_H
#define MACA_UTIL_H
#include <stdint.h>

#define FG_LYELLOW "\033[1;33m"
#define FG_YELLOW "\033[0;33m"
#define FG_GREEN "\033[1;32m"
#define FG_LGREEN "\033[0;32m"
#define FG_BLUE "\033[1;34m"
#define BG_LBLUE "\033[1;44m"
#define BG_BLUE "\033[0;44m"
#define BG_CYAN "\033[1;30;1;46m"
#define BG_MAGENTA "\033[1;37;1;45m"

#define FG_CYAN "\033[1;36m"
#define FG_MAGENTA "\033[1;35m"
#define FG_LBLUE "\033[0;34m"
#define FG_LWHITE "\033[1;37m"
#define FG_NORMAL "\033[0;37m"

#define BG_LMAGENTA "\033[37;1;45;0m"
#define BG_RED2 "\033[0;37;1;41m"
#define BG_GREEN2 "\033[0;37;1;42m"
#define FG_RED "\033[0;31m"
#define FG_LRED "\033[1;31m"
#define BG_RED "\033[0;44;1;33m"
#define BG_WHITE "\033[30;47m"

#define RESET "\033[0m"


#define N_TYPE 5
#define N_OSABI 20
#define N_ISA 259

extern char *type[N_TYPE];
extern char *osabi[N_OSABI];
extern char *isa[N_ISA];

typedef struct config_t {
  // Add configuration flags here
  uint8_t  show_strings;
  int      strings_size;
} MACA_CONF;

#ifdef __cplusplus
extern "C" { 
#endif

  int        maca_util_elf_get_s_flags (char s[32], uint64_t f);
  char      *maca_util_elf_get_ph_type (uint64_t type);
  char      *maca_util_elf_get_dtag    (MACA_OBJ *o, int i);
  char      *maca_util_elf_get_flag    (uint64_t i, char str[1024]);
  char      *maca_util_elf_ph_perm     (uint64_t i, char str[128]);
  char      *maca_util_elf_get_flag1   (uint64_t i, char str[1024]);
  char      *maca_util_elf_get_isa     (MACA_OBJ *obj);
  int        maca_util_get_file_size   (int fd);  
  MACA_CONF *maca_util_get_conf ();

#ifdef __cplusplus
}
#endif


#endif
