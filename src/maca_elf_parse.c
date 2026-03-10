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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <ctype.h>

#include <elf.h>

#include "maca_obj.h"
#include "maca_util.h"

int
maca_read_ph32 (MACA_OBJ *o) {
  Elf32_Phdr  *p;
  
#ifdef DEBUG  
  printf ("%d program headers at %lx\n", o->n_p, o->p_off);
#endif  
  o->pdyn = NULL;
  p = (Elf32_Phdr*)(o->p + o->p_off);
  for (int i = 0; i < o->n_p; i++) {
    o->ph[i].type  = o->get_u32(p[i].p_type);
    o->ph[i].flags = o->get_u32(p[i].p_flags);
    o->ph[i].off   = o->get_u32(p[i].p_offset);
    o->ph[i].vaddr = o->get_u32(p[i].p_vaddr);
    o->ph[i].paddr = o->get_u32(p[i].p_paddr);
    o->ph[i].fsize = o->get_u32(p[i].p_filesz);
    o->ph[i].msize = o->get_u32(p[i].p_memsz);
    o->ph[i].align = o->get_u32(p[i].p_align);
    if (o->ph[i].type == PT_DYNAMIC) o->pdyn = &o->ph[i];
    
#if DEBUG
    printf ("[%02d] %16s %lx %08lx %08lx %08lx %08lx %08lx\n",
	    i, maca_util_elf_get_ph_type(o->ph[i].type), o->ph[i].flags,
	    o->ph[i].vaddr, o->ph[i].paddr,
	    o->ph[i].fsize, o->ph[i].msize,
	    o->ph[i].align
	    );
#endif			   
  }

  if (o->pdyn) { // Find Dynamic String table
    int i;
    int n_dn = o->pdyn->fsize / sizeof(Elf32_Dyn);
    Elf32_Dyn *dp = (Elf32_Dyn*) (o->p + o->pdyn->off);
    for (i = 0; i < n_dn; i++) {
      if (o->get_u32(dp[i].d_tag) == DT_STRTAB) {
	if (o->type == ET_DYN)
	  o->dstr = (char*)(o->p + o->get_u32(dp[i].d_un.d_ptr));
	else
	  o->dstr = (char*)(o->p + (o->get_u32(dp[i].d_un.d_ptr) - o->ph[0].vaddr +sizeof (Elf32_Ehdr)));
	break;
      }
    }
#ifdef DEBUG    
    printf ("Dynamic String Table at %0x\n",  o->get_u32(dp[i].d_un.d_ptr));
#endif
  }
  return 0;

}

int
maca_read_sections32 (MACA_OBJ *o) {
  Elf32_Shdr *s, *sstr;
  char       *str, *tmp;
  
  s = (Elf32_Shdr*)(o->p + o->s_off);
  
  sstr = &s[o->sstr];
  str = (char*)( o->p + o->get_u32(sstr->sh_offset));
  o->str = str;
  
  for (int i = 0; i < o->n_s; i++) {
    int   name        = o->get_u32 (s[i].sh_name);
    
    o->s[i].type      = o->get_u32(s[i].sh_type);
    o->s[i].flags     = o->get_u32(s[i].sh_flags);
    o->s[i].addr      = o->get_u32(s[i].sh_addr);
    o->s[i].off       = o->get_u32(s[i].sh_offset);
    o->s[i].size      = o->get_u32(s[i].sh_size);
    o->s[i].link      = o->get_u32(s[i].sh_link);
    o->s[i].info      = o->get_u32(s[i].sh_info);
    o->s[i].ent_size  = o->get_u32(s[i].sh_entsize);
    tmp = str+ name;
    o->s[i].name = tmp;
    
#ifdef DEBUG 
    printf ("[%02d] %20s %4x %8lx %6lx %6lx %02ld %lx %2d %2d\n",
	    i, str + name, o->s[i].type,
	    o->s[i].addr, o->s[i].off, o->s[i].size,
	    o->s[i].ent_size, o->s[i].flags, o->s[i].link, o->s[i].info);
    printf ("C : %f C++: %f RUST: %f GO : %f\n",
	  o->lang[LANG_C],
	  o->lang[LANG_CPP],
	  o->lang[LANG_RUST],
	  o->lang[LANG_GO]
	  );
#endif
  }
  
  return 0;
}


int
maca_read_dynamic_section32 (MACA_OBJ *o) {
  if (!o) return -1;
  if (o->pdyn == NULL) return -1;
  
  o->n_dn = o->pdyn->fsize / sizeof(Elf32_Dyn);
  o->dyn = malloc (sizeof(MACA_DYN)*o->n_dn);
  
  Elf32_Dyn *dp = (Elf32_Dyn*) (o->p + o->pdyn->off);
  for (int i = 0; i < o->n_dn; i++) {
    o->dyn[i].tag = o->get_u32(dp[i].d_tag);
    o->dyn[i].name = maca_util_elf_get_dtag(o, o->dyn[i].tag);
    o->dyn[i].un.val = o->get_u32(dp[i].d_un.d_val);
    // TODO:Parse Flags and DT_NEED so we do have the info available for analysis
  }
  return 0;
}


