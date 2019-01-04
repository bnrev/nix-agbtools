/* as.h - global header file
   Copyright (C) 1987-2018 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef GAS
#define GAS 1
/* I think this stuff is largely out of date.  xoxorich.

   CAPITALISED names are #defined.
   "lowercaseH" is #defined if "lowercase.h" has been #include-d.
   "lowercaseT" is a typedef of "lowercase" objects.
   "lowercaseP" is type "pointer to object of type 'lowercase'".
   "lowercaseS" is typedef struct ... lowercaseS.

 #define DEBUG to enable all the "know" assertion tests.
 #define SUSPECT when debugging hash code.
 #define COMMON as "extern" for all modules except one, where you #define
        COMMON as "".
   If TEST is #defined, then we are testing a module: #define COMMON as "".  */

/* Now, tend to the rest of the configuration.  */

/* System include files first...  */
#include "config.h"
#include <stdio.h>

#ifdef STRING_WITH_STRINGS
#include <string.h>
#include <strings.h>
#else
#include <string.h>
#endif
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
/* for size_t, pid_t */
#include <sys/types.h>
#endif

#include <errno.h>

#include <stdarg.h>

#include "getopt.h"
/* The first getopt value for machine-independent long options.
   150 isn't special; it's just an arbitrary non-ASCII char value.  */
#define OPTION_STD_BASE 150
/* The first getopt value for machine-dependent long options.
   290 gives the standard options room to grow.  */
#define OPTION_MD_BASE  290

#ifdef DEBUG
#undef NDEBUG
#endif
#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 6)
#define __PRETTY_FUNCTION__  ((char*)NULL)
#endif
#define gas_assert(P)   ((void)((P) ? 0 : (abort(), 0)))
#undef abort
#define abort()         as_abort(__FILE__, __LINE__, __PRETTY_FUNCTION__)

/* Now GNU header files...  */
#include "ansidecl.h"
#include "bfd.h"
#include "libiberty.h"

/* This is needed for VMS.  */
#if !defined (HAVE_UNLINK) && defined (HAVE_REMOVE)
#define unlink remove
#endif

#ifndef FOPEN_WB
#include "fopen-bin.h"
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

#include "asintl.h"

#define BAD_CASE(val)                                                       \
    {                                                                         \
        as_fatal(_("Case value %ld unexpected at line %d of file \"%s\"\n"),   \
                 (long)val, __LINE__, __FILE__);                              \
    }

#include "flonum.h"

/* These are assembler-wide concepts */

extern bfd *stdoutput;
typedef bfd_vma addressT;
typedef bfd_signed_vma offsetT;

/* Type of symbol value, etc.  For use in prototypes.  */
typedef addressT valueT;

#ifndef COMMON
#ifdef TEST
#define COMMON                  /* Declare our COMMONs storage here.  */
#else
#define COMMON extern           /* Our commons live elsewhere.  */
#endif
#endif
/* COMMON now defined */

#define know(p) do {} while (0) /* know() checks are no-op.ed  */

/* input_scrub.c */

/* Supplies sanitised buffers to read.c.
   Also understands printing line-number part of error messages.  */

/* subsegs.c     Sub-segments. Also, segment(=expression type)s.*/

typedef asection *segT;
#define SEG_NORMAL(SEG)         ((SEG) != absolute_section   \
                                 && (SEG) != undefined_section  \
                                 && (SEG) != reg_section        \
                                 && (SEG) != expr_section)
typedef int subsegT;

/* What subseg we are accessing now?  */
COMMON subsegT now_subseg;

/* Segment our instructions emit to.  */
COMMON segT now_seg;

#define segment_name(SEG)       bfd_get_section_name(stdoutput, SEG)

extern segT reg_section, expr_section;
/* Shouldn't these be eliminated someday?  */
extern segT text_section, data_section, bss_section;
#define absolute_section        bfd_abs_section_ptr
#define undefined_section       bfd_und_section_ptr

enum _relax_state {
    /* Dummy frag used by listing code.  */
    rs_dummy = 0,

    /* Variable chars to be repeated fr_offset times.
       Fr_symbol unused. Used with fr_offset == 0 for a
       constant length frag.  */
    rs_fill,

