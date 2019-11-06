#ifndef ENCODE_INCLUDED
#define ENCODE_INCLUDED

#include <R.h>
#include <Rinternals.h>

SEXP encode_c(SEXP colour, SEXP alpha, SEXP from, SEXP white);

#endif