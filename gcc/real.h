/* Definitions of floating-point access for GNU compiler.
   Copyright (C) 1989, 91, 94, 96, 97, 1998 Free Software Foundation, Inc.

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

#ifndef REAL_H_INCLUDED
#define REAL_H_INCLUDED

/* Define codes for all the float formats that we know of.  */
#define UNKNOWN_FLOAT_FORMAT 0
#define IEEE_FLOAT_FORMAT 1
#define VAX_FLOAT_FORMAT 2
#define IBM_FLOAT_FORMAT 3
#define C4X_FLOAT_FORMAT 4

/* Default to IEEE float if not specified.  Nearly all machines use it.  */

#define	TARGET_FLOAT_FORMAT	IEEE_FLOAT_FORMAT

#define	HOST_FLOAT_FORMAT	IEEE_FLOAT_FORMAT

#if TARGET_FLOAT_FORMAT == IEEE_FLOAT_FORMAT
#define REAL_INFINITY
#endif

/* If FLOAT_WORDS_BIG_ENDIAN and HOST_FLOAT_WORDS_BIG_ENDIAN are not defined
   in the header files, then this implies the word-endianness is the same as
   for integers.  */

/* This is defined 0 or 1, like WORDS_BIG_ENDIAN.  */
#ifndef FLOAT_WORDS_BIG_ENDIAN
#define FLOAT_WORDS_BIG_ENDIAN WORDS_BIG_ENDIAN
#endif

/* This is defined 0 or 1, unlike HOST_WORDS_BIG_ENDIAN.  */
#ifndef HOST_FLOAT_WORDS_BIG_ENDIAN
#ifdef HOST_WORDS_BIG_ENDIAN
#define HOST_FLOAT_WORDS_BIG_ENDIAN 1
#else
#define HOST_FLOAT_WORDS_BIG_ENDIAN 0
#endif
#endif

/* Defining REAL_ARITHMETIC invokes a floating point emulator
   that can produce a target machine format differing by more
   than just endian-ness from the host's format.  The emulator
   is also used to support extended real XFmode.  */
#define LONG_DOUBLE_TYPE_SIZE 64

/* **** Start of software floating point emulator interface macros **** */

extern int significand_size(enum machine_mode);

/* If emulation has been enabled by defining REAL_ARITHMETIC or by
   setting LONG_DOUBLE_TYPE_SIZE to 96 or 128, then define macros so that
   they invoke emulator functions. This will succeed only if the machine
   files have been updated to use these macros in place of any
   references to host machine `double' or `float' types.  */
#undef REAL_ARITHMETIC
#define REAL_ARITHMETIC(value, code, d1, d2) \
  earith (&(value), (code), &(d1), &(d2))

/* Declare functions in real.c. */
extern void earith(double *, int, double *, double *);
extern double etrunci(double);
extern double etruncui(double);
extern double ereal_atof(const char *, enum machine_mode);
extern double ereal_negate(double);
extern int32_t efixi(double);
extern uint32_t efixui(double);
extern void ereal_from_int(double *, int32_t, int32_t, enum machine_mode);
extern void ereal_from_uint(double *, uint32_t, uint32_t, enum machine_mode);
extern void ereal_to_int(int32_t *, int32_t *, double);
extern double ereal_ldexp(double, int);

extern void etardouble(double, int32_t[]);
extern int32_t etarsingle(double);
extern void ereal_to_decimal(double, char *);
extern int ereal_cmp(double, double);
extern int ereal_isneg(double);
extern double ereal_from_float(int32_t);
extern double ereal_from_double(int32_t[]);

#define REAL_VALUES_EQUAL(x, y) (ereal_cmp ((x), (y)) == 0)
/* true if x < y : */
#define REAL_VALUES_LESS(x, y) (ereal_cmp ((x), (y)) == -1)
#define REAL_VALUE_LDEXP(x, n) ereal_ldexp (x, n)

/* These return double: */
#define REAL_VALUE_RNDZINT(x) (etrunci (x))
#define REAL_VALUE_UNSIGNED_RNDZINT(x) (etruncui (x))
extern double real_value_truncate(enum machine_mode, double);
#define REAL_VALUE_TRUNCATE(mode, x)  real_value_truncate (mode, x)

/* These return int32_t: */
/* Convert a floating-point value to integer, rounding toward zero.  */
#define REAL_VALUE_FIX(x) (efixi (x))
/* Convert a floating-point value to unsigned integer, rounding
   toward zero. */
#define REAL_VALUE_UNSIGNED_FIX(x) (efixui (x))

/* Convert ASCII string S to floating point in mode M.
   Decimal input uses ATOF.  Hexadecimal uses HTOF.  */
#define REAL_VALUE_ATOF ereal_atof
#define REAL_VALUE_HTOF ereal_atof

#define REAL_VALUE_NEGATE ereal_negate

#define REAL_VALUE_MINUS_ZERO(x) \
 ((ereal_cmp (x, dconst0) == 0) && (ereal_isneg (x) != 0 ))

