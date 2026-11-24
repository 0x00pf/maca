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
#include <ctype.h>
#include <math.h>


#include <elf.h>

#include "maca_obj.h"

int
maca_ana_sections (MACA_OBJ *o) {
  char *p;
  
  if (!o) return -1;
  if (!o->s) return -1;
  
  printf ("+ Analysing %d sections...\n", o->n_s);
  for (int i = 0; i < o->n_s; i++) {
    p = o->s[i].name;
    if (!strcmp (p, ".gosymtab")) o->lang[LANG_GO] += 1;
    else if (!strcmp (p, ".gopclntab")) o->lang[LANG_GO] += 1;
    else if (!strcmp (p, ".go.buildinfo")) o->lang[LANG_GO] += 1;
    else if (!strcmp (p, ".note.go.buildid")) o->lang[LANG_GO] += 1;
    else if (!strcmp (p, ".go.buildinfo")) o->lang[LANG_GO] += 1;
    else if (!strcmp (p, ".tdata")) o->lang[LANG_RUST] += 1;
    else if (!strcmp (p, ".tbss")) o->lang[LANG_RUST] += 1;   
  }
  
#ifdef DEBUG
  printf ("SECTIONS\nC : %f C++: %f RUST: %f GO : %f\n", o->lang[LANG_C],
	  o->lang[LANG_CPP],
	  o->lang[LANG_RUST],
	  o->lang[LANG_GO]
	  );
#endif    
  return 0; 
}

int
maca_ana_dyn_section (MACA_OBJ *o) {
  if (!o) return -1;
  if (!o->s) return -1;
  if (!o->pdyn) return -1;
  for (int i = 0; i < o->n_dn; i++) {
    if (o->dyn[i].tag == DT_NEEDED) {
      if (strstr (o->dstr + o->dyn[i].un.val, "libstdc++") != NULL) o->lang[LANG_CPP]+=1;
      if (strstr (o->dstr + o->dyn[i].un.val, "libc") != NULL) {
	o->lang[LANG_C]+=1;
	o->lang[LANG_RUST]+=1;
	o->lang[LANG_CPP]+=1;
      }
    }
  }
#ifdef DEBUG
  printf ("DYNAMIC SECTION\nC : %f C++: %f RUST: %f GO : %f\n", o->lang[LANG_C],
	  o->lang[LANG_CPP],
	  o->lang[LANG_RUST],
	  o->lang[LANG_GO]
	  );
#endif    

  return 0;
}

int
maca_ana_entropy (MACA_OBJ *o) {
  double  h1[256];   // Normal histogram
  double  h2[256];   // Overall histogram
  long    t1, t2;
  
  for (int i = 0; i < 256; i++) h1[i] = h2[i] = 0;
  t1 = t2 = 0;
  
  // Calculate entropy per section
  for (int i = 0; i < o->n_s; i++) {
    unsigned char *p = o->p + o->s[i].off;
    t1 = 0;
    for (int j = 0; j < o->s[i].size; j++, t1++) h1[p[j]]++;
    if (t1) {
      for (int j = 0; j < 256; j++) h1[j] = h1[j] / (double)t1;
      // Calculate entropy
      double sum = 0;
      for (int j = 0; j < 256; j++) {
	if (h1[j]) sum += h1[j] * log2 (h1[j]);
      }
      o->s[i].ent = -sum;
    } else o->s[i].ent = 0;
  }
  
  // Calculate entropy per PHDR
  for (int i = 0; i < o->n_p; i++) {
    unsigned char *p = o->p + o->ph[i].off;
    t1 = 0;
    for (int j = 0; j < o->ph[i].fsize; j++, t1++) h1[p[j]]++;
    if (t1) {
      for (int j = 0; j < 256; j++) h1[j] = h1[j] / (double)t1;
      // Calculate entropy
      double sum = 0;
      for (int j = 0; j < 256; j++) {
	if (h1[j]) sum += h1[j] * log2 (h1[j]);
      }
      o->ph[i].ent = -sum;
    } else o->ph[i].ent = 0;
  }
  // XXX: Sliding window Entropy calculation?
  return 0;
}


int
maca_ana_code_caves (MACA_OBJ *o) {
  int64_t   size = 0;
  uint64_t  addr = 0;
  MACA_CC   *p  = o->cc;

  for (int i = 1; i < o->n_p; i++) {
    if (o->ph[i].type == PT_LOAD && o->ph[i].vaddr != 0) {
      addr = o->ph[i - 1].vaddr  + o->ph[i - 1].fsize;
      size = o->ph[i].vaddr - addr;
    
      if (size > 0) {
#if DEBUG	
	printf ("Found Cave at %lx. Size %lx, Flag %lx\n",
		addr, size, o->ph[i - 1].flags);
#endif
	if ((p = realloc (o->cc, (o->n_cc + 1) * sizeof (MACA_CC))) == NULL) {
	  fprintf (stderr, "Cannot reallocate memory.\n");
	  return -1;
	}
	o->cc = p;
	o->cc[o->n_cc].addr = addr;
	o->cc[o->n_cc].size = size;
	o->cc[o->n_cc].flags = o->ph[i-1].flags;
	o->n_cc++;
      }
    }
    
  }
  return 0;
}
