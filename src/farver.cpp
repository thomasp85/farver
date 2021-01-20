#include "farver.h"
#include "Comparison.h"
#include <memory>

//------------------------------------------------------------------------------
//--- Conversions --------------------------------------------------------------
//------------------------------------------------------------------------------

// this is where the real work happens, 
// the template parameters Space_From and Space_To give us the types of the 
// from and to colours. 
template <typename Space_From, typename Space_To>
SEXP convert_dispatch_impl(SEXP colour, SEXP white_from, SEXP white_to) {
  int ncol = dimension<Space_From>();
  // check that the dimensions of the input match the colour space
  if (ncol > Rf_ncols(colour)) {
    Rf_errorcall(R_NilValue, "colourspace requires %d values", ncol);    
  }
  int ncol_out = dimension<Space_To>();
  
  double wf1 = REAL(white_from)[0];
  double wf2 = REAL(white_from)[1];
  double wf3 = REAL(white_from)[2];
  double wt1 = REAL(white_to)[0];
  double wt2 = REAL(white_to)[1];
  double wt3 = REAL(white_to)[2];
  
  // make the result matrix
  int n = Rf_nrows(colour);
  SEXP out = PROTECT(Rf_allocMatrix(REALSXP, n, ncol_out));
  double* out_p = REAL(out);
  
  ColorSpace::Rgb rgb;
  Space_To to;
  int offset1 = 0;
  int offset2 = offset1 + n;
  int offset3 = offset2 + n;
  int offset4 = offset3 + n;
  double* colour_d = NULL;
  int* colour_i = NULL;
  bool colour_is_int = Rf_isInteger(colour);
  if (colour_is_int) {
    colour_i = INTEGER(colour);
  } else {
    colour_d = REAL(colour);
  }
  
  for (int i = 0; i < n; ++i) {
    // fill `rgb` based on the input color in the `from` color space
    ColorSpace::XyzConverter::SetWhiteReference(wf1, wf2, wf3);
    if (colour_is_int) {
      fill_rgb<Space_From>(&rgb, colour_i[offset1 + i], colour_i[offset2 + i], colour_i[offset3 + i], ncol == 4 ? colour_i[offset4 + i] : 0);
    } else {
      fill_rgb<Space_From>(&rgb, colour_d[offset1 + i], colour_d[offset2 + i], colour_d[offset3 + i], ncol == 4 ? colour_d[offset4 + i] : 0.0);
    }
    
    // ... convert the color to the `to` color space
    ColorSpace::XyzConverter::SetWhiteReference(wt1, wt2, wt3);
    ColorSpace::IConverter<Space_To>::ToColorSpace(&rgb, &to);
    
    // ... and move it to the row of the `out` matrix
    to.Cap();
    grab<Space_To>(to, out_p + offset1 + i, out_p + offset2 + i, out_p + offset3 + i, ncol_out == 4 ? out_p + offset4 + i : out_p);
  }
  
  copy_names(colour, out);
  UNPROTECT(1);
  return out;
}

// this is a trick to do a runtime fake compile time dispatch
// the idea is to call the right instantiation of the convert_dispatch_impl template 
// where the real work happens, based on `to`
template <typename From>
SEXP convert_dispatch_to(SEXP colour, int to, SEXP white_from, SEXP white_to) {
  switch (to) {
    case CMY: return convert_dispatch_impl<From, ColorSpace::Cmy>(colour, white_from, white_to);
    case CMYK: return convert_dispatch_impl<From, ColorSpace::Cmyk>(colour, white_from, white_to);
    case HSL: return convert_dispatch_impl<From, ColorSpace::Hsl>(colour, white_from, white_to);
    case HSB: return convert_dispatch_impl<From, ColorSpace::Hsb>(colour, white_from, white_to);
    case HSV: return convert_dispatch_impl<From, ColorSpace::Hsv>(colour, white_from, white_to);
    case LAB: return convert_dispatch_impl<From, ColorSpace::Lab>(colour, white_from, white_to);
    case HUNTERLAB: return convert_dispatch_impl<From, ColorSpace::HunterLab>(colour, white_from, white_to);
    case LCH: return convert_dispatch_impl<From, ColorSpace::Lch>(colour, white_from, white_to);
    case LUV: return convert_dispatch_impl<From, ColorSpace::Luv>(colour, white_from, white_to);
    case RGB: return convert_dispatch_impl<From, ColorSpace::Rgb>(colour, white_from, white_to);
    case XYZ: return convert_dispatch_impl<From, ColorSpace::Xyz>(colour, white_from, white_to);
    case YXY: return convert_dispatch_impl<From, ColorSpace::Yxy>(colour, white_from, white_to);
    case HCL: return convert_dispatch_impl<From, ColorSpace::Hcl>(colour, white_from, white_to);
    case OKLAB: return convert_dispatch_impl<From, ColorSpace::OkLab>(colour, white_from, white_to);
    case OKLCH: return convert_dispatch_impl<From, ColorSpace::OkLch>(colour, white_from, white_to);
  }
  
  // never happens
  return colour ;
}

