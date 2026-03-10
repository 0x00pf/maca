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

#ifndef MACA_OUT_H
#define MACA_OUT_H

#include "maca_obj.h"

#ifdef __cplusplus
extern "C" {
#endif

  int maca_out_elf_info (MACA_OBJ *o);
  int maca_out_elf_sections (MACA_OBJ *o);
  int maca_out_elf_segments (MACA_OBJ *o);
  int maca_out_elf_dyn_section (MACA_OBJ *o);
  int maca_out_strings (MACA_OBJ *o, size_t size, int print);
  int maca_out_file_lang (MACA_OBJ *o);
  
#ifdef __cplusplus
}
#endif



#endif

