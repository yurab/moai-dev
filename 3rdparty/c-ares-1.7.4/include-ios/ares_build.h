#ifndef __CARES_BUILD_H
#define __CARES_BUILD_H


/* Copyright (C) 2009 - 2010 by Daniel Stenberg et al
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

/* ================================================================ */
/*               NOTES FOR CONFIGURE CAPABLE SYSTEMS                */
/* ================================================================ */

/*
 * NOTE 1:
 * -------
 *
 * See file ares_build.h.in, run configure, and forget that this file
 * exists it is only used for non-configure systems.
 * But you can keep reading if you want ;-)
 *
 */

/* ================================================================ */
/*                 NOTES FOR NON-CONFIGURE SYSTEMS                  */
/* ================================================================ */

/*
 * NOTE 1:
 * -------
 *
 * Nothing in this file is intended to be modified or adjusted by the
 * c-ares library user nor by the c-ares library builder.
 *
 * If you think that something actually needs to be changed, adjusted
 * or fixed in this file, then, report it on the c-ares development
 * mailing list: http://cool.haxx.se/mailman/listinfo/c-ares/
 *
 * Try to keep one section per platform, compiler and architecture,
 * otherwise, if an existing section is reused for a different one and
 * later on the original is adjusted, probably the piggybacking one can
 * be adversely changed.
 *
 * In order to differentiate between platforms/compilers/architectures
 * use only compiler built in predefined preprocessor symbols.
 *
 * This header file shall only export symbols which are 'cares' or 'CARES'
 * prefixed, otherwise public name space would be polluted.
 *
 * NOTE 2:
 * -------
 *
 * Right now you might be staring at file ares_build.h.dist or ares_build.h,
 * this is due to the following reason: file ares_build.h.dist is renamed
 * to ares_build.h when the c-ares source code distribution archive file is
 * created.
 *
 * File ares_build.h.dist is not included in the distribution archive.
 * File ares_build.h is not present in the git tree.
 *
 * The distributed ares_build.h file is only intended to be used on systems
 * which can not run the also distributed configure script.
 *
 * On systems capable of running the configure script, the configure process
 * will overwrite the distributed ares_build.h file with one that is suitable
 * and specific to the library being configured and built, which is generated
 * from the ares_build.h.in template file.
 *
 * If you check out from git on a non-configure platform, you must run the
 * appropriate buildconf* script to set up ares_build.h and other local files.
 *
 */

/* ================================================================ */
/*  DEFINITION OF THESE SYMBOLS SHALL NOT TAKE PLACE ANYWHERE ELSE  */
/* ================================================================ */
#error DIE!

#ifdef CARES_SIZEOF_LONG
#  error "CARES_SIZEOF_LONG shall not be defined except in ares_build.h"
   Error Compilation_aborted_CARES_SIZEOF_LONG_already_defined
#endif

#ifdef CARES_TYPEOF_ARES_SOCKLEN_T
#  error "CARES_TYPEOF_ARES_SOCKLEN_T shall not be defined except in ares_build.h"
   Error Compilation_aborted_CARES_TYPEOF_ARES_SOCKLEN_T_already_defined
#endif

#ifdef CARES_SIZEOF_ARES_SOCKLEN_T
#  error "CARES_SIZEOF_ARES_SOCKLEN_T shall not be defined except in ares_build.h"
   Error Compilation_aborted_CARES_SIZEOF_ARES_SOCKLEN_T_already_defined
#endif

/* ================================================================ */
/*    EXTERNAL INTERFACE SETTINGS FOR NON-CONFIGURE SYSTEMS ONLY    */
/* ================================================================ */
/* The size of `long', as computed by sizeof. */
#define CARES_SIZEOF_LONG 4

/* Integral data type used for curl_socklen_t. */
#define CARES_TYPEOF_ARES_SOCKLEN_T socklen_t

/* The size of `curl_socklen_t', as computed by sizeof. */
#define CARES_SIZEOF_ARES_SOCKLEN_T 4

/*#  define CARES_PULL_SYS_TYPES_H      1*/
/*#  define CARES_PULL_SYS_SOCKET_H     1*/


#endif /* __CARES_BUILD_H */
