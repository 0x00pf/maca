#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <getopt.h>

#include "maca_obj.h"
#include "maca_util.h"
#include "maca_out.h"


int
maca_obj_dump (MACA_OBJ *o) {
  if (!o) return -1;
  if (o->status < MO_STATUS_MAPPED) return -1;
  
  MACA_CONF *cfg = maca_util_get_conf();

  maca_out_elf_info (o);
  
  maca_out_strings (o, cfg->strings_size, cfg->show_strings);

  maca_out_elf_segments (o);
  maca_out_elf_sections (o);
  maca_out_elf_dyn_section (o);
  maca_out_file_lang(o);

  return 0;
}

/* TO BE COMPLETED*/
static struct option long_options[] = {
  {"strings", optional_argument, NULL, 's'},
  {NULL, 0, NULL, 0}
};

int
main (int argc, char *argv[]) {
  MACA_OBJ  *o;
  MACA_CONF *cfg = maca_util_get_conf();

  while (1) {
    int index = 0;
    int c = getopt_long (argc, argv, "s:", long_options, &index);
    if (c == -1) break;
    if (c == 's') {
      cfg->show_strings = 1;
      if (optarg) cfg->strings_size = atoi (optarg);
      else cfg->strings_size = 4;
    }
  }
  if ((o = maca_obj_new(argv[optind])) == NULL) {
    exit (EXIT_FAILURE);
  }
  maca_obj_dump (o);
  maca_obj_free (o);
  
  return 0;
}
