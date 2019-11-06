#ifndef FARVER_INCLUDED
#define FARVER_INCLUDED

#include <R.h>
#include <Rinternals.h>

// these are used in the dispatcher functions
// this is one-based in the same order as the `colourspaces` 
// vector define in aaa.R
#define CMY 1
#define CMYK 2
#define HSL 3
#define HSB 4
#define HSV 5
#define LAB 6
#define HUNTERLAB 7
#define LCH 8
#define LUV 9
#define RGB 10
#define XYZ 11
#define YXY 12

#define EUCLIDEAN 1
#define CIE1976 2
#define CIE94 3
#define CIE2000 4
#define CMC 5


SEXP convert_c(SEXP colour, SEXP from, SEXP to, SEXP white_from, SEXP white_to);
SEXP compare_c(SEXP from, SEXP to, SEXP from_space, SEXP to_space, SEXP dist, SEXP sym, SEXP white_from, SEXP white_to);

#endif