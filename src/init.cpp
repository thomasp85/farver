#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>
#include "farver.h"
#include "encode.h"

static const R_CallMethodDef CallEntries[] = {
  {"convert_c", (DL_FUNC) &convert_c, 5},
  {"compare_c", (DL_FUNC) &compare_c, 8},
  {"encode_c", (DL_FUNC) &encode_c, 4},
  {NULL, NULL, 0}
};

extern "C" void R_init_farver(DllInfo *dll) {
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
