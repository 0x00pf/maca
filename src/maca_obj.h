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

#ifndef MACA_OBJ_H
#define MACA_OBJ_H

#include <stdint.h>
#include <stddef.h>

#define MO_STATUS_MAPPED 1
#define MO_STATUS_ANALYSED 2

typedef struct maca_section_t{
  char        *name;
  uint16_t    type;
  uint64_t    addr;
  uint64_t    off;
  uint64_t    size;
  uint64_t    flags;
  uint32_t    link;
  uint32_t    info;
  uint64_t    ent_size;
} MACA_SECTION;

typedef struct maca_ph_t {
  uint64_t    type;
  uint64_t    flags;
  uint64_t    off;
  uint64_t    vaddr;
  uint64_t    paddr;
  uint64_t    fsize;
  uint64_t    msize;
  uint64_t    align;
} MACA_PH;

typedef struct maca_dyn_t {
  uint64_t    tag;
  char        *name;
  union {
    uint64_t  val;
    void      *ptr;
  } un;
} MACA_DYN;


// TODO: Add zig, Nim and Crystal
#define LANG_UNKNOWN 0
#define LANG_C       1
#define LANG_CPP     2
#define LANG_RUST    3
#define LANG_GO      4
#define LANG_LAST    5

extern char *lang[LANG_LAST];

typedef struct maca_obj_t {
  char           *fname;
  float           lang[LANG_LAST];
  unsigned char  *p;     // Mapped file
  size_t          mem_size;
  size_t          file_size;
  uint8_t         status;
  uint16_t        (*get_u16)(uint16_t);
  uint32_t        (*get_u32)(uint32_t);
  uint64_t        (*get_u64)(uint64_t);  
  uint16_t        isa;  
  uint8_t         class;    // 32/64 bits
  uint8_t         endian;   // Endianness
  uint8_t         osabi;    // OS ABI
  uint8_t         osabiver; // OS ABI Version
  uint16_t        type;     // PIE, NON-Pie
  uint16_t        machine;  // target machine
  uint64_t        entry;
  uint16_t        n_s;
  uint16_t        n_p;
  uint16_t        n_dn;
  uint16_t        sent_size;
  uint16_t        pent_size;
  uint64_t        s_off;
  uint64_t        p_off;
  uint16_t        hsize;
  uint16_t        sstr;        //String section
  char            *str;        // Strings table
  char            *dstr;       // Strings table for Dynamic Section
  MACA_SECTION    *s;
  MACA_PH         *ph;
  MACA_PH         *pdyn;
  MACA_DYN        *dyn;

} MACA_OBJ;


#define MAX_STR_SIZE 8192
#define BUFFER_SIZE  1024

extern uint16_t (*get_u16)(uint16_t);
extern uint32_t (*get_u32)(uint32_t);
extern uint64_t (*get_u64)(uint64_t);

#ifdef __cplusplus
extern "C" {
#endif
  
  MACA_OBJ *maca_obj_new (char *fname);
  void      maca_obj_free (MACA_OBJ *o);
  
#ifdef __cplusplus
}
#endif

  
#endif
