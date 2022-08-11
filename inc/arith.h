#ifndef ARITH_H
#define ARITH_H 1
/* $Id: arith.h,v 1.2 1999/01/03 02:05:52 sybalsky Exp $ (C) Copyright Venue, All Rights Reserved */

/************************************************************************/
/*									*/
/*	(C) Copyright 1989-92 Venue. All Rights Reserved.		*/
/*	Manufactured in the United States of America.			*/
/*									*/
/************************************************************************/

#include "adr68k.h"    // for Addr68k_from_LADDR, LADDR_from_68k
#include "commondefs.h" // for error
#include "lispemul.h"  // for SEGMASK, ERROR_EXIT
#include "lispmap.h"   // for S_NEGATIVE, S_POSITIVE
#include "lsptypes.h"  // for TYPE_FIXP, GetTypeNumber, TYPE_FLOATP
#include "mkcelldefs.h" // for createcell68k

#define MAX_SMALL 65535  /* == 0x0000FFFF  */
#define MIN_SMALL (-65536) /* == 0xFFFF0000  */

#define MAX_FIXP 2147483647  /* == 0x7FFFFFFF  */
#define MIN_FIXP (-2147483648) /* == 0x80000000  */

#define GetSmalldata(x)          \
  (((SEGMASK & (x)) == S_POSITIVE)            \
       ? (0xFFFF & (x))            \
       : (((SEGMASK & (x)) == S_NEGATIVE) ? (0xFFFF0000 | (x)) : error("Not smallp address")))

#define GetSmallp(x)                                                                  \
  ((0xFFFF0000 & (x)) ? (((0xFFFF0000 & (x)) == 0xFFFF0000) ? (S_NEGATIVE | (0xFFFF & (x))) \
                                                        : error("Not Smallp data"))   \
                    : (S_POSITIVE | (0xFFFF & (x))))

#define FIXP_VALUE(dest) *((int *)Addr68k_from_LADDR(dest))

#define FLOATP_VALUE(dest) *((float *)Addr68k_from_LADDR(dest))

#define N_GETNUMBER(sour, dest, label)                        \
  do {                                                        \
    (dest) = (sour); /* access memory once */                 \
    switch (SEGMASK & (dest)) {                               \
      case S_POSITIVE: (dest) &= 0xFFFF; break;       \
      case S_NEGATIVE: (dest) |= (int)0xFFFF0000; break; \
      default:                                                \
        /* NOLINTNEXTLINE(bugprone-macro-parentheses) */      \
        if (GetTypeNumber(dest) != TYPE_FIXP) goto label;     \
        (dest) = FIXP_VALUE(dest);                            \
    }                                                         \
  } while (0)

#define N_IGETNUMBER(sour, dest, label)                                                   \
  do {                                                                                    \
    (dest) = (sour); /* access memory once */                                             \
    switch (SEGMASK & (dest)) {                                                           \
      case S_POSITIVE: (dest) &= 0xFFFF; break;                                   \
      case S_NEGATIVE: (dest) |= (int)0xFFFF0000; break; \
      default:                                                                            \
        switch (GetTypeNumber(dest)) {                                                    \
          case TYPE_FIXP: (dest) = FIXP_VALUE(dest); break;               \
          case TYPE_FLOATP: {                                                             \
            register float temp;                                                          \
            temp = FLOATP_VALUE(dest);                                                    \
            /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                              \
            if ((temp > ((float)0x7fffffff)) || (temp < ((float)0x80000000))) goto label; \
            (dest) = (int)temp;                                                           \
          } break;                                                                        \
          default: goto label; /* NOLINT(bugprone-macro-parentheses) */                   \
        }                                                                                 \
        break;                                                                            \
    }                                                                                     \
  } while (0)

#define ARITH_SWITCH(arg, result)                                          \
  do {                                                                     \
    switch ((arg) & (int)0xFFFF0000) {                      \
      case 0: (result) = (S_POSITIVE | (int)(arg)); break;                 \
      case (int)0xFFFF0000: (result) = (S_NEGATIVE | (0xFFFF & (arg))); break; \
      default: {                                                           \
        register LispPTR *wordp;                                           \
        /* arg is FIXP, call createcell */                                 \
        wordp = (LispPTR *)createcell68k(TYPE_FIXP);                       \
        *((int *)wordp) = (int)(arg);                                      \
        (result) = (LADDR_from_68k(wordp));                                \
        break;                                                             \
      }                                                                    \
    }                                                                      \
  } while (0)

/* *******
        NEED to See if this is faster than the N_ARITH_SWITCH macro

        if( (MIN_FIXP <= result) && (result <= MAX_FIXP) ){
                if(0 <= result){
                        if(result <= MAX_SMALL)
                                return(S_POSITIVE | result);
                        else{
                                wordp = createcell68k(TYPE_FIXP);
                                *((unsigned int *)wordp) = result;
                                return(LADDR_from_68k(wordp));
                        }
                }else{
                        if(MIN_SMALL <= result)
                                return(S_NEGATIVE | (0xFFFF & result));
                        else{
                                wordp = createcell68k(TYPE_FIXP);
                                *((unsigned int *)wordp) = result;
                                return(LADDR_from_68k(wordp));
                        }
                }/
        }
****** */

#define N_ARITH_SWITCH(arg)                                  \
  do {                                                       \
    switch ((arg) & (int)0xFFFF0000) {             \
    case 0: return (LispPTR) (S_POSITIVE | (arg));                       \
    case (int)0xFFFF0000: return (LispPTR)(S_NEGATIVE | (0xFFFF & (arg))); \
      default: {                                             \
        register LispPTR *fixpp;                             \
        /* arg is FIXP, call createcell */                   \
        fixpp = (LispPTR *)createcell68k(TYPE_FIXP);         \
        *((int *)fixpp) = arg;                               \
        return (LADDR_from_68k(fixpp));                      \
      }                                                      \
    }                                                        \
  } while (0)

#endif /* ARITH_H */
