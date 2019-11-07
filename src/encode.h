#ifndef ENCODE_INCLUDED
#define ENCODE_INCLUDED

#include <R.h>
#include <Rinternals.h>
#include <unordered_map>
#include <string>

struct rgb_colour {
  int r;
  int g;
  int b;
};
typedef std::unordered_map<std::string, rgb_colour> ColourMap;

// Defined in init.cpp
ColourMap& get_named_colours();

SEXP encode_c(SEXP colour, SEXP alpha, SEXP from, SEXP white);
SEXP decode_c(SEXP codes, SEXP alpha, SEXP to, SEXP white);
SEXP load_colour_names_c(SEXP name, SEXP value);

#endif