    /* Align.  The fr_offset field holds the power of 2 to which to
       align.  The fr_var field holds the number of characters in the
       fill pattern.  The fr_subtype field holds the maximum number of
       bytes to skip when aligning, or 0 if there is no maximum.  */
    rs_align,

    /* Align code.  The fr_offset field holds the power of 2 to which
       to align.  This type is only generated by machine specific
       code, which is normally responsible for handling the fill
       pattern.  The fr_subtype field holds the maximum number of
       bytes to skip when aligning, or 0 if there is no maximum.  */
    rs_align_code,

    /* Test for alignment.  Like rs_align, but used by several targets
       to warn if data is not properly aligned.  */
    rs_align_test,

    /* Org: Fr_offset, fr_symbol: address. 1 variable char: fill
       character.  */
    rs_org,

#ifndef WORKING_DOT_WORD
    /* JF: gunpoint */
    rs_broken_word,
#endif

    /* Machine specific relaxable (or similarly alterable) instruction.  */
    rs_machine_dependent,

    /* .space directive with expression operand that needs to be computed
       later.  Similar to rs_org, but different.
       fr_symbol: operand
       1 variable char: fill character  */
    rs_space,

    /* .nop directive with expression operand that needs to be computed
       later.  Similar to rs_space, but different.  It fills with no-op
       instructions.
       fr_symbol: operand
       1 constant byte: no-op fill control byte.  */
    rs_space_nop,

    /* Similar to rs_fill.  It is used to implement .nop directive .  */
    rs_fill_nop,

    /* A DWARF leb128 value; only ELF uses this.  The subtype is 0 for
       unsigned, 1 for signed.  */
    rs_leb128,

    /* Exception frame information which we may be able to optimize.  */
    rs_cfa,

    /* Cross-fragment dwarf2 line number optimization.  */
    rs_dwarf2dbg
};

typedef enum _relax_state relax_stateT;

/* This type is used in prototypes, so it can't be a type that will be
   widened for argument passing.  */
typedef unsigned int relax_substateT;

/* Enough bits for address, but still an integer type.
   Could be a problem, cross-assembling for 64-bit machines.  */
typedef addressT relax_addressT;

struct relax_type {
    /* Forward reach. Signed number. > 0.  */
    offsetT rlx_forward;
    /* Backward reach. Signed number. < 0.  */
    offsetT rlx_backward;

    /* Bytes length of this address.  */
    unsigned char rlx_length;

    /* Next longer relax-state.  0 means there is no 'next' relax-state.  */
    relax_substateT rlx_more;
};

typedef struct relax_type relax_typeS;

/* main program "as.c" (command arguments etc).  */

COMMON unsigned char flag_no_comments; /* -f */
COMMON unsigned char flag_debug; /* -D */
COMMON unsigned char flag_signed_overflow_ok; /* -J */
#ifndef WORKING_DOT_WORD
COMMON unsigned char flag_warn_displacement; /* -K */
#endif

/* True if local symbols should be retained.  */
COMMON int flag_keep_locals; /* -L */

/* True if we are assembling in MRI mode.  */
COMMON int flag_mri;

/* Should the data section be made read-only and appended to the text
   section?  */
COMMON unsigned char flag_readonly_data_in_text; /* -R */

/* True if warnings should be inhibited.  */
COMMON int flag_no_warnings; /* -W */

/* True if warnings count as errors.  */
COMMON int flag_fatal_warnings; /* --fatal-warnings */

/* True if we should attempt to generate output even if non-fatal errors
   are detected.  */
COMMON unsigned char flag_always_generate_output; /* -Z */

/* This is true if the assembler should output time and space usage.  */
COMMON unsigned char flag_print_statistics;

/* True if local absolute symbols are to be stripped.  */
COMMON int flag_strip_local_absolute;

/* True if we should generate a traditional format object file.  */
COMMON int flag_traditional_format;

/* Type of compressed debug sections we should generate.   */
COMMON enum compressed_debug_section_type flag_compress_debug;

/* TRUE if .note.GNU-stack section with SEC_CODE should be created */
COMMON int flag_execstack;

/* TRUE if .note.GNU-stack section with SEC_CODE should be created */
COMMON int flag_noexecstack;

/* name of emitted object file */
COMMON const char *out_file_name;

/* name of file defining extensions to the basic instruction set */
COMMON char *insttbl_file_name;

