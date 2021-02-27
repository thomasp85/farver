#include "farver.h"
#include "encode.h"
#include <R.h>
#define R_NO_REMAP
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

static ColourMap* named_colours;

ColourMap& get_named_colours() {
  return *named_colours;
}

static const R_CallMethodDef CallEntries[] = {
  {"convert_c", (DL_FUNC) &convert_c, 5},
  {"compare_c", (DL_FUNC) &compare_c, 8},
  {"encode_c", (DL_FUNC) &encode_c, 4},
  {"decode_c", (DL_FUNC) &decode_c, 5},
  {"encode_channel_c", (DL_FUNC) &encode_channel_c, 7},
  {"decode_channel_c", (DL_FUNC) &decode_channel_c, 5},
  {"load_colour_names_c", (DL_FUNC) &load_colour_names_c, 2},
  {"encode_native_c", (DL_FUNC) &encode_native_c, 1},
  {"decode_native_c", (DL_FUNC) &decode_native_c, 1},
  {NULL, NULL, 0}
};

extern "C" void R_init_farver(DllInfo *dll) {
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
  
  named_colours = new ColourMap();
}

extern "C" void R_unload_farver(DllInfo *dll) {
  delete named_colours;
}