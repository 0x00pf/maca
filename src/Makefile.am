#  MACA
#  Copyright (c) 2026 pico
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

bin_PROGRAMS=maca

macaincludedir  = $(includedir)/maca
macainclude_HEADERS=maca_util.h maca_obj.h maca_elf_parse.h maca_ana.h maca_out.h
EXTRA_DIST=

maca_SOURCES=main.c maca_obj.c maca_util.c maca_elf_parse.c maca_ana.c maca_out.c
maca_LDADD=-lm
