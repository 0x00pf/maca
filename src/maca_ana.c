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
#include <string.h>
#include <ctype.h>

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
  printf ("C : %f C++: %f RUST: %f GO : %f\n", o->lang[LANG_C],
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
  return 0;
}