#define REAL_VALUE_TO_INT ereal_to_int

/* Here the cast to int32_t sign-extends arguments such as ~0.  */
#define REAL_VALUE_FROM_INT(d, lo, hi, mode) \
  ereal_from_int (&d, (int32_t) (lo), (int32_t) (hi), mode)

#define REAL_VALUE_FROM_UNSIGNED_INT(d, lo, hi, mode) \
  ereal_from_uint (&d, lo, hi, mode)

/* IN is a double.  OUT is an array of longs. */
#define REAL_VALUE_TO_TARGET_DOUBLE(IN, OUT) (etardouble ((IN), (OUT)))

/* IN is a double.  OUT is a long. */
#define REAL_VALUE_TO_TARGET_SINGLE(IN, OUT) ((OUT) = etarsingle ((IN)))

/* Inverse of REAL_VALUE_TO_TARGET_SINGLE. */
#define REAL_VALUE_UNTO_TARGET_SINGLE(f)  (ereal_unto_float (f))

/* d is an array of int32_t that holds a double precision
   value in the target computer's floating point format. */
#define REAL_VALUE_FROM_TARGET_DOUBLE(d)  (ereal_from_double (d))

/* f is a int32_t containing a single precision target float value. */
#define REAL_VALUE_FROM_TARGET_SINGLE(f)  (ereal_from_float (f))

/* Conversions to decimal ASCII string.  */
#define REAL_VALUE_TO_DECIMAL(r, fmt, s) (ereal_to_decimal (r, s))

/* **** End of software floating point emulator interface macros **** */

/* Convert a type `double' value in host format first to a type `float'
   value in host format and then to a single type `long' value which
   is the bitwise equivalent of the `float' value.  */
#ifndef REAL_VALUE_TO_TARGET_SINGLE
#define REAL_VALUE_TO_TARGET_SINGLE(IN, OUT)		\
do {							\
  union {						\
    float f;						\
    int32_t l;					\
  } u;							\
  u.l = 0;						\
  u.f = (IN);						\
  (OUT) = u.l;						\
} while (0)
#endif

/* Convert a type `double' value in host format to a pair of type `long'
   values which is its bitwise equivalent, but put the two words into
   proper word order for the target.  */
#ifndef REAL_VALUE_TO_TARGET_DOUBLE
#define REAL_VALUE_TO_TARGET_DOUBLE(IN, OUT)				\
do {									\
  union {								\
    double f;							\
    int32_t l[2];							\
  } u;									\
  u.l[0] = u.l[1] = 0;							\
  u.f = (IN);								\
  if (HOST_FLOAT_WORDS_BIG_ENDIAN == FLOAT_WORDS_BIG_ENDIAN)		\
    (OUT)[0] = u.l[0], (OUT)[1] = u.l[1];				\
  else									\
    (OUT)[1] = u.l[0], (OUT)[0] = u.l[1];				\
} while (0)
#endif

/* Compare two floating-point objects for bitwise identity.
   This is not the same as comparing for equality on IEEE hosts:
   -0.0 equals 0.0 but they are not identical, and conversely
   two NaNs might be identical but they cannot be equal.  */
#define REAL_VALUES_IDENTICAL(x, y) \
  (!memcmp ((char *) &(x), (char *) &(y), sizeof (double)))

/* Compare two floating-point values for equality.  */
#ifndef REAL_VALUES_EQUAL
#define REAL_VALUES_EQUAL(x, y) ((x) == (y))
#endif

/* Compare two floating-point values for less than.  */
#ifndef REAL_VALUES_LESS
#define REAL_VALUES_LESS(x, y) ((x) < (y))
#endif

/* Truncate toward zero to an integer floating-point value.  */
#ifndef REAL_VALUE_RNDZINT
#define REAL_VALUE_RNDZINT(x) ((double) ((int) (x)))
#endif

/* Truncate toward zero to an unsigned integer floating-point value.  */
#ifndef REAL_VALUE_UNSIGNED_RNDZINT
#define REAL_VALUE_UNSIGNED_RNDZINT(x) ((double) ((unsigned int) (x)))
#endif

/* Convert a floating-point value to integer, rounding toward zero.  */
#ifndef REAL_VALUE_FIX
#define REAL_VALUE_FIX(x) ((int) (x))
#endif

/* Convert a floating-point value to unsigned integer, rounding
   toward zero. */
#ifndef REAL_VALUE_UNSIGNED_FIX
#define REAL_VALUE_UNSIGNED_FIX(x) ((unsigned int) (x))
#endif

/* Scale X by Y powers of 2.  */
#ifndef REAL_VALUE_LDEXP
#define REAL_VALUE_LDEXP(x, y) ldexp (x, y)
extern double ldexp(double, int);
#endif

