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

  if (cfg->show_header) maca_out_elf_info (o);

  maca_out_strings (o, cfg->strings_size, cfg->show_strings);

  if (cfg->show_segments) maca_out_elf_segments (o);
  if (cfg->show_sections) maca_out_elf_sections (o);
  if (cfg->show_dynamic) maca_out_elf_dyn_section (o);
  if (cfg->show_lang) maca_out_file_lang(o);

  return 0;
}

void usage (void) {
  printf ("Usage: maca <options> elf-file\n");
  printf ("Options:\n");
  printf (" -a --all               Equivalent to -h -l -S -s -d -L\n");
  printf (" -h --help              Shows this help\n");
  printf (" -s --strings=[SIZE]    Finds strings with size >= than SIZE\n");
  printf (" -S --sections          Displays Section headers\n");
  printf (" -l --segments          Displays Program headers\n");
  printf (" -d --dynamic           Displays Dynamic Section\n");
  printf (" -L --languages         Displays Source Programmng Language guess\n");
}

/* TO BE COMPLETED*/
static struct option long_options[] = {
  {"all",       no_argument, NULL, 'a'},
  {"help",      no_argument, NULL, 'H'},
  {"strings",   optional_argument, NULL, 's'},
  {"header",    no_argument, NULL, 'h'},
  {"sections",  no_argument, NULL, 'S'},
  {"segments",  no_argument, NULL, 'l'},
  {"dynamic",   no_argument, NULL, 'd'},
  {"languages", no_argument, NULL, 'L'},
  {NULL, 0, NULL, 0}
};

int
main (int argc, char *argv[]) {
  MACA_OBJ  *o;
  MACA_CONF *cfg = maca_util_get_conf();

  memset (cfg, 0, sizeof(MACA_CONF));
  while (1) {
    int index = 0;
    int c = getopt_long (argc, argv, "aHhSlLds:", long_options, &index);
    if (c == -1) break;
    if (c == 'a') {
      cfg->strings_size = 4;
      cfg->show_strings = 1;
      cfg->show_header = 1;
      cfg->show_sections = 1;
      cfg->show_segments = 1;
      cfg->show_dynamic = 1;
      cfg->show_lang = 1;
      
    }
    if (c == 'S') cfg->show_sections = 1;
    if (c == 'l') cfg->show_segments = 1;
    if (c == 'd') cfg->show_dynamic = 1;
    if (c == 'L') cfg->show_lang = 1;
    if (c == 's') {
      cfg->show_strings = 1;
      if (optarg) cfg->strings_size = atoi (optarg);
      else cfg->strings_size = 4;
    }
    if (c == 'H') {
      usage ();
      exit (EXIT_SUCCESS);
    }
  }

  if (!cfg->show_header && !cfg->show_sections && !cfg->show_segments &&
      !cfg->show_dynamic && !cfg->show_lang && !cfg->show_strings) {
    usage ();
    exit (EXIT_SUCCESS);
  }
  
  if ((o = maca_obj_new(argv[optind])) == NULL) {
    exit (EXIT_FAILURE);
  }
  maca_obj_dump (o);
  maca_obj_free (o);
  
  return 0;
}
