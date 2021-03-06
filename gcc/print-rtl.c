/* Print RTL for GNU C Compiler.
   Copyright (C) 1987, 1988, 1992, 1997, 1998 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */


#include "config.h"
#include "system.h"
#include "rtl.h"
#include "bitmap.h"
#include "real.h"
#include "flags.h"


/* How to print out a register name.
   We don't use PRINT_REG because some definitions of PRINT_REG
   don't work here.  */
#ifndef DEBUG_PRINT_REG
#define DEBUG_PRINT_REG(RTX, CODE, FILE) fprintf((FILE), "%d %s", REGNO(RTX), reg_names[REGNO(RTX)])
#endif

/* Array containing all of the register names */

#ifdef DEBUG_REGISTER_NAMES
static const char *reg_names[] = DEBUG_REGISTER_NAMES;
#else
static const char *reg_names[] = REGISTER_NAMES;
#endif

static FILE *outfile;

static const char xspaces[]
    = "                                                                                                                                                                ";

static int sawclose = 0;

static int indent = 0;

/* Names for patterns.  Non-zero only when linked with insn-output.c.  */

extern char **insn_name_ptr;

static void print_rtx(rtx);

/* Nonzero means suppress output of instruction numbers and line number
   notes in debugging dumps.
   This must be defined here so that programs like gencodes can be linked.  */
int flag_dump_unnumbered = 0;

/* Nonzero if we are dumping graphical description.  */
int dump_for_graph;

/* Print IN_RTX onto OUTFILE.  This is the recursive part of printing.  */

static void print_rtx(rtx in_rtx)
{
    int i = 0;
    int j;
    const char *format_ptr;
    int is_insn;

    if (sawclose)
    {
        fprintf(outfile, "\n%s", (xspaces + (sizeof xspaces - 1 - indent * 2)));
        sawclose = 0;
    }

    if (in_rtx == 0)
    {
        fputs("(nil)", outfile);
        sawclose = 1;
        return;
    }

    is_insn = (GET_RTX_CLASS(GET_CODE(in_rtx)) == 'i');

    /* When printing in VCG format we write INSNs, NOTE, LABEL, and BARRIER
       in separate nodes and therefore have to handle them special here.  */
    if (dump_for_graph
        && (is_insn || GET_CODE(in_rtx) == NOTE || GET_CODE(in_rtx) == CODE_LABEL
               || GET_CODE(in_rtx) == BARRIER))
    {
        i = 3;
        indent = 0;
    }
    else
    {
        /* print name of expression code */
        fprintf(outfile, "(%s", GET_RTX_NAME(GET_CODE(in_rtx)));

        if (in_rtx->in_struct)
            fputs("/s", outfile);

        if (in_rtx->volatil)
            fputs("/v", outfile);

        if (in_rtx->unchanging)
            fputs("/u", outfile);

        if (in_rtx->integrated)
            fputs("/i", outfile);

        if (in_rtx->frame_related)
            fputs("/f", outfile);

        if (GET_MODE(in_rtx) != VOIDmode)
        {
            /* Print REG_NOTE names for EXPR_LIST and INSN_LIST.  */
            if (GET_CODE(in_rtx) == EXPR_LIST || GET_CODE(in_rtx) == INSN_LIST)
                fprintf(outfile, ":%s", GET_REG_NOTE_NAME(GET_MODE(in_rtx)));
            else
                fprintf(outfile, ":%s", GET_MODE_NAME(GET_MODE(in_rtx)));
        }
    }

    /* Get the format string and skip the first elements if we have handled
       them already.  */
    format_ptr = GET_RTX_FORMAT(GET_CODE(in_rtx)) + i;

    for (; i < GET_RTX_LENGTH(GET_CODE(in_rtx)); i++)
        switch (*format_ptr++)
        {
        case 'S':
        case 's':
            if (i == 3 && GET_CODE(in_rtx) == NOTE
                && (NOTE_LINE_NUMBER(in_rtx) == NOTE_INSN_EH_REGION_BEG
                       || NOTE_LINE_NUMBER(in_rtx) == NOTE_INSN_EH_REGION_END
                       || NOTE_LINE_NUMBER(in_rtx) == NOTE_INSN_BLOCK_BEG
                       || NOTE_LINE_NUMBER(in_rtx) == NOTE_INSN_BLOCK_END))
            {
                fprintf(outfile, " %d", NOTE_BLOCK_NUMBER(in_rtx));
                sawclose = 1;
                break;
            }

            if (i == 3 && GET_CODE(in_rtx) == NOTE
                && (NOTE_LINE_NUMBER(in_rtx) == NOTE_INSN_RANGE_START
                       || NOTE_LINE_NUMBER(in_rtx) == NOTE_INSN_RANGE_END
                       || NOTE_LINE_NUMBER(in_rtx) == NOTE_INSN_LIVE))
            {
                indent += 2;
                if (!sawclose)
                    fprintf(outfile, " ");
                print_rtx(NOTE_RANGE_INFO(in_rtx));
                indent -= 2;
                break;
            }

            if (XSTR(in_rtx, i) == 0)
                fputs(dump_for_graph ? " \\\"\\\"" : " \"\"", outfile);
            else
                fprintf(outfile, dump_for_graph ? " (\\\"%s\\\")" : " (\"%s\")", XSTR(in_rtx, i));
            sawclose = 1;
            break;

            /* 0 indicates a field for internal use that should not be printed.  */
        case '0':
            break;

        case 'e':
            indent += 2;
            if (!sawclose)
                fprintf(outfile, " ");
            print_rtx(XEXP(in_rtx, i));
            indent -= 2;
            break;

        case 'E':
        case 'V':
            indent += 2;
            if (sawclose)
            {
                fprintf(outfile, "\n%s", (xspaces + (sizeof xspaces - 1 - indent * 2)));
                sawclose = 0;
            }
            fputs("[ ", outfile);
            if (NULL != XVEC(in_rtx, i))
            {
                indent += 2;
                if (XVECLEN(in_rtx, i))
                    sawclose = 1;

                for (j = 0; j < XVECLEN(in_rtx, i); j++)
                    print_rtx(XVECEXP(in_rtx, i, j));

                indent -= 2;
            }
            if (sawclose)
                fprintf(outfile, "\n%s", (xspaces + (sizeof xspaces - 1 - indent * 2)));

            fputs("] ", outfile);
            sawclose = 1;
            indent -= 2;
            break;

        case 'w':
            fprintf(outfile, " ");
            fprintf(outfile, int32_t_PRINT_DEC, XWINT(in_rtx, i));
            break;

        case 'i':
        {
            int value = XINT(in_rtx, i);

            if (GET_CODE(in_rtx) == REG && value < FIRST_PSEUDO_REGISTER)
            {
                fputc(' ', outfile);
                DEBUG_PRINT_REG(in_rtx, 0, outfile);
            }
            else if (flag_dump_unnumbered && (is_insn || GET_CODE(in_rtx) == NOTE))
                fputc('#', outfile);
            else
                fprintf(outfile, " %d", value);
        }
            if (is_insn && &INSN_CODE(in_rtx) == &XINT(in_rtx, i) && insn_name_ptr
                && XINT(in_rtx, i) >= 0)
                fprintf(outfile, " {%s}", insn_name_ptr[XINT(in_rtx, i)]);
            sawclose = 0;
            break;

            /* Print NOTE_INSN names rather than integer codes.  */

        case 'n':
            if (XINT(in_rtx, i) <= 0)
                fprintf(outfile, " %s", GET_NOTE_INSN_NAME(XINT(in_rtx, i)));
            else
                fprintf(outfile, " %d", XINT(in_rtx, i));
            sawclose = 0;
            break;

        case 'u':
            if (XEXP(in_rtx, i) != NULL)
            {
                if (flag_dump_unnumbered)
                    fputc('#', outfile);
                else
                    fprintf(outfile, " %d", INSN_UID(XEXP(in_rtx, i)));
            }
            else
                fputs(" 0", outfile);
            sawclose = 0;
            break;

        case 'b':
            if (XBITMAP(in_rtx, i) == NULL)
                fputs(" {null}", outfile);
            else
                bitmap_print(outfile, XBITMAP(in_rtx, i), " {", "}");
            sawclose = 0;
            break;

        case 't':
            fprintf(outfile, " %p", (char *)XTREE(in_rtx, i));
            break;

        case '*':
            fputs(" Unknown", outfile);
            sawclose = 0;
            break;

        default:
            fprintf(stderr,
                "switch format wrong in rtl.print_rtx(). format was: %c.\n",
                format_ptr[-1]);
            abort();
        }

    if (GET_CODE(in_rtx) == MEM)
        fprintf(outfile, " %d", MEM_ALIAS_SET(in_rtx));

#if HOST_FLOAT_FORMAT == TARGET_FLOAT_FORMAT && LONG_DOUBLE_TYPE_SIZE == 64
    if (GET_CODE(in_rtx) == CONST_DOUBLE && FLOAT_MODE_P(GET_MODE(in_rtx)))
    {
        double val;
        REAL_VALUE_FROM_CONST_DOUBLE(val, in_rtx);
        fprintf(outfile, " [%.16g]", val);
    }
#endif

    if (dump_for_graph
        && (is_insn || GET_CODE(in_rtx) == NOTE || GET_CODE(in_rtx) == CODE_LABEL
               || GET_CODE(in_rtx) == BARRIER))
        sawclose = 0;
    else
    {
        fputc(')', outfile);
        sawclose = 1;
    }
}