/* Convert the string X to a floating-point value.  */
#ifndef REAL_VALUE_ATOF
/* Use real.c to convert decimal numbers to binary, ... */
double ereal_atof();
#define REAL_VALUE_ATOF(x, s) ereal_atof (x, s)
/* Could use ereal_atof here for hexadecimal floats too, but real_hex_to_f
   is OK and it uses faster native fp arithmetic.  */
/* #define REAL_VALUE_HTOF(x, s) ereal_atof (x, s) */
#endif

/* Hexadecimal floating constant input for use with host computer's
   fp arithmetic.  */
#ifndef REAL_VALUE_HTOF
extern double real_hex_to_f(char *, enum machine_mode);
#define REAL_VALUE_HTOF(s,m) real_hex_to_f(s,m)
#endif

/* Negate the floating-point value X.  */
#ifndef REAL_VALUE_NEGATE
#define REAL_VALUE_NEGATE(x) (- (x))
#endif

/* Truncate the floating-point value X to mode MODE.  This is correct only
   for the most common case where the host and target have objects of the same
   size and where `float' is SFmode.  */

/* Don't use REAL_VALUE_TRUNCATE directly--always call real_value_truncate.  */
extern double real_value_truncate(enum machine_mode, double);

#ifndef REAL_VALUE_TRUNCATE
#define REAL_VALUE_TRUNCATE(mode, x) \
 (GET_MODE_BITSIZE (mode) == sizeof (float) * HOST_BITS_PER_CHAR	\
  ? (float) (x) : (x))
#endif

/* Determine whether a floating-point value X is infinite. */
#ifndef REAL_VALUE_ISINF
#define REAL_VALUE_ISINF(x) (target_isinf (x))
#endif

/* Determine whether a floating-point value X is a NaN. */
#ifndef REAL_VALUE_ISNAN
#define REAL_VALUE_ISNAN(x) (target_isnan (x))
#endif

/* Determine whether a floating-point value X is negative. */
#ifndef REAL_VALUE_NEGATIVE
#define REAL_VALUE_NEGATIVE(x) (target_negative (x))
#endif

extern int target_isnan(double);
extern int target_isinf(double);
extern int target_negative(double);

/* Determine whether a floating-point value X is minus 0. */
#ifndef REAL_VALUE_MINUS_ZERO
#define REAL_VALUE_MINUS_ZERO(x) ((x) == 0 && REAL_VALUE_NEGATIVE (x))
#endif

/* Constant real values 0, 1, 2, and -1.  */

extern double dconst0;
extern double dconst1;
extern double dconst2;
extern double dconstm1;

/* Union type used for extracting real values from CONST_DOUBLEs
   or putting them in.  */

union real_extract
{
  double d;
  int32_t i[2];
};

/* For a CONST_DOUBLE:
   The usual two ints that hold the value.
   For a DImode, that is all there are;
    and CONST_DOUBLE_LOW is the low-order word and ..._HIGH the high-order.
   For a float, the number of ints varies,
    and CONST_DOUBLE_LOW is the one that should come first *in memory*.
    So use &CONST_DOUBLE_LOW(r) as the address of an array of ints.  */
#define CONST_DOUBLE_LOW(r) XWINT (r, 2)
#define CONST_DOUBLE_HIGH(r) XWINT (r, 3)

/* Link for chain of all CONST_DOUBLEs in use in current function.  */
#define CONST_DOUBLE_CHAIN(r) XEXP (r, 1)
/* The MEM which represents this CONST_DOUBLE's value in memory,
   or const0_rtx if no MEM has been made for it yet,
   or cc0_rtx if it is not on the chain.  */
#define CONST_DOUBLE_MEM(r) XEXP (r, 0)

/* Given a CONST_DOUBLE in FROM, store into TO the value it represents.  */
/* Function to return a real value (not a tree node)
   from a given integer constant.  */
union tree_node;
extern double real_value_from_int_cst(tree, tree);

#define REAL_VALUE_FROM_CONST_DOUBLE(to, from) \
do { \
    union real_extract u; \
    size_t i; \
    for (i = 0; i < 2; i++) \
        u.i[i] = XWINT((from), 2 + i); \
    to = u.d; \
} while (0)

/* Return a CONST_DOUBLE with value R and mode M.  */

#define CONST_DOUBLE_FROM_REAL_VALUE(r, m) immed_real_const_1 (r,  m)
extern rtx immed_real_const_1(double, enum machine_mode);


/* Convert a floating point value `r', that can be interpreted
   as a host machine float or double, to a decimal ASCII string `s'
   using printf format string `fmt'.  */
#ifndef REAL_VALUE_TO_DECIMAL
#define REAL_VALUE_TO_DECIMAL(r, fmt, s) (sprintf (s, fmt, r))
#endif

/* Replace R by 1/R in the given machine mode, if the result is exact.  */
extern int exact_real_inverse(enum machine_mode, double *);

/* In varasm.c */
extern void assemble_real(double, enum machine_mode);
#endif /* Not REAL_H_INCLUDED */
