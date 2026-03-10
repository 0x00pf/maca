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
#include <sys/stat.h>

#include <elf.h>
#include "maca_obj.h"
#include "maca_util.h"

static MACA_CONF maca_conf;

char *type[N_TYPE] = {
  "Unknown",
  "ET_REL : Relocatable file",
  "ET_EXEC: Executable file",
  "ET_DYN : Shared object",
  "ET_CORE: Core file"
};

char *osabi[N_OSABI] = {
  "System V",
  "HP-UX",
  "NetBSD",
  "Linux",
  "GNU Hurd",
  "Solaris",
  "AIX (Monterey)",
  "IRIX",
  "FreeBSD",
  "Tru64",
  "Novell Modesto",
  "OpenBSD",
  "OpenVMS",
  "NonStop Kernel",
  "AROS",
  "FenixOS",
  "Nuxi CloudABI",
  "Stratus Technologies OpenVOS",
  NULL
};

char *isa[N_ISA] = {
  "No specific instruction set",
  "AT&T WE 32100",
  "SPARC",
  "x86",
  "Motorola 68000 (M68k)",
  "Motorola 88000 (M88k)",
  "Intel MCU",
  "Intel 80860",
  "MIPS",
  "IBM System/370",
  "MIPS RS3000 Little-endian",
  "Reserved for future use",
  "Reserved for future use",
  "Reserved for future use",
  "Reserved for future use",  
  "Hewlett-Packard PA-RISC",
  "Reserved",
  "Fujitsu VPP500",
  "Sun's \"v8plus\"",
  "Intel 80960",
  "PowerPC",
  "PowerPC (64-bit)",
  "S390, including S390x",
  "IBM SPU/SPC",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "NEC V800",
  "Fujitsu FR20",
  "TRW RH-32",
  "Motorola RCE",
  "Arm (up to Armv7/AArch32)",
  "Digital Alpha",
  "SuperH",
  "SPARC Version 9",
  "Siemens TriCore embedded processor",
  "Argonaut RISC Core",
  "Hitachi H8/300",
  "Hitachi H8/300H",
  "Hitachi H8S",
  "Hitachi H8/500",
  "IA-64",
  "Stanford MIPS-X",
  "Motorola ColdFire",
  "Motorola M68HC12",
  "Fujitsu MMA Multimedia Accelerator",
  "Siemens PCP",
  "Sony nCPU embedded RISC processor", 
  "Denso NDR1 microprocessor",
  "Motorola Star*Core processor",
  "Toyota ME16 processor",
  "STMicroelectronics ST100 processor",
  "Advanced Logic Corp. TinyJ embedded processor family",
  "AMD x86-64",
  "Sony DSP Processor",
  "Digital Equipment Corp. PDP-10",
  "Digital Equipment Corp. PDP-11",
  "Siemens FX66 microcontroller",
  "STMicroelectronics ST9+ 8/16-bit microcontroller",
  "STMicroelectronics ST7 8-bit microcontroller",
  "Motorola MC68HC16 Microcontroller",
  "Motorola MC68HC11 Microcontroller",
  "Motorola MC68HC08 Microcontroller",
  "Motorola MC68HC05 Microcontroller",
  "Silicon Graphics SVx",
  "STMicroelectronics ST19 8-bit microcontroller",
  "Digital VAX",
  "Axis Communications 32-bit embedded processor",
  "Infineon Technologies 32-bit embedded processor",
  "Element 14 64-bit DSP Processor",
  "LSI Logic 16-bit DSP Processor",
  "Donald Knuth's educational 64-bit proc",
  "Harvard University machine-independent object files",
  "SiTera Prism",
  "Atmel AVR 8-bit microcontroller",
  "Fujitsu FR30",
  "Mitsubishi D10V",
  "Mitsubishi D30V",
  "NEC v850",
  "Mitsubishi M32R",
  "Matsushita MN10300",
  "Matsushita MN10200",
  "picoJava",
  "OpenRISC 32-bit embedded processor",
  "ARC International ARCompact",
  "Tensilica Xtensa Architecture",
  "Alphamosaic VideoCore",
  "Thompson Multimedia General Purpose Proc",
  "National Semi. 32000",
  "Tenor Network TPC",
  "Trebia SNP 1000",
  "STMicroelectronics ST200",
  "Ubicom IP2xxx",
  "MAX processor",
  "National Semi. CompactRISC",
  "Fujitsu F2MC16",
  "Texas Instruments msp430",
  "Analog Devices Blackfin DSP",
  "Seiko Epson S1C33 family",
  "Sharp embedded microprocessor",
  "Arca RISC",
  "PKU-Unity & MPRC Peking Uni. mc series",
  "eXcess configurable cpu",
  "Icera Semi. Deep Execution Processor",
  "Altera Nios II",
  "National Semi. CompactRISC CRX",
  "Motorola XGATE",
  "Infineon C16x/XC16x",
  "Renesas M16C",
  "Microchip Technology dsPIC30F",
  "Freescale Communication Engine RISC",
  "Renesas M32C",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Altium TSK3000",
  "Freescale RS08",
  "Analog Devices SHARC family",
  "Cyan Technology eCOG2",
  "Sunplus S+core7 RISC",
  "New Japan Radio (NJR) 24-bit DSP",
  "Broadcom VideoCore III",
  "RISC for Lattice FPGA",
  "Seiko Epson C17",
  "Texas Instruments TMS320C6000 DSP",
  "#define EM_TI_C2000	141	/* Texas Instruments TMS320C2000 DSP",
  "#define EM_TI_C5500	142	/* Texas Instruments TMS320C55x DSP",
  "#define EM_TI_ARP32	143	/* Texas Instruments App. Specific RISC",
  "#define EM_TI_PRU	144	/* Texas Instruments Prog. Realtime Unit",
  /* reserved 145-159 */
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "STMicroelectronics 64bit VLIW DSP",
  "Cypress M8C",
  "Renesas R32C",
  "NXP Semi. TriMedia",
  "QUALCOMM DSP6",
  "Intel 8051 and variants",
  "STMicroelectronics STxP7x",
  "Andes Tech. compact code emb. RISC",
  "Cyan Technology eCOG1X",
  "Dallas Semi. MAXQ30 mc",
  "New Japan Radio (NJR) 16-bit DSP",
  "M2000 Reconfigurable RISC",
  "Cray NV2 vector architecture",
  "Renesas RX",
  "Imagination Tech. META",
  "MCST Elbrus",
  "Cyan Technology eCOG16",
  "National Semi. CompactRISC CR16",
  "Freescale Extended Time Processing Unit",
  "Infineon Tech. SLE9X",
  "Intel L10M",
  "Intel K10M",
  "Reserved",
  "ARM AARCH64",
  "Reserved",
  "Amtel 32-bit microprocessor",
  "STMicroelectronics STM8",
  "Tilera TILE64",
  "Tilera TILEPro",
  "Xilinx MicroBlaze",
  "NVIDIA CUDA",
  "Tilera TILE-Gx",
  "CloudShield",
  "KIPO-KAIST Core-A 1st gen.",
  "KIPO-KAIST Core-A 2nd gen.",
  "Synopsys ARCv2 ISA.",
  "Open8 RISC",
  "Renesas RL78",
  "Broadcom VideoCore V",
  "Renesas 78KOR",
  "Freescale 56800EX DSC",
  "Beyond BA1",
  "Beyond BA2",
  "XMOS xCORE",
  "Microchip 8-bit PIC(r)",
  "Intel Graphics Technology",
  /* reserved 206-209 */
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "KM211 KM32",
  "KM211 KMX32",
  "KM211 KMX16",
  "KM211 KMX8",
  "KM211 KVARC",
  "Paneve CDP",
  "Cognitive Smart Memory Processor",
  "Bluechip CoolEngine",
  "Nanoradio Optimized RISC",
  "CSR Kalimba",
  "Zilog Z80",
  "Controls and Data Services VISIUMcore",
  "FTDI Chip FT32",
  "Moxie processor",
  "AMD GPU",
  /* reserved 225-242 */
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "RISC-V",
  "Reserved",
  "Reserved",
  "Reserved",
  "Linux BPF -- in-kernel virtual machine",
  "C-SKY",
  "LoongArch"
};

