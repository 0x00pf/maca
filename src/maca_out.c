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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <elf.h>
#include <ctype.h>

#include "maca_obj.h"
#include "maca_util.h"
#include "maca_out.h"

char *lang[LANG_LAST]={"Unknown", "C/ASM", "CPP", "Rust", "Go"};

int
maca_out_elf_info (MACA_OBJ *o) {
  if (!o) return -1;
  printf ( BG_WHITE "ELF Header                                                                                                             "RESET"\n");
  printf (FG_LWHITE"Description      :"RESET" [%s] ",
	  o->class == 1 ? "32 bits" : "64 bits");

  if (o->isa > N_ISA)
    printf ("Unknown Machine [%x] ", o->isa);
  else {
    printf ("[%s] ", isa[o->isa]);
  }
  printf ("[%s] ", o->endian == 1 ? "LSB" : "MSB");
    if ( o->osabi  > N_OSABI)
    printf ("[Unknown ABI (%x %x)] ", o->osabi, o->osabiver);
  else {
    printf ("[%s] ", osabi[o->osabi]);
    if (o->osabi == 3) printf (" | Dynamic Linker ABI: %x", o->osabiver);
    else printf (" | ABI Version [%x] ", o->osabiver);
  }
  
  //o->type  = get_u16(elf_hdr->e_type);
  if (o->type > N_TYPE)
    printf ("Unknown Type [%x] ", o->type);
  else
    printf ("[%s] ", type[o->type]);
  
  
  printf ("\n");
    printf (FG_LWHITE "ENTRY            : " FG_LGREEN "0x%lx" RESET "\n", o->entry);
  printf (FG_LWHITE "Header Size      : "
	  FG_CYAN " %2d  " RESET "|  " FG_LGREEN "0x%-8x" RESET "\n",
	  o->hsize, o->hsize);


  printf (FG_LWHITE "Program Headers  : "
	  FG_CYAN " %2d  " RESET "@  " FG_LGREEN "0x%-4lx" RESET " (%d bytes/entry)\n",
	  o->n_p, o->p_off,o->pent_size);
  
  printf (FG_LWHITE "Sections         : "
	  FG_CYAN " %2d  " RESET "@  " FG_LGREEN "0x%-4lx" RESET " (%d bytes/entry)\n",
	  o->n_s, o->s_off, o->sent_size);
  
  printf (FG_LWHITE "Sections Names   : "
	  FG_CYAN "[%2d] " RESET "-> " FG_LGREEN "0x%-4lx" RESET "\n",
	  o->sstr,
	  o->s_off + o->sstr * o->sent_size);
  return 0;
}

int
maca_out_elf_sections (MACA_OBJ *o) {
  printf ( BG_WHITE "SECTIONS                                                                                                               "RESET"\n");
  if (o->n_s) {
    printf (FG_LWHITE" N     NAME                  TYPE   ADD    OFF     SZ ES   F  L  I\n"RESET);
    for (int i = 0; i < o->n_s; i++) {
      if (!strcmp(o->s[i].name, ".text"))              printf (FG_LGREEN);
      else if (!strcmp  (o->s[i].name, ".rodata"))     printf (FG_CYAN);
      else if (!strcmp  (o->s[i].name, ".data"))       printf (FG_GREEN);
      else if (!strcmp  (o->s[i].name, ".bss"))        printf (FG_GREEN);
      else if (!strcmp  (o->s[i].name, ".interp"))     printf (FG_LYELLOW);
      else if (!strcmp  (o->s[i].name, ".plt"))        printf (FG_YELLOW);
      else if (!strcmp  (o->s[i].name, ".plt.got"))    printf (FG_YELLOW);
      else if (!strcmp  (o->s[i].name, ".got"))        printf (FG_YELLOW);
      else if (!strcmp  (o->s[i].name, ".got.plt"))    printf (FG_YELLOW);
      else if (!strcmp  (o->s[i].name, ".comment"))    printf (FG_LYELLOW);
      else if (!strcmp  (o->s[i].name, ".init"))       printf (FG_LBLUE);
      else if (!strcmp  (o->s[i].name, ".fini"))       printf (FG_LBLUE);
      else if (!strcmp  (o->s[i].name, ".init_array")) printf (FG_LBLUE);
      else if (!strcmp  (o->s[i].name, ".fini_array")) printf (FG_LBLUE);
      else if (!strncmp (o->s[i].name, ".debug",6))    printf (FG_MAGENTA);
      else if (!strncmp (o->s[i].name, ".note",5))     printf (FG_LYELLOW);
      char f[32];
      maca_util_elf_get_s_flags(f, o->s[i].flags);
      printf ("[%02d] %20s %4x %8lx %6lx %6lx %02ld %3s %2d %2d\n" RESET,
	      i, o->s[i].name, o->s[i].type,
	      o->s[i].addr, o->s[i].off, o->s[i].size,
	      o->s[i].ent_size, f, o->s[i].link, o->s[i].info);
    }
  } else {
    printf (FG_LRED "Stripped binary\n" RESET); 
  }
  return 0;
}