// same trick, but for the `from` 
SEXP convert_dispatch_from(SEXP colour, int from, int to, SEXP white_from, SEXP white_to) {
  switch (from) {
    case CMY: return convert_dispatch_to<ColorSpace::Cmy>(colour, to, white_from, white_to);
    case CMYK: return convert_dispatch_to<ColorSpace::Cmyk>(colour, to, white_from, white_to);
    case HSL: return convert_dispatch_to<ColorSpace::Hsl>(colour, to, white_from, white_to);
    case HSB: return convert_dispatch_to<ColorSpace::Hsb>(colour, to, white_from, white_to);
    case HSV: return convert_dispatch_to<ColorSpace::Hsv>(colour, to, white_from, white_to);
    case LAB: return convert_dispatch_to<ColorSpace::Lab>(colour, to, white_from, white_to);
    case HUNTERLAB: return convert_dispatch_to<ColorSpace::HunterLab>(colour, to, white_from, white_to);
    case LCH: return convert_dispatch_to<ColorSpace::Lch>(colour, to, white_from, white_to);
    case LUV: return convert_dispatch_to<ColorSpace::Luv>(colour, to, white_from, white_to);
    case RGB: return convert_dispatch_to<ColorSpace::Rgb>(colour, to, white_from, white_to);
    case XYZ: return convert_dispatch_to<ColorSpace::Xyz>(colour, to, white_from, white_to);
    case YXY: return convert_dispatch_to<ColorSpace::Yxy>(colour, to, white_from, white_to);
    case HCL: return convert_dispatch_to<ColorSpace::Hcl>(colour, to, white_from, white_to);
    case OKLAB: return convert_dispatch_to<ColorSpace::OkLab>(colour, to, white_from, white_to);
    case OKLCH: return convert_dispatch_to<ColorSpace::OkLch>(colour, to, white_from, white_to);
  }
  
  // never happens so we just return the input to quiet the compiler
  return colour;
}

SEXP convert_c(SEXP colour, SEXP from, SEXP to, SEXP white_from, SEXP white_to) {
  return convert_dispatch_from(colour, INTEGER(from)[0], INTEGER(to)[0], white_from, white_to);
}

//------------------------------------------------------------------------------
//--- Comparisons --------------------------------------------------------------
//------------------------------------------------------------------------------

double get_colour_dist(ColorSpace::Rgb& from, ColorSpace::Rgb& to, int dist) {
  switch (dist) {
    case EUCLIDEAN: return ColorSpace::EuclideanComparison::Compare(&from, &to);
    case CIE1976: return ColorSpace::Cie1976Comparison::Compare(&from, &to);
    case CIE94: return ColorSpace::Cie94Comparison::Compare(&from, &to);
    case CIE2000: return ColorSpace::Cie2000Comparison::Compare(&from, &to);
    case CMC: return ColorSpace::CmcComparison::Compare(&from, &to);
  }
  // Never happens
  return 0.0;
}