/* Call this function from the debugger to see what X looks like.  */

void debug_rtx(rtx x)
{
    outfile = stderr;
    print_rtx(x);
    fprintf(stderr, "\n");
}

/* External entry point for printing a chain of insns
   starting with RTX_FIRST onto file OUTF.
   A blank line separates insns.

   If RTX_FIRST is not an insn, then it alone is printed, with no newline.  */

void print_rtl(FILE *outf, rtx rtx_first)
{
    rtx tmp_rtx;

    outfile = outf;
    sawclose = 0;

    if (rtx_first == 0)
        fputs("(nil)\n", outf);
    else
        switch (GET_CODE(rtx_first))
        {
        case INSN:
        case JUMP_INSN:
        case CALL_INSN:
        case NOTE:
        case CODE_LABEL:
        case BARRIER:
            for (tmp_rtx = rtx_first; NULL != tmp_rtx; tmp_rtx = NEXT_INSN(tmp_rtx))
            {
                if (!flag_dump_unnumbered || GET_CODE(tmp_rtx) != NOTE
                    || NOTE_LINE_NUMBER(tmp_rtx) < 0)
                {
                    print_rtx(tmp_rtx);
                    fprintf(outfile, "\n");
                }
            }
            break;

        default:
            print_rtx(rtx_first);
        }
}

/* Like print_rtx, except specify a file.  */
/* Return nonzero if we actually printed anything.  */

int print_rtl_single(FILE *outf, rtx x)
{
    outfile = outf;
    sawclose = 0;
    if (!flag_dump_unnumbered || GET_CODE(x) != NOTE || NOTE_LINE_NUMBER(x) < 0)
    {
        print_rtx(x);
        putc('\n', outf);
        return 1;
    }
    return 0;
}
