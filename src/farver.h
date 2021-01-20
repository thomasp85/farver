#ifndef FARVER_INCLUDED
#define FARVER_INCLUDED

#include "ColorSpace.h"
#include <R.h>
#define R_NO_REMAP
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
#define HCL 13
#define OKLAB 14
#define OKLCH 15

#define EUCLIDEAN 1
#define CIE1976 2
#define CIE94 3
#define CIE2000 4
#define CMC 5

SEXP convert_c(SEXP colour, SEXP from, SEXP to, SEXP white_from, SEXP white_to);
SEXP compare_c(SEXP from, SEXP to, SEXP from_space, SEXP to_space, SEXP dist, SEXP sym, SEXP white_from, SEXP white_to);

inline void copy_names(SEXP from, SEXP to) {
  SEXP names;
  bool from_matrix = Rf_isMatrix(from);
  if (from_matrix) {
    names = PROTECT(Rf_getAttrib(from, Rf_install("dimnames")));
    if (!Rf_isNull(names)) {
      names = VECTOR_ELT(names, 0);
    }
  } else {
    names = PROTECT(Rf_getAttrib(from, R_NamesSymbol));
  }
  
  if (!Rf_isNull(names)) {
    if (Rf_isMatrix(to)) {
      SEXP dn = PROTECT(Rf_allocVector(VECSXP, 2));
      SET_VECTOR_ELT(dn, 0, names);
      Rf_setAttrib(to, Rf_install("dimnames"), dn);
      UNPROTECT(1);
    } else {
      Rf_namesgets(to, names);
    }
  }
  
  UNPROTECT(1);
}

inline void copy_names(SEXP from1, SEXP from2, SEXP to) {
  SEXP names1, names2, names;
  bool from1_matrix = Rf_isMatrix(from1);
  bool from2_matrix = Rf_isMatrix(from2);
  if (from1_matrix) {
    names1 = PROTECT(Rf_getAttrib(from1, Rf_install("dimnames")));
    if (!Rf_isNull(names1)) {
      names1 = VECTOR_ELT(names1, 0);
    }
  } else {
    names1 = PROTECT(Rf_getAttrib(from1, Rf_install("names")));
  }
  if (from2_matrix) {
    names2 = PROTECT(Rf_getAttrib(from2, Rf_install("dimnames")));
    if (!Rf_isNull(names2)) {
      names2 = VECTOR_ELT(names2, 0);
    }
  } else {
    names2 = PROTECT(Rf_getAttrib(from2, Rf_install("names")));
  }
  
  if ((!Rf_isNull(names1) || !Rf_isNull(names2)) && Rf_isMatrix(to)) {
    names = PROTECT(Rf_allocVector(VECSXP, 2));
    if (!Rf_isNull(names1)) {
      SET_VECTOR_ELT(names, 0, names1);
    }
    if (!Rf_isNull(names2)) {
      SET_VECTOR_ELT(names, 1, names2);
    }
    Rf_setAttrib(to, Rf_install("dimnames"), names);
    UNPROTECT(1);
  }
  
  UNPROTECT(2);
}

// returns the number of dimensions for a color space type
// with a special case for Cmyk
template <typename SPACE>
inline int dimension(){
  return 3 ;
}
template <>
inline int dimension<ColorSpace::Cmyk>(){
  return 4 ;
}

// read a color from a color space in the row, and convert it to rgb
template <typename Space>
inline void fill_rgb(ColorSpace::Rgb* rgb, double x1, double x2, double x3, double x4=0.0){
  Space col(x1, x2, x3);
  col.Cap();
  col.ToRgb(rgb);
}
template <>
inline void fill_rgb<ColorSpace::Cmyk>(ColorSpace::Rgb* rgb, double x1, double x2, double x3, double x4){
  ColorSpace::Cmyk col(x1, x2, x3, x4);
  col.Cap();
  col.ToRgb(rgb);
}
template <typename Space>
inline void fill_rgb(ColorSpace::Rgb* rgb, int x1, int x2, int x3, int x4=0){
  Space col(x1, x2, x3);
  col.Cap();
  col.ToRgb(rgb);
}
template <>
inline void fill_rgb<ColorSpace::Cmyk>(ColorSpace::Rgb* rgb, int x1, int x2, int x3, int x4){
  ColorSpace::Cmyk col(x1, x2, x3, x4);
  col.Cap();
  col.ToRgb(rgb);
}

// these grab values from the Space type and use them to fill `row`
// unfortunately, given how the `ColorSpace` C++ library is written, 
// this has to do lots of special casing
template <typename Space>
inline void grab(const Space&, double* x1, double* x2, double* x3, double* x4) ;

template <>
inline void grab<ColorSpace::Rgb>(const ColorSpace::Rgb& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.r ;
    *x2 = color.g ;
    *x3 = color.b ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::Xyz>(const ColorSpace::Xyz& color, double* x1, double* x2, double* x3, double* x4){
  *x1 = color.x ;
  *x2 = color.y ;
  *x3 = color.z ;
}

template <>
inline void grab<ColorSpace::Hsl>(const ColorSpace::Hsl& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.h ;
    *x2 = color.s ;
    *x3 = color.l ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::Lab>(const ColorSpace::Lab& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.l ;
    *x2 = color.a ;
    *x3 = color.b ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::Lch>(const ColorSpace::Lch& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.l ;
    *x2 = color.c ;
    *x3 = color.h ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::Luv>(const ColorSpace::Luv& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.l ;
    *x2 = color.u ;
    *x3 = color.v ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::Yxy>(const ColorSpace::Yxy& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.y1 ;
    *x2 = color.x ;
    *x3 = color.y2 ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::Cmy>(const ColorSpace::Cmy& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.c ;
    *x2 = color.m ;
    *x3 = color.y ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::Cmyk>(const ColorSpace::Cmyk& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.c ;
    *x2 = color.m ;
    *x3 = color.y ;
    *x4 = color.k ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
    *x4 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::Hsv>(const ColorSpace::Hsv& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.h ;
    *x2 = color.s ;
    *x3 = color.v ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::Hsb>(const ColorSpace::Hsb& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.h ;
    *x2 = color.s ;
    *x3 = color.b ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::HunterLab>(const ColorSpace::HunterLab& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.l ;
    *x2 = color.a ;
    *x3 = color.b ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::Hcl>(const ColorSpace::Hcl& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.h ;
    *x2 = color.c ;
    *x3 = color.l ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::OkLab>(const ColorSpace::OkLab& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.l ;
    *x2 = color.a ;
    *x3 = color.b ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

template <>
inline void grab<ColorSpace::OkLch>(const ColorSpace::OkLch& color, double* x1, double* x2, double* x3, double* x4){
  if (color.valid) {
    *x1 = color.l ;
    *x2 = color.c ;
    *x3 = color.h ;
  } else {
    *x1 = R_NaReal ;
    *x2 = R_NaReal ;
    *x3 = R_NaReal ;
  }
}

#endif