template <typename Space_From, typename Space_To>
SEXP compare_dispatch_impl(SEXP from, SEXP to, int dist, bool sym, SEXP white_from, SEXP white_to){
  int from_col = dimension<Space_From>();
  int to_col = dimension<Space_To>();
  
  // check that the dimensions of the input match the colour space
  if (from_col > Rf_ncols(from)) {
    Rf_errorcall(R_NilValue, "colourspace requires %d values", from_col);    
  }
  if (to_col > Rf_ncols(to)) {
    Rf_errorcall(R_NilValue, "colourspace requires %d values", to_col);    
  }
  double wf1 = REAL(white_from)[0];
  double wf2 = REAL(white_from)[1];
  double wf3 = REAL(white_from)[2];
  double wt1 = REAL(white_to)[0];
  double wt2 = REAL(white_to)[1];
  double wt3 = REAL(white_to)[2];
  int n = Rf_nrows(from);
  int m = Rf_nrows(to);
  int noffset1 = 0;
  int noffset2 = noffset1 + n;
  int noffset3 = noffset2 + n;
  int noffset4 = noffset3 + n;
  int moffset1 = 0;
  int moffset2 = moffset1 + m;
  int moffset3 = moffset2 + m;
  int moffset4 = moffset3 + m;
  double* from_d = NULL;
  double* to_d = NULL;
  int* from_i = NULL;
  int* to_i = NULL;
  bool from_is_int = Rf_isInteger(from);
  bool to_is_int = Rf_isInteger(to);
  if (from_is_int) {
    from_i = INTEGER(from);
  } else {
    from_d = REAL(from);
  }
  if (to_is_int) {
    to_i = INTEGER(to);
  } else {
    to_d = REAL(to);
  }
  SEXP out = PROTECT(Rf_allocMatrix(REALSXP, n, m));
  double* out_p = REAL(out);
  double temp = 0.0;
  
  ColorSpace::Rgb from_rgb, to_rgb;
  
  for (int i = 0; i < n; ++i) {
    ColorSpace::XyzConverter::whiteReference = {wf1, wf2, wf3};
    if (from_is_int) {
      fill_rgb<Space_From>(&from_rgb, from_i[noffset1 + i], from_i[noffset2 + i], from_i[noffset3 + i], from_col == 4 ? from_i[noffset4 + i] : 0); 
    } else {
      fill_rgb<Space_From>(&from_rgb, from_d[noffset1 + i], from_d[noffset2 + i], from_d[noffset3 + i], from_col == 4 ? from_d[noffset4 + i] : 0.0); 
    }
    ColorSpace::XyzConverter::whiteReference = {wt1, wt2, wt3};
    for (int j =  0; j < m; ++j) {
      if (sym && j <= i) {
        out_p[n * j + i] = 0.0;
        continue;
      }
      if (to_is_int) {
        fill_rgb<Space_From>(&to_rgb, to_i[moffset1 + j], to_i[moffset2 + j], to_i[moffset3 + j], to_col == 4 ? to_i[moffset4 + j] : 0);
      } else {
        fill_rgb<Space_From>(&to_rgb, to_d[moffset1 + j], to_d[moffset2 + j], to_d[moffset3 + j], to_col == 4 ? to_d[moffset4 + j] : 0.0); 
      }
      temp = get_colour_dist(from_rgb, to_rgb, dist);
      out_p[n * j + i] = temp < 0 ? R_NaReal : temp;
    }
  }
  
  copy_names(from, to, out);
  UNPROTECT(1);
  return out;
}

