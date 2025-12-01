/*
 * Types.h
 *
 *  Created on: Oct 2, 2025
 *      Author: ahmed
 */

#ifndef TYPES_H_
#define TYPES_H_

/*------------------------------ Types ------------------------------*/

/* Boolean Type */
typedef unsigned char               boolean;

/* Character Type */
typedef char                        char_t;

/* Signed Integer Types */
typedef signed char                 sint8_t;
typedef signed short                sint16_t;
typedef signed int                  sint32_t;
typedef signed long long            sint64_t;

/* Unsigned Integer Types */
typedef unsigned char               uint8_t;
typedef unsigned short              uint16_t;
typedef unsigned int                uint32_t;
typedef unsigned long long          uint64_t;

/* Volatile Signed Integer Types */
typedef volatile signed char        vsint8_t;
typedef volatile signed short       vsint16_t;
typedef volatile signed int         vsint32_t;
typedef volatile signed long long   vsint64_t;

/* Volatile Unsigned Integer Types */
typedef volatile unsigned char      vuint8_t;
typedef volatile unsigned short     vuint16_t;
typedef volatile unsigned int       vuint32_t;
typedef volatile unsigned long long vuint64_t;

/* Floating-Point Types */
typedef float                       float32;
typedef double                      float64;

/*--------------------------- Pointer Types --------------------------*/

/* Boolean Pointer */
typedef boolean*                    pboolean;

/* Character Pointer */
typedef char_t*                     pchar_t;

/* Signed Integer Pointers */
typedef sint8_t*                    psint8_t;
typedef sint16_t*                   psint16_t;
typedef sint32_t*                   psint32_t;
typedef sint64_t*                   psint64_t;

/* Unsigned Integer Pointers */
typedef uint8_t*                    puint8_t;
typedef uint16_t*                   puint16_t;
typedef uint32_t*                   puint32_t;
typedef uint64_t*                   puint64_t;

/* Volatile Signed Integer Pointers */
typedef vsint8_t*                   pvsint8_t;
typedef vsint16_t*                  pvsint16_t;
typedef vsint32_t*                  pvsint32_t;
typedef vsint64_t*                  pvsint64_t;

/* Volatile Unsigned Integer Pointers */
typedef vuint8_t*                   pvuint8_t;
typedef vuint16_t*                  pvuint16_t;
typedef vuint32_t*                  pvuint32_t;
typedef vuint64_t*                  pvuint64_t;

/* Floating Point Pointers */
typedef float32*                    pfloat32;
typedef float64*                    pfloat64;

/*------------------------------ Defines -----------------------------*/

#ifndef TRUE
#define TRUE                        ((boolean) 1)
#endif

#ifndef FALSE
#define FALSE                       ((boolean) 0)
#endif

#endif /* TYPES_H_ */