int
maca_out_elf_segments (MACA_OBJ *o) {
  if (!o) return -1;
  printf ( BG_WHITE "PROGRAM HEADERS                                                                                                        "RESET"\n");

  printf (FG_LWHITE"[  ]             TYPE PERM    VADDR     PADDR       OFFSET FILESIZE  MEMSIZE    ALIGN\n"RESET);
  for (int i = 0; i < o->n_p; i++) {
    char perm[128];
    memset (perm,0, 128);
    if (o->ph[i].type == PT_LOAD) printf (FG_LGREEN);
    else if (o->ph[i].type == PT_INTERP) printf (FG_LYELLOW);
    else if (o->ph[i].type == PT_NOTE) printf (FG_CYAN);
    maca_util_elf_ph_perm (o->ph[i].flags, perm);
    printf ("[%02d] %16s [%lx] %3s %8lx %8lx %8lx %08lx %8lx %8lx\n" RESET,
	    i, maca_util_elf_get_ph_type(o->ph[i].type), o->ph[i].flags, perm,
	    o->ph[i].vaddr, o->ph[i].paddr, o->ph[i].off,
	    o->ph[i].fsize, o->ph[i].msize,
	    o->ph[i].align
	    );
    
  }  
  return 0;
}

int
maca_out_elf_dyn_section (MACA_OBJ *o){
  if (!o) return -1;
  
  printf ( BG_WHITE "DYNAMIC SECTION                                                                                                        "RESET"\n");
  if (o->pdyn == NULL) printf ("No Dynamic Section\n");
  else {
    int show_constructors = 0;
    for (int i = 0; i < o->n_dn; i++) {
      char flags[4096];
      
      if (o->dyn[i].tag == DT_PLTGOT || o->dyn[i].tag == DT_PLTRELSZ || o->dyn[i].tag == DT_PLTREL)
	printf (FG_LGREEN);
      if (o->dyn[i].tag == DT_STRTAB || o->dyn[i].tag == DT_STRSZ || o->dyn[i].tag == DT_SYMTAB)
	printf (FG_MAGENTA);
      
      if (o->dyn[i].tag == DT_INIT || o->dyn[i].tag == DT_FINI
	  || o->dyn[i].tag == DT_INIT_ARRAY || o->dyn[i].tag == DT_FINI_ARRAY)
	printf (FG_LBLUE);
      if (o->dyn[i].tag == DT_INIT_ARRAYSZ && o->dyn[i].un.val > 8) {
	show_constructors = 1;
	printf (FG_LRED);
      }
      if (o->dyn[i].tag == DT_FINI_ARRAYSZ && o->dyn[i].un.val > 8) printf (FG_LRED);
      
      if (o->dyn[i].tag == DT_NEEDED) {
	if (strstr (o->dstr + o->dyn[i].un.val, "c++") != NULL) o->lang[LANG_CPP]+=1;
	printf (FG_LYELLOW "%-20s %s\n" RESET,
		maca_util_elf_get_dtag(o, o->dyn[i].tag), o->dstr + o->dyn[i].un.val);
      } else if (o->dyn[i].tag == DT_FLAGS) {
	maca_util_elf_get_flag(o->dyn[i].un.val, flags);
	printf (FG_LGREEN "%-20s " RESET"%s\n" RESET, maca_util_elf_get_dtag(o, o->dyn[i].tag), flags);
      }
      
      else if (o->dyn[i].tag == DT_FLAGS_1) {
	maca_util_elf_get_flag1(o->dyn[i].un.val, flags);
	printf (FG_LGREEN "%-20s " RESET"%s\n" RESET, maca_util_elf_get_dtag(o, o->dyn[i].tag), flags);
      }
      else
	printf ("%-20s 0x%lx\n", maca_util_elf_get_dtag(o, o->dyn[i].tag), o->dyn[i].un.val);
      if (show_constructors) {
	// To be completed
	show_constructors = 0;
      }
      printf (RESET);
      if (o->dyn[i].tag == DT_NULL) break;
    }
   
  }
  return 0;
}