template <typename From>
SEXP compare_dispatch_to(SEXP from, SEXP to, int to_space, int dist, bool sym, SEXP white_from, SEXP white_to) {
  switch (to_space) {
    case CMY: return compare_dispatch_impl<From, ColorSpace::Cmy>(from, to, dist, sym, white_from, white_to);
    case CMYK: return compare_dispatch_impl<From, ColorSpace::Cmyk>(from, to, dist, sym, white_from, white_to);
    case HSL: return compare_dispatch_impl<From, ColorSpace::Hsl>(from, to, dist, sym, white_from, white_to);
    case HSB: return compare_dispatch_impl<From, ColorSpace::Hsb>(from, to, dist, sym, white_from, white_to);
    case HSV: return compare_dispatch_impl<From, ColorSpace::Hsv>(from, to, dist, sym, white_from, white_to);
    case LAB: return compare_dispatch_impl<From, ColorSpace::Lab>(from, to, dist, sym, white_from, white_to);
    case HUNTERLAB: return compare_dispatch_impl<From, ColorSpace::HunterLab>(from, to, dist, sym, white_from, white_to);
    case LCH: return compare_dispatch_impl<From, ColorSpace::Lch>(from, to, dist, sym, white_from, white_to);
    case LUV: return compare_dispatch_impl<From, ColorSpace::Luv>(from, to, dist, sym, white_from, white_to);
    case RGB: return compare_dispatch_impl<From, ColorSpace::Rgb>(from, to, dist, sym, white_from, white_to);
    case XYZ: return compare_dispatch_impl<From, ColorSpace::Xyz>(from, to, dist, sym, white_from, white_to);
    case YXY: return compare_dispatch_impl<From, ColorSpace::Yxy>(from, to, dist, sym, white_from, white_to);
    case HCL: return compare_dispatch_impl<From, ColorSpace::Hcl>(from, to, dist, sym, white_from, white_to);
    case OKLAB: return compare_dispatch_impl<From, ColorSpace::OkLab>(from, to, dist, sym, white_from, white_to);
    case OKLCH: return compare_dispatch_impl<From, ColorSpace::OkLch>(from, to, dist, sym, white_from, white_to);
  }
  
  // never happens
  return from;
}

SEXP compare_dispatch_from(SEXP from, SEXP to, int from_space, int to_space, int dist, bool sym, SEXP white_from, SEXP white_to) {
  switch (from_space) {
    case CMY: return compare_dispatch_to<ColorSpace::Cmy>(from, to, to_space, dist, sym, white_from, white_to);
    case CMYK: return compare_dispatch_to<ColorSpace::Cmyk>(from, to, to_space, dist, sym, white_from, white_to);
    case HSL: return compare_dispatch_to<ColorSpace::Hsl>(from, to, to_space, dist, sym, white_from, white_to);
    case HSB: return compare_dispatch_to<ColorSpace::Hsb>(from, to, to_space, dist, sym, white_from, white_to);
    case HSV: return compare_dispatch_to<ColorSpace::Hsv>(from, to, to_space, dist, sym, white_from, white_to);
    case LAB: return compare_dispatch_to<ColorSpace::Lab>(from, to, to_space, dist, sym, white_from, white_to);
    case HUNTERLAB: return compare_dispatch_to<ColorSpace::HunterLab>(from, to, to_space, dist, sym, white_from, white_to);
    case LCH: return compare_dispatch_to<ColorSpace::Lch>(from, to, to_space, dist, sym, white_from, white_to);
    case LUV: return compare_dispatch_to<ColorSpace::Luv>(from, to, to_space, dist, sym, white_from, white_to);
    case RGB: return compare_dispatch_to<ColorSpace::Rgb>(from, to, to_space, dist, sym, white_from, white_to);
    case XYZ: return compare_dispatch_to<ColorSpace::Xyz>(from, to, to_space, dist, sym, white_from, white_to);
    case YXY: return compare_dispatch_to<ColorSpace::Hcl>(from, to, to_space, dist, sym, white_from, white_to);
    case OKLAB: return compare_dispatch_to<ColorSpace::OkLab>(from, to, to_space, dist, sym, white_from, white_to);
    case OKLCH: return compare_dispatch_to<ColorSpace::OkLch>(from, to, to_space, dist, sym, white_from, white_to);
  }
  
  // never happens so we just return the input to quiet the compiler
  return from;
}

SEXP compare_c(SEXP from, SEXP to, SEXP from_space, SEXP to_space, SEXP dist, SEXP sym, SEXP white_from, SEXP white_to) {
  return compare_dispatch_from(from, to, INTEGER(from_space)[0], INTEGER(to_space)[0], INTEGER(dist)[0], LOGICAL(sym)[0], white_from, white_to);
}