int
maca_read_sections64 (MACA_OBJ *o) {
  Elf64_Shdr *s, *sstr;
  char       *str, *tmp;
  
  s = (Elf64_Shdr*)(o->p + o->s_off);
  sstr = &s[o->sstr];
  str = (char*)( o->p + o->get_u64(sstr->sh_offset));
  o->str = str;
  for (int i = 0; i < o->n_s; i++) {
    int   name        = o->get_u32 (s[i].sh_name);
    o->s[i].type      = o->get_u32(s[i].sh_type);
    o->s[i].flags     = o->get_u64(s[i].sh_flags);
    o->s[i].addr      = o->get_u64(s[i].sh_addr);
    o->s[i].off       = o->get_u64(s[i].sh_offset);
    o->s[i].size      = o->get_u64(s[i].sh_size);
    o->s[i].link      = o->get_u32(s[i].sh_link);
    o->s[i].info      = o->get_u32(s[i].sh_info);
    o->s[i].ent_size  = o->get_u64(s[i].sh_entsize);
    tmp = str+ name;
    o->s[i].name = tmp;
#ifdef DEBUG 
    printf ("[%02d] %20s %4x %8lx %6lx %6lx %02ld %lx %2d %2d\n",
	    i, str + name, o->s[i].type,
	    o->s[i].addr, o->s[i].off, o->s[i].size,
	    o->s[i].ent_size, o->s[i].flags, o->s[i].link, o->s[i].info);
#endif
  }
#ifdef DEBUG  
  printf ("C : %f C++: %f RUST: %f GO : %f\n",
	  o->lang[LANG_C],
	  o->lang[LANG_CPP],
	  o->lang[LANG_RUST],
	  o->lang[LANG_GO]
	  );
#endif
  return 0;
}

int
maca_read_ph64 (MACA_OBJ *o) {
  Elf64_Phdr  *p;
  
#ifdef DEBUG  
  printf ("%d program headers at %lx\n", o->n_p, o->p_off);
#endif
  
  o->pdyn = NULL;
  p = (Elf64_Phdr*)(o->p + o->p_off);
  
  for (int i = 0; i < o->n_p; i++) {
    o->ph[i].type  = o->get_u32(p[i].p_type);
    o->ph[i].flags = o->get_u32(p[i].p_flags);
    o->ph[i].off   = o->get_u64(p[i].p_offset);
    o->ph[i].vaddr = o->get_u64(p[i].p_vaddr);
    o->ph[i].paddr = o->get_u64(p[i].p_paddr);
    o->ph[i].fsize = o->get_u64(p[i].p_filesz);
    o->ph[i].msize = o->get_u64(p[i].p_memsz);
    o->ph[i].align = o->get_u64(p[i].p_align);
    if (o->ph[i].type == PT_DYNAMIC) o->pdyn = &o->ph[i];
#if 0
    printf ("[%02d] %16s %lx %08lx %08lx %08lx %08lx %08lx\n",
	    i, maca_util_elf_get_ph_type(o->ph[i].type), o->ph[i].flags,
	    o->ph[i].vaddr, o->ph[i].paddr,
	    o->ph[i].fsize, o->ph[i].msize,
	    o->ph[i].align
	    );
#endif			   
  }

  if (o->pdyn) { // Find String table
    int i;
    int n_dn = o->pdyn->fsize / sizeof(Elf64_Dyn);
    Elf64_Dyn *dp = (Elf64_Dyn*) (o->p + o->pdyn->off);
    for (i = 0; i < n_dn; i++) {
      if (dp[i].d_tag == DT_STRTAB) {
	if (o->type == ET_DYN)
	  o->dstr = (char*)(o->p + o->get_u64(dp[i].d_un.d_ptr));
	else
	  o->dstr = (char*)(o->p + (o->get_u64(dp[i].d_un.d_ptr) - o->ph[0].vaddr +sizeof (Elf64_Ehdr)));
	break;
      }
    }
#ifdef DEBUG    
    printf ("Dynamic String Table at %0lx\n",  o->get_u64(dp[i].d_un.d_ptr));
#endif    
  }
  return 0;
}


int
maca_read_dynamic_section64 (MACA_OBJ *o) {
  if (!o) return -1;
  if (o->pdyn == NULL) return -1;
  
  o->n_dn = o->pdyn->fsize / sizeof(Elf64_Dyn);
  o->dyn = malloc (sizeof(MACA_DYN)*o->n_dn);
  
  Elf64_Dyn *dp = (Elf64_Dyn*) (o->p + o->pdyn->off);
  for (int i = 0; i < o->n_dn; i++) {
    o->dyn[i].tag = o->get_u64(dp[i].d_tag);
    o->dyn[i].name = maca_util_elf_get_dtag(o, o->dyn[i].tag);
    o->dyn[i].un.val = o->get_u64(dp[i].d_un.d_val);
    // TODO:Parse Flags and DT_NEED so we do not have the info already available
  }
  return 0;
}