/* TRUE if we need a second pass.  */
COMMON int need_pass_2;

/* TRUE if we should do no relaxing, and
   leave lots of padding.  */
COMMON int linkrelax;

COMMON int do_not_pad_sections_to_alignment;

/* TRUE if we should produce a listing.  */
extern int listing;

/* Type of debugging information we should generate.  We currently support
   stabs, ECOFF, and DWARF2.

   NOTE!  This means debug information about the assembly source code itself
   and _not_ about possible debug information from a high-level language.
   This is especially relevant to DWARF2, since the compiler may emit line
   number directives that the assembler resolves.  */

enum debug_info_type {
    DEBUG_UNSPECIFIED,
    DEBUG_NONE,
    DEBUG_DWARF,
    DEBUG_DWARF2
};

extern enum debug_info_type debug_type;
extern int use_gnu_debug_info_extensions;
COMMON bfd_boolean flag_dwarf_sections;

/* Maximum level of macro nesting.  */
extern int max_macro_nest;

/* Verbosity level.  */
extern int verbose;

/* Obstack chunk size.  Keep large for efficient space use, make small to
   increase malloc calls for monitoring memory allocation.  */
extern int chunksize;

enum agbasm_type {
    AGBASM_DISABLED,
    AGBASM_NORMAL,
    AGBASM_DEBUG
};

/* agbasm features enabled */
COMMON enum agbasm_type flag_agbasm;
/* filename for debug output */
extern char * agbasm_debug_filename;

struct _pseudo_type {
    /* assembler mnemonic, lower case, no '.' */
    const char *poc_name;
    /* Do the work */
    void (*poc_handler) (int);
    /* Value to pass to handler */
    int poc_val;
};

typedef struct _pseudo_type pseudo_typeS;

#if (__GNUC__ >= 2) && !defined(VMS)
/* for use with -Wformat */

#if __GNUC__ == 2 && __GNUC_MINOR__ < 6
/* Support for double underscores in attribute names was added in gcc
   2.6, so avoid them if we are using an earlier version.  */
#define __printf__ printf
#define __format__ format
#endif

#define PRINTF_LIKE(FCN) \
    void FCN(const char *format, ...) \
    __attribute__ ((__format__(__printf__, 1, 2)))
#define PRINTF_WHERE_LIKE(FCN) \
    void FCN(const char *file, unsigned int line, const char *format, ...) \
    __attribute__ ((__format__(__printf__, 3, 4)))

#else /* __GNUC__ < 2 || defined(VMS) */

#define PRINTF_LIKE(FCN)        void FCN(const char *format, ...)
#define PRINTF_WHERE_LIKE(FCN)  void FCN(const char *file, \
                                         unsigned int line, \
                                         const char *format, ...)

#endif /* __GNUC__ < 2 || defined(VMS) */

PRINTF_LIKE(as_bad);
PRINTF_LIKE(as_fatal) ATTRIBUTE_NORETURN;
PRINTF_LIKE(as_tsktsk);
PRINTF_LIKE(as_warn);
PRINTF_WHERE_LIKE(as_bad_where);
PRINTF_WHERE_LIKE(as_warn_where);

void   as_abort(const char *, int, const char *) ATTRIBUTE_NORETURN;
void   signal_init(void);
void sprint_value(char *, addressT);
int    had_errors(void);
int    had_warnings(void);
void as_warn_value_out_of_range(const char *, offsetT, offsetT, offsetT,
                                const char *, unsigned);
void   print_version_id(void);
char * app_push(void);
char * atof_ieee(char *, int, LITTLENUM_TYPE *);
const char * ieee_md_atof(int, char *, int *, bfd_boolean);
const char * vax_md_atof(int, char *, int *);
char * input_scrub_include_file(const char *, char *);
void   input_scrub_insert_file(char *);
char * input_scrub_new_file(const char *);
char * input_scrub_next_buffer(char **bufp);
size_t do_scrub_chars(size_t (*get)(char *, size_t), char *, size_t);
int    gen_to_words(LITTLENUM_TYPE *, int, long);
int    had_err(void);
int    ignore_input(void);
void   cond_finish_check(int);
void   cond_exit_macro(int);
int    seen_at_least_1_file(void);
void   app_pop(char *);
const char * as_where(unsigned int *);
const char * as_where_physical(unsigned int *);
void   bump_line_counters(void);
void   do_scrub_begin(int);
void   input_scrub_begin(void);
void   input_scrub_close(void);
void   input_scrub_end(void);
int    new_logical_line(const char *, int);
int    new_logical_line_flags(const char *, int, int);
void   subsegs_begin(void);
void subseg_change(segT, int);
segT subseg_new(const char *, subsegT);
segT subseg_force_new(const char *, subsegT);
void subseg_set(segT, subsegT);
int subseg_text_p(segT);
int seg_not_empty_p(segT);
void   start_dependencies(char *);
void   register_dependency(const char *);
void   print_dependencies(void);
segT   subseg_get(const char *, int);