int
maca_util_elf_get_s_flags (char s[32], uint64_t f) {
  int i = 0;
  
  memset (s, 0, 32);
  if (f & SHF_WRITE) s[i++] = 'W';
  if (f & SHF_ALLOC) s[i++] = 'A';
  if (f & SHF_EXECINSTR) s[i++] = 'X';
  if (f & SHF_MERGE) s[i++] = 'M';
  if (f & SHF_STRINGS) s[i] = 'S';
  if (f & SHF_INFO_LINK) s[i] = 'I';
  if (f & SHF_LINK_ORDER) s[i] = 'O';
  if (f & SHF_GROUP) s[i] = 'G';
  if (f & SHF_TLS) s[i] = 'T';
  if (f & SHF_COMPRESSED) s[i] = 'C';
  if (f & SHF_GNU_RETAIN) s[i] = 'R';
  if (f & SHF_ORDERED) s[i] = 'S';
  if (f & SHF_EXCLUDE) s[i] = 'E';
  
  return 0;
}

char *
maca_util_elf_get_ph_type (uint64_t type) {
  switch (type) {
  case PT_NULL:         return "NULL";
  case PT_LOAD:         return "LOAD";
  case PT_DYNAMIC:      return "DYNAMIC";
  case PT_INTERP:       return "INTERP";
  case PT_NOTE:         return "NOTE";
  case PT_SHLIB:        return "SHLIB";
  case PT_PHDR:         return "PHDR";
  case PT_TLS:          return "TLS";
  case PT_GNU_EH_FRAME: return "GNU_EH_FRAME";
  case PT_GNU_STACK:    return "GNU_STACK";
  case PT_GNU_RELRO:    return "GNU_RELO";
  case PT_GNU_PROPERTY: return "GNU_PROPERTY";
  case PT_GNU_SFRAME:   return "GNU_SFRAME";
  case PT_SUNWBSS:      return "SUN_BSS"; /* Sun Specific segment */
  case PT_SUNWSTACK:    return "SUN_STACK";
  case PT_MIPS_ABIFLAGS:return "ABIFLAGS";
  case PT_HISUNW:
  case PT_LOPROC:
  case PT_HIPROC:
  default:
    return "UNKNOWN";
  }
}