int
maca_obj_find_sec_by_addr (MACA_OBJ *o, uint64_t addr) {
  for (int i = 0; i < o->n_s; i++) {
    if ((addr >= o->s[i].off) && (addr < (o->s[i].off + o->s[i].size))) return i;
  }
  return -1;
}


int
maca_out_strings (MACA_OBJ *o, size_t size, int out) {
  unsigned char *p, *q, *e;
  int            n;
  int            rust_ind = 0, cpp_ind = 0, go_ind = 0;
  
  if (!o) return -1;
  if (o->status < MO_STATUS_MAPPED) return -1;
  if (out)
    printf ( BG_WHITE "STRINGS                                                                                                                "RESET"\n");
  p = o->p;
  q = p;
  e = p + o->file_size;
  n = 0;
  do {
    if (*p!=0 && (isprint(*p) || isspace(*p) )) n++;
    else {
      if (n >= size) {
	if (q[1] == '_' && q[2] == 'Z' && q[3] == 'N') {
	  if (isdigit(q[4])) rust_ind++;
	  else cpp_ind++;
	}
	
	if ((strstr ((char*)q + 1, "/lib/go-")) != NULL) go_ind++;
	if ((strstr ((char*)q + 1, ":unwrap()")) != NULL) rust_ind++;
	if (out) {

	  int s = maca_obj_find_sec_by_addr (o, q - o->p);

	  // Colorize based on sections that usually contains strings
	  if (s != -1) {
	    if      (!strcmp (o->s[s].name, ".shstrtab")) printf (FG_CYAN);
	    else if (!strcmp (o->s[s].name, ".strtab")) printf (FG_CYAN);
	    else if (!strcmp (o->s[s].name, ".rodata")) printf (FG_LGREEN);
	    else if (!strcmp (o->s[s].name, ".interp")) printf (FG_LYELLOW);
	    else if (!strcmp (o->s[s].name, ".bss")) printf (FG_LYELLOW);
	    printf ("[%18s] ",o->s[s].name);
	  } 
	  printf ("0x%06lx [%03x]: ",  q-o->p, n);

	  if (n > 80) n = 80;
	  for (int i = 1; i <= n; i++) {
	    if (q[i] < 32) printf ("\\0x%02x", q[i]);
	    else printf ("%c", q[i]);
	  }
	  printf (RESET "\n");
	}
      }
      q = p;
      n = 0;
    }
  } while (++p < e);
  // Heuristics
  if (go_ind < 50) go_ind = 0;
  if (go_ind > cpp_ind && go_ind > rust_ind) o->lang[LANG_GO] += 1;
  else if (rust_ind > 20 && rust_ind > cpp_ind) o->lang[LANG_RUST] += 1;
  else if (cpp_ind > 0) o->lang[LANG_CPP] += 1; else o->lang[LANG_C] += 1;

#ifdef DEBUG  
  
  printf ("C++: %d RUST: %d GO : %d\n", cpp_ind, rust_ind, go_ind);
  printf ("C : %f C++: %f RUST: %f GO : %f\n", o->lang[LANG_C],
	  o->lang[LANG_CPP],
	  o->lang[LANG_RUST],
	  o->lang[LANG_GO]
	  );
#endif  
  return 0;
}


int
maca_out_file_lang (MACA_OBJ *o){
  if (!o) return -1;
  printf ( BG_WHITE "PROGRAMMING LANGUAGE GUESS                                                                                             "RESET"\n");
    
  uint8_t   index[LANG_LAST];
  for (int i = 0; i < LANG_LAST - 1; i++) index[i] = i+1;
  
  for (int i = 0; i < LANG_LAST - 1; i++) {
    for (int j = i; j < LANG_LAST - 1; j++)
      if (o->lang[index[j]] > o->lang[index[i]]) {
	int tmp = index[j];
	index[j] = index[i];
	index[i] = tmp;
      }
  }
  printf (FG_LGREEN "[%s] (%.2f) " RESET, lang[index[0]], o->lang[index[0]]);
  for (int i = 1; i < LANG_LAST - 1; i++) {
    printf (FG_CYAN "[%s] (%.2f) " RESET, lang[index[i]], o->lang[index[i]]);
  }
  printf ("\n");  
  return 0;
}

