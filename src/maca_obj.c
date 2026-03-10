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

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <fcntl.h>
#include <sys/mman.h> // mmap
#include <unistd.h>

#include <byteswap.h>

#include "maca_obj.h"
#include "maca_elf_parse.h"
#include "maca_ana.h"
#include "maca_util.h"


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static int host_endian = ELFDATA2LSB;
#else
static int host_endian = ELFDATA2MSB;
#endif

uint16_t get_u16_normal (uint16_t x) {return x;}
uint32_t get_u32_normal (uint32_t x) {return x;}
uint64_t get_u64_normal (uint64_t x) {return x;}

uint16_t get_u16_swap (uint16_t x) {return bswap_16(x);}
uint32_t get_u32_swap (uint32_t x) {return bswap_32(x);}
uint64_t get_u64_swap (uint64_t x) {return bswap_64(x);}


MACA_OBJ *
maca_obj_new (char *fname) {
  MACA_OBJ *tmp;
  int       fd;
  
  if ((tmp = malloc (sizeof(MACA_OBJ))) == NULL) {
    perror ("malloc:");
    return NULL;
  }
  
  memset (tmp, 0, sizeof(MACA_OBJ));
  tmp->fname = strndup (fname, MAX_STR_SIZE);
  
  /* Memory map file */
  if ((fd = open (fname, O_RDONLY)) < 0)
    {
      perror ("open");
      return NULL;
    }
  
  tmp->file_size = maca_util_get_file_size (fd);
  if ((tmp->p = mmap (NULL, tmp->file_size, PROT_READ,
		      MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
      perror ("mmap:");
      return NULL;exit (1);
    }

  tmp->mem_size = tmp->file_size;
  tmp->status   = MO_STATUS_MAPPED; // XXX: Probably we don't need this
  
  Elf64_Ehdr* elf_hdr = (Elf64_Ehdr *) tmp->p;
  if (elf_hdr->e_ident[0] != 0x7f ||
      elf_hdr->e_ident[1] != 0x45 ||
      elf_hdr->e_ident[2] != 0x4c ||
      elf_hdr->e_ident[3] != 0x46
      ) {
    fprintf (stderr, "Format unrecognized...\n");
    munmap (tmp->p, tmp->file_size);
    free (tmp);
    
    return NULL;
  }
  
  for (int i = 0; i < LANG_LAST; tmp->lang[i++] = 0.0);

  // Get common values
  tmp->class    = elf_hdr->e_ident[EI_CLASS];
  tmp->endian   = elf_hdr->e_ident[EI_DATA] == ELFDATA2MSB ? 0 : 1;
  tmp->osabi    = elf_hdr->e_ident[EI_OSABI];
  tmp->osabiver = elf_hdr->e_ident[EI_ABIVERSION];

  // Setup proper way to access data in the file if host and file endianness doesn't match
  if (host_endian != tmp->endian) {
    tmp->get_u16 = get_u16_swap;
    tmp->get_u32 = get_u32_swap;
    tmp->get_u64 = get_u64_swap;
  } else {
    tmp->get_u16 = get_u16_normal;
    tmp->get_u32 = get_u32_normal;
    tmp->get_u64 = get_u64_normal;
  }

  tmp->type    = tmp->get_u16(elf_hdr->e_type);
  tmp->machine = tmp->get_u16(elf_hdr->e_machine);
  tmp->isa     = tmp->machine;
  
  // Read Header info and allocates internal structures
  if (tmp->class == 1) { //32 bits
    Elf32_Ehdr* elf_hdr1 = (Elf32_Ehdr *) tmp->p;
    
    tmp->n_s       = tmp->get_u16(elf_hdr1->e_shnum);
    tmp->n_p       = tmp->get_u16(elf_hdr1->e_phnum);
    tmp->sent_size = tmp->get_u16(elf_hdr1->e_shentsize);
    tmp->pent_size = tmp->get_u16(elf_hdr1->e_phentsize);
    tmp->s_off     = tmp->get_u32(elf_hdr1->e_shoff);
    tmp->p_off     = tmp->get_u32(elf_hdr1->e_phoff);
    tmp->s         = malloc (tmp->n_s * sizeof (MACA_SECTION));
    tmp->ph        = malloc (tmp->n_p * sizeof (MACA_PH));
    tmp->sstr      = tmp->get_u16(elf_hdr1->e_shstrndx);
    tmp->entry     = tmp->get_u32(elf_hdr1->e_entry);
    tmp->hsize     = tmp->get_u16(elf_hdr1->e_ehsize);
    
    maca_read_sections32 (tmp);
    maca_read_ph32 (tmp);
    maca_read_dynamic_section32 (tmp);
    
  } else { // 64 bits
    tmp->n_s       = tmp->get_u16(elf_hdr->e_shnum);
    tmp->n_p       = tmp->get_u16(elf_hdr->e_phnum);
    tmp->sent_size = tmp->get_u16(elf_hdr->e_shentsize);
    tmp->pent_size = tmp->get_u16(elf_hdr->e_phentsize);
    tmp->s_off     = tmp->get_u64(elf_hdr->e_shoff);
    tmp->p_off     = tmp->get_u64(elf_hdr->e_phoff);
    tmp->s         = malloc (tmp->n_s * sizeof (MACA_SECTION));
    tmp->ph        = malloc (tmp->n_p * sizeof (MACA_PH));
    tmp->sstr      = tmp->get_u16(elf_hdr->e_shstrndx);
    tmp->entry     = tmp->get_u64(elf_hdr->e_entry);
    tmp->hsize     = tmp->get_u16(elf_hdr->e_ehsize);
    
    maca_read_sections64 (tmp);
    maca_read_ph64 (tmp);
    maca_read_dynamic_section64 (tmp);
    
  }

  // At this point everything is parsed. We can analyse things
  printf ("Analysing binary...\n");
  maca_ana_sections (tmp);
  maca_ana_dyn_section (tmp);
  
  return tmp;
}

void
maca_obj_free (MACA_OBJ *o) {
  if (!o) return;
  if (o->fname) free (o->fname);
  if (o->s) free (o->s);
  if (o->ph) free (o->ph);
  if (o->dyn) free (o->dyn);
  
  munmap (o->p, o->file_size);
  free(o);
  
  return;
}