char *
maca_util_elf_get_dtag (MACA_OBJ *o, int i) {
  switch (i) {
  case DT_NULL:            return "NULL";
  case DT_NEEDED :         return "NEEDED";          /* Name of needed library */
  case DT_PLTRELSZ:        return "PLTRELSZ";        /* Size in bytes of PLT relocs */
  case DT_PLTGOT:          return "PLTGOT";          /* Processor defined value */
  case DT_HASH:            return "HASH";            /* Address of symbol hash table */
  case DT_STRTAB:          return "STRTAB";          /* Address of string table */
  case DT_SYMTAB:          return "SYMTAB";          /* Address of symbol table */
  case DT_RELA:            return "RELA";            /* Address of Rela relocs */
  case DT_RELASZ:          return "RELASZ";          /* Total size of Rela relocs */
  case DT_RELAENT:         return "RELAENT";         /* Size of one Rela reloc */
  case DT_STRSZ:           return "STRSZ";           /* Size of string table */
  case DT_SYMENT:          return "SYMENT";          /* Size of one symbol table entry */
  case DT_INIT:            return "INIT";            /* Address of init function */
  case DT_FINI:            return "FINI";            /* Address of termination function */
  case DT_SONAME:          return "SONAME";          /* Name of shared object */
  case DT_RPATH:           return "RPATH";           /* Library search path (deprecated) */
  case DT_SYMBOLIC:        return "SYMBOLIC";        /* Start symbol search here */
  case DT_REL:             return  "REL";            /* Address of Rel relocs */
  case DT_RELSZ:           return "RELSZ";           /* Total size of Rel relocs */
  case DT_RELENT:          return "RELENT";          /* Size of one Rel reloc */
  case DT_PLTREL:          return "PLTREL";          /* Type of reloc in PLT */
  case DT_DEBUG:           return "DEBUG";           /* For debugging; unspecified */
  case DT_TEXTREL:         return "TEXTREL";         /* Reloc might modify .text */
  case DT_JMPREL:          return "JMPREL";          /* Address of PLT relocs */
  case DT_BIND_NOW:        return "BIND_NOW";        /* Process relocations of object */
  case DT_INIT_ARRAY:      return "INIT_ARRAY";      /* Array with addresses of init fct */
  case DT_FINI_ARRAY:      return "FINI_ARRAY";      /* Array with addresses of fini fct */
  case DT_INIT_ARRAYSZ:    return "INIT_ARRAYSZ";    /* Size in bytes of DT_INIT_ARRAY */
  case DT_FINI_ARRAYSZ:    return "FINI_ARRAYSZ";    /* Size in bytes of DT_FINI_ARRAY */
  case DT_RUNPATH:         return "RUNPATH";         /* Library search path */
  case DT_FLAGS:           return "FLAGS";           /* Flags for the object being loaded */
  case DT_PREINIT_ARRAY:   return "PREINIT_ARRAY";   /* Array with addresses of preinit fct*/
  case DT_PREINIT_ARRAYSZ: return "PREINIT_ARRAYSZ"; /* size in bytes of DT_PREINIT_ARRAY */
  case DT_SYMTAB_SHNDX:    return "DT_SYMTAB_SHNDX"; /* Address of SYMTAB_SHNDX section */
  case DT_RELRSZ:          return "RELRSZ";          /* Total size of RELR relative relocations */
  case DT_RELR:            return "RELR";            /* Address of RELR relative relocations */
  case DT_RELRENT:         return "RELRENT";         /* Size of one RELR relative relocaction */
  case DT_VALRNGLO:        return "VALRNGLO";
  case DT_GNU_PRELINKED:   return "GNU_PRELINKED";   /* Prelinking timestamp */
  case DT_GNU_CONFLICTSZ:  return "GNU_CONFLICTSZ";  /* Size of conflict section */
  case DT_GNU_LIBLISTSZ:   return "GNU_LIBLISTSZ";   /* Size of library list */
  case DT_CHECKSUM:        return "CHECKSUM";
  case DT_PLTPADSZ:        return "PLTPADSZ";
  case DT_MOVEENT:         return "MOVEENT";      
  case DT_MOVESZ:          return "MOVESZ";
  case DT_FEATURE_1:       return "FEATURE_1";       /* Feature selection (DTF_*).  */
  case DT_POSFLAG_1:       return "POSFLAG_1";       /* Flags for DT_* entries, effecting  the following DT_* entry.  */
  case DT_SYMINSZ:         return "SYMINSZ";         /* Size of syminfo table (in bytes) */
  case DT_SYMINENT:        return "SYMINENT";        /* Entry size of syminfo */
  case DT_ADDRRNGLO:       return "ADDRRNGLO";
  case DT_GNU_HASH:        return "GNU_HASH";        /* GNU-style hash table.  */
  case DT_TLSDESC_PLT:     return "TLSDESC_PLT";
  case DT_TLSDESC_GOT:     return "TLSDESC_GOT";
  case DT_GNU_CONFLICT:    return "GNU_CONFLICT";    /* Start of conflict section */
  case DT_GNU_LIBLIST:     return "GNU_LIBLIST";     /* Library list */
  case DT_CONFIG:          return  "CONFIG";         /* Configuration information.  */
  case DT_DEPAUDIT:        return "DEPAUDIT";        /* Dependency auditing.  */
  case DT_AUDIT:           return "AUDIT";           /* Object auditing.  */
  case DT_PLTPAD:          return "PLTPAD";          /* PLT padding.  */
  case DT_MOVETAB:         return "MOVETAB";         /* Move table.  */
  case DT_SYMINFO:         return "SYMINFO";         /* Syminfo table.  */
  case DT_VERSYM:          return "VERSYM";
  case DT_RELACOUNT:       return  "RELACOUNT";
  case DT_RELCOUNT:        return "RELCOUNT";
/* These were chosen by Sun.  */
  case DT_FLAGS_1:         return "FLAGS_1";         /* State flags, see DF_1_* below.  */
  case DT_VERDEF:          return "VERDEF";          /* Address of version definition table */
  case DT_VERDEFNUM:       return "VERDEFNUM";       /* Number of version definitions */
  case DT_VERNEED:         return "VERNEED";         /* Address of table with needed versions */
  case DT_VERNEEDNUM:      return "VERNEEDNUM";      /* Number of needed versions */
/* Sun added these machine-independent extensions in the "processor-specific"
   range.  Be compatible.  */
  case DT_AUXILIARY:        return "AUXILIARY";      /* Shared object to load before self */
  case DT_FILTER:           return "FILTER";         /* Shared object to get values from */
  }
  if (o->machine == EM_MIPS)
  switch (i) {
    // MIPS // THis is processor specific...
  case DT_MIPS_RLD_VERSION:       return "MIPS_RLD_VERSION"; /* Runtime linker interface version */
  case DT_MIPS_TIME_STAMP:        return "MIPS_TIME_STAMP"; /* Timestamp */
  case DT_MIPS_ICHECKSUM:         return "MIPS_ICHECKSUM";    /* Checksum */
  case DT_MIPS_IVERSION:          return "MIPS_IVERSION"; /* Version string (string tbl index) */
  case DT_MIPS_FLAGS:             return "MIPS_FLAGS";    /* Flags */
  case DT_MIPS_BASE_ADDRESS:      return "MIPS_BASE_ADDRESS"; /* Base address */
  case DT_MIPS_MSYM:              return "MIPS_MSYM";
  case DT_MIPS_CONFLICT:          return "MIPS_CONFLICT"; /* Address of CONFLICT section */
  case DT_MIPS_LIBLIST:           return "MIPS_LIBLIST";   /* Address of LIBLIST section */
  case DT_MIPS_LOCAL_GOTNO:       return "MIPS_LOCAL_GOTNO"; /* Number of local GOT entries */
  case DT_MIPS_CONFLICTNO:        return "MIPS_CONFLICTNO"; /* Number of CONFLICT entries */
  case DT_MIPS_LIBLISTNO:         return "MIPS_LIBLISTNO"; /* Number of LIBLIST entries */
  case DT_MIPS_SYMTABNO:          return "MIPS_SYMTABNO"; /* Number of DYNSYM entries */
  case DT_MIPS_UNREFEXTNO:        return "MIPS_UNREFEXTNO";  /* First external DYNSYM */
  case DT_MIPS_GOTSYM:            return "MIPS_GOTSYM"; /* First GOT entry in DYNSYM */
  case DT_MIPS_HIPAGENO:          return "MIPS_HIPAGENO"; /* Number of GOT page table entries */
  case DT_MIPS_RLD_MAP:           return "MIPS_RLD_MAP";  /* Address of run time loader map.  */
  case DT_MIPS_DELTA_CLASS:       return "MIPS_DELTA_CLASS"; /* Delta C++ class definition.  */
  case DT_MIPS_DELTA_CLASS_NO:    return "MIPS_DELTA_CLASS_NO"; /* Number of entries in DT_MIPS_DELTA_CLASS.  */
  case DT_MIPS_DELTA_INSTANCE:    return "MIPS_DELTA_INSTANCE"; /* Delta C++ class instances.  */
  case DT_MIPS_DELTA_INSTANCE_NO: return "MIPS_DELTA_INSTANCE_NO"; /* Number of entries in DT_MIPS_DELTA_INSTANCE.  */
  case DT_MIPS_DELTA_RELOC:       return "MIPS_DELTA_RELOC"; /* Delta relocations.  */
  case DT_MIPS_DELTA_RELOC_NO:    return "MIPS_DELTA_RELOC_NO"; /* Number of entries in DT_MIPS_DELTA_RELOC.  */
  case DT_MIPS_DELTA_SYM:         return "MIPS_DELTA_SYM"; /* Delta symbols that Delta relocations refer to.  */
  case DT_MIPS_DELTA_SYM_NO:      return "MIPS_DELTA_SYM_NO"; /* Number of entries in DT_MIPS_DELTA_SYM.  */
  case DT_MIPS_DELTA_CLASSSYM:    return "MIPS_DELTA_CLASSSYM"; /* Delta symbols that hold the class declaration.  */
  case DT_MIPS_DELTA_CLASSSYM_NO: return "MIPS_DELTA_CLASSSYM_NO"; /* Number of entries in
									 DT_MIPS_DELTA_CLASSSYM.  */
  case  DT_MIPS_CXX_FLAGS:        return "MIPS_CXX_FLAGS"; /* Flags indicating for C++ flavor.  */
  case DT_MIPS_PIXIE_INIT:        return "MIPS_PIXIE_INIT";
  case DT_MIPS_SYMBOL_LIB:        return "MIPS_SYMBOL_LIB";
  case DT_MIPS_LOCALPAGE_GOTIDX:  return "MIPS_LOCALPAGE_GOTIDX";
  case DT_MIPS_LOCAL_GOTIDX:      return "MIPS_LOCAL_GOTIDX";
  case DT_MIPS_HIDDEN_GOTIDX:     return "MIPS_HIDDEN_GOTIDX";
  case DT_MIPS_PROTECTED_GOTIDX:  return "MIPS_PROTECTED_GOTIDX";
  case DT_MIPS_OPTIONS:           return "MIPS_OPTIONS"; /* Address of .options.  */
  case DT_MIPS_INTERFACE:         return "MIPS_INTERFACE"; /* Address of .interface.  */
  case DT_MIPS_DYNSTR_ALIGN:      return "MIPS_DYNSTR_ALIGN";
  case DT_MIPS_INTERFACE_SIZE:    return "MIPS_INTERFACE_SIZE"; /* Size of the .interface section. */
  case DT_MIPS_RLD_TEXT_RESOLVE_ADDR: return "MIPS_RLD_TEXT_RESOLVE_ADDR"; /* Address of rld_text_rsolve function stored in GOT.  */
  case DT_MIPS_PERF_SUFFIX:       return "MIPS_PERF_SUFFIX"; /* Default suffix of dso to be added by rld on dlopen() calls.  */
  case DT_MIPS_COMPACT_SIZE:      return "MIPS_COMPACT_SIZE"; /* (O32)Size of compact rel section. */
  case DT_MIPS_GP_VALUE:          return "MIPS_GP_VALUE";      /* GP value for aux GOTs.  */
  case DT_MIPS_AUX_DYNAMIC:       return "MIPS_AUX_DYNAMIC"; /* Address of aux .dynamic.  */
  case DT_MIPS_PLTGOT:            return "_MIPS_PLTGOT";
  case DT_MIPS_RWPLT:             return "_MIPS_RWPLT";
  case DT_MIPS_RLD_MAP_REL:       return "MIPS_RLD_MAP_REL";
  case DT_MIPS_XHASH:             return "MIPS_XHASH";   
  }
  return NULL;
}