const char *remap_debug_filename(const char *);
void add_debug_prefix_map(const char *);

static inline char *xmemdup0(const char *in, size_t len)
{
    char *out = (char*)malloc(len + 1);
    out[len] = 0;
    return (char*)memcpy(out, in, len);
}

struct expressionS;
struct fix;
typedef struct symbol symbolS;
typedef struct frag fragS;

/* literal.c */
valueT add_to_literal_pool(symbolS *, valueT, segT, int);

int check_eh_frame(struct expressionS *, unsigned int *);
int eh_frame_estimate_size_before_relax(fragS *);
int eh_frame_relax_frag(fragS *);
void eh_frame_convert_frag(fragS *);
int generic_force_reloc(struct fix *);
/* debug output for agbasm */
void agbasm_debug_write(const char * format, ...);

#include "expr.h"               /* Before targ-*.h */

/* This one starts the chain of target dependent headers.  */
#include "te-armeabi.h"

#ifdef OBJ_MAYBE_ELF
#define IS_ELF (OUTPUT_FLAVOR == bfd_target_elf_flavour)
#else
#ifdef OBJ_ELF
#define IS_ELF 1
#else
#define IS_ELF 0
#endif
#endif

#include "write.h"
#include "frags.h"
#include "hash.h"
#include "read.h"
#include "symbols.h"

#include "tc.h"
#include "obj.h"

#include "listing.h"

#ifdef H_TICK_HEX
extern int enable_h_tick_hex;
#endif

#define flag_m68k_mri 0

#ifdef WARN_COMMENTS
COMMON int warn_comment;
COMMON unsigned int found_comment;
COMMON const char *        found_comment_file;
#endif

#if defined OBJ_ELF || defined OBJ_MAYBE_ELF
/* If .size directive failure should be error or warning.  */
COMMON int flag_allow_nonconst_size;

/* If we should generate ELF common symbols with the STT_COMMON type.  */
extern int flag_use_elf_stt_common;

/* TRUE iff GNU Build attribute notes should
   be generated if none are in the input files.  */
extern bfd_boolean flag_generate_build_notes;

/* If section name substitution sequences should be honored */
COMMON int flag_sectname_subst;
#endif

#ifndef DOLLAR_AMBIGU
#define DOLLAR_AMBIGU 0
#endif

#ifndef NUMBERS_WITH_SUFFIX
#define NUMBERS_WITH_SUFFIX 0
#endif

#ifndef LOCAL_LABELS_DOLLAR
#define LOCAL_LABELS_DOLLAR 0
#endif

#ifndef LOCAL_LABELS_FB
#define LOCAL_LABELS_FB 0
#endif

#ifndef LABELS_WITHOUT_COLONS
#define LABELS_WITHOUT_COLONS 0
#endif

#ifndef NO_PSEUDO_DOT
#define NO_PSEUDO_DOT 0
#endif

#ifndef TEXT_SECTION_NAME
#define TEXT_SECTION_NAME       ".text"
#define DATA_SECTION_NAME       ".data"
#define BSS_SECTION_NAME        ".bss"
#endif

#ifndef OCTETS_PER_BYTE_POWER
#define OCTETS_PER_BYTE_POWER 0
#endif
#ifndef OCTETS_PER_BYTE
#define OCTETS_PER_BYTE (1 << OCTETS_PER_BYTE_POWER)
#endif
#if OCTETS_PER_BYTE != (1 << OCTETS_PER_BYTE_POWER)
 #error "Octets per byte conflicts with its power-of-two definition!"
#endif

#endif /* GAS */