char *
maca_util_elf_get_flag (uint64_t i, char str[1024]) {
  char *p = str;
  
  if (i & DF_ORIGIN)     p += sprintf (p, "ORIGIN" );  
  if (i & DF_SYMBOLIC)   p += sprintf (p, "SYMBOLIC");
  if (i & DF_TEXTREL)    p += sprintf (p, "TEXTREL" );  
  if (i & DF_BIND_NOW)   p += sprintf (p, FG_LYELLOW "BIND_NOW" RESET);
  if (i & DF_STATIC_TLS) p += sprintf (p, "STATIC_TLS");
  
  return NULL;
}


char *
maca_util_elf_ph_perm (uint64_t i, char str[128]) {
  char *p = str;

  strcpy (p, "   ");
  if (i & PF_X) p[2] = 'E';  
  if (i & PF_W) p[1] = 'W';
  if (i & PF_R) p[0] = 'R';
  
  return NULL;
}


char *
maca_util_elf_get_flag1 (uint64_t i, char str[1024]) {
  char *p = str;
  
  if (i & DF_1_NOW)        p += sprintf (str, FG_LYELLOW "NOW" RESET);     /* Set RTLD_NOW for this object.  */
  if (i & DF_1_GLOBAL)     p += sprintf (str, FG_LYELLOW "GLOBAL" RESET);  /* Set RTLD_GLOBAL for this object.  */
  if (i & DF_1_GROUP)      p += sprintf (str, FG_LYELLOW "GROUP" RESET);   /* Set RTLD_GROUP for this object.  */
  if (i & DF_1_NODELETE)   p += sprintf (str, FG_LYELLOW "NODELETE" RESET);/* Set RTLD_NODELETE for this object.*/
  if (i & DF_1_LOADFLTR)   p += sprintf (str, "LOADFLTR");                 /* Trigger filtee loading at runtime.*/
  if (i & DF_1_INITFIRST)  p += sprintf (str, FG_LYELLOW "INITFIRST" RESET);  /* Set RTLD_INITFIRST for this object*/
  if (i & DF_1_NOOPEN)     p += sprintf (str, FG_LYELLOW "NOOPEN" RESET);   /* Set RTLD_NOOPEN for this object.  */
  if (i & DF_1_ORIGIN)     p += sprintf (str, "ORIGIN");                    /* $ORIGIN must be handled.  */
  if (i & DF_1_DIRECT)     p += sprintf (str, "DIRECT");                    /* Direct binding enabled.  */
  if (i & DF_1_TRANS)      p += sprintf (str, "TRANS");
  if (i & DF_1_INTERPOSE)  p += sprintf (str, "INTERPOSE");                 /* Object is used to interpose.  */
  if (i & DF_1_NODEFLIB)   p += sprintf (str, "NODEFLIB");                  /* Ignore default lib search path.  */
  if (i & DF_1_NODUMP)     p += sprintf (str, "NODUMP");                    /* Object can't be dldump'ed.  */
  if (i & DF_1_CONFALT)    p += sprintf (str, "CONFALT");                   /* Configuration alternative created.*/
  if (i & DF_1_ENDFILTEE)  p += sprintf (str, "ENDFILTEE");                 /* Filtee terminates filters search. */
  if (i & DF_1_DISPRELDNE) p += sprintf (str, "DISPRELDNE");                /* Disp reloc applied at build time. */
  if (i & DF_1_DISPRELPND) p += sprintf (str, "DISPRELPND");                /* Disp reloc applied at run-time.  */
  if (i & DF_1_NODIRECT)   p += sprintf (str, "NODIRECT");                  /* Object has no-direct binding. */
  if (i & DF_1_IGNMULDEF)  p += sprintf (str, "IGNMULDEF");
  if (i & DF_1_NOKSYMS)    p += sprintf (str, FG_CYAN "NOKSYMS" RESET); 
  if (i & DF_1_NOHDR)      p += sprintf (str, "NOHDR");
  if (i & DF_1_EDITED)     p += sprintf (str, "EDITED");                    /* Object is modified after built.  */
  if (i & DF_1_NORELOC)    p += sprintf (str, "NORELOC");  
  if (i & DF_1_SYMINTPOSE) p += sprintf (str, "SYMINTPOSE");                /* Object has individual interposers.  */
  if (i & DF_1_GLOBAUDIT)  p += sprintf (str, "GLOBAUDIT");                 /* Global auditing required.  */
  if (i & DF_1_SINGLETON)  p += sprintf (str, "SINGLETON");                 /* Singleton symbols are used.  */
  if (i & DF_1_STUB)       p += sprintf (str, "STUB");  
  if (i & DF_1_PIE)        p += sprintf (str, FG_LGREEN "PIE" RESET);
  if (i & DF_1_KMOD)       p += sprintf (str, FG_CYAN "KMOD" RESET);
  if (i & DF_1_WEAKFILTER) p += sprintf (str, "WEAKFILTER"); 
  if (i & DF_1_NOCOMMON)   p += sprintf (str, "NOCOMMON");
  
  return "Unknown";
}

char *
maca_util_elf_get_isa (MACA_OBJ *obj) {
  if (!obj) return NULL;
  if (obj->isa > N_ISA) return "Unknown";
  
  else return isa[obj->isa];
}
 
int
maca_util_get_file_size (int fd)
{
  struct stat _info;

  fstat (fd, &_info);
  
  return _info.st_size;
}

// Instead of adding setter/getter, the struct is only written from main
// All other functions only read
MACA_CONF*
maca_util_get_conf () {
  return &maca_conf;
}
