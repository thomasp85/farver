#include <Rcpp.h>
#include "ColorSpace.h"
#include "Comparison.h"
#include <memory>

using namespace Rcpp;

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
void fill_rgb( NumericMatrix::ConstRow row, ColorSpace::Rgb* rgb){
  Space( row[0], row[1], row[2] ).ToRgb(rgb) ;
}
template <>
void fill_rgb<ColorSpace::Cmyk>( NumericMatrix::ConstRow row, ColorSpace::Rgb* rgb){
  ColorSpace::Cmyk( row[0], row[1], row[2], row[3] ).ToRgb(rgb) ;
}

// these grab values from the Space type and use them to fill `row`
// unfortunately, given how the `ColorSpace` C++ library is written, 
// this has to do lots of special casing
template <typename Space>
void grab( NumericMatrix::Row row, const Space& ) ;

template <>
void grab<ColorSpace::Rgb>( NumericMatrix::Row row, const ColorSpace::Rgb& color){
  row[0] = color.r ;
  row[1] = color.g ;
  row[2] = color.b ;
}

template <>
void grab<ColorSpace::Xyz>( NumericMatrix::Row row, const ColorSpace::Xyz& color){
  row[0] = color.x ;
  row[1] = color.y ;
  row[2] = color.z ;
}

template <>
void grab<ColorSpace::Hsl>( NumericMatrix::Row row, const ColorSpace::Hsl& color){
  row[0] = color.h ;
  row[1] = color.s ;
  row[2] = color.l ;
}

template <>
void grab<ColorSpace::Lab>( NumericMatrix::Row row, const ColorSpace::Lab& color){
  row[0] = color.l ;
  row[1] = color.a ;
  row[2] = color.b ;
}

template <>
void grab<ColorSpace::Lch>( NumericMatrix::Row row, const ColorSpace::Lch& color){
  row[0] = color.l ;
  row[1] = color.c ;
  row[2] = color.h ;
}

template <>
void grab<ColorSpace::Luv>( NumericMatrix::Row row, const ColorSpace::Luv& color){
  row[0] = color.l ;
  row[1] = color.u ;
  row[2] = color.v ;
}

template <>
void grab<ColorSpace::Yxy>( NumericMatrix::Row row, const ColorSpace::Yxy& color){
  row[0] = color.y1 ;
  row[1] = color.x ;
  row[2] = color.y2 ;
}

template <>
void grab<ColorSpace::Cmy>( NumericMatrix::Row row, const ColorSpace::Cmy& color){
  row[0] = color.c ;
  row[1] = color.m ;
  row[2] = color.y ;
}

template <>
void grab<ColorSpace::Cmyk>( NumericMatrix::Row row, const ColorSpace::Cmyk& color){
  row[0] = color.c ;
  row[1] = color.m ;
  row[2] = color.y ;
  row[3] = color.k ;
}

template <>
void grab<ColorSpace::Hsv>( NumericMatrix::Row row, const ColorSpace::Hsv& color){
  row[0] = color.h ;
  row[1] = color.s ;
  row[2] = color.v ;
}

template <>
void grab<ColorSpace::Hsb>( NumericMatrix::Row row, const ColorSpace::Hsb& color){
  row[0] = color.h ;
  row[1] = color.s ;
  row[2] = color.b ;
}

template <>
void grab<ColorSpace::HunterLab>( NumericMatrix::Row row, const ColorSpace::HunterLab& color){
  row[0] = color.l ;
  row[1] = color.a ;
  row[2] = color.b ;
}

// this is where the real work happens, 
// the template parameters Space_From and Space_To give us the types of the 
// from and to colours. 
template <typename Space_From, typename Space_To>
NumericMatrix convert_dispatch_impl( const NumericMatrix& colour, const NumericVector& white_from, const NumericVector& white_to ){
  
  // check that the dimensions of the input match the colour space
  if( dimension<Space_From>() != colour.ncol() ){
    stop("colourspace requires %d values", dimension<Space_From>() ) ;    
  }
  
  // make the result matrix
  int n = colour.nrow() ;
  NumericMatrix out(n, dimension<Space_To>() ) ;
  
  ColorSpace::Rgb rgb ;
  Space_To to ;
  
  for( int i=0; i<n; i++){
    // fill `rgb` based on the input color in the `from` color space
    ColorSpace::XyzConverter::whiteReference = {white_from[0], white_from[1], white_from[2]};
    fill_rgb<Space_From>( colour(i,_), &rgb) ; 
    
    // ... convert the color to the `to` color space
    ColorSpace::XyzConverter::whiteReference = {white_to[0], white_to[1], white_to[2]};
    ColorSpace::IConverter<Space_To>::ToColorSpace(&rgb, &to) ;
    
    // ... and move it to the row of the `out` matrix
    grab<Space_To>( out(i,_), to ) ;
  }
  
  return out ;
  
}

// these are used below in the dispatcher functions
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

// this is a trick to do a runtime fake compile time dispatch
// the idea is to call the right instantiation of the convert_dispatch_impl template 
// where the real work happens, based on `to`
template <typename From>
NumericMatrix convert_dispatch_to( const NumericMatrix& colour, int to, const NumericVector& white_from, const NumericVector& white_to ){
  switch(to){
    case CMY: return convert_dispatch_impl<From, ColorSpace::Cmy>( colour, white_from, white_to ) ; 
    case CMYK: return convert_dispatch_impl<From, ColorSpace::Cmyk>( colour, white_from, white_to ) ; 
    case HSL: return convert_dispatch_impl<From, ColorSpace::Hsl>( colour, white_from, white_to ) ; 
    case HSB: return convert_dispatch_impl<From, ColorSpace::Hsb>( colour, white_from, white_to ) ; 
    case HSV: return convert_dispatch_impl<From, ColorSpace::Hsv>( colour, white_from, white_to ) ; 
    case LAB: return convert_dispatch_impl<From, ColorSpace::Lab>( colour, white_from, white_to ) ; 
    case HUNTERLAB: return convert_dispatch_impl<From, ColorSpace::HunterLab>( colour, white_from, white_to ) ; 
    case LCH: return convert_dispatch_impl<From, ColorSpace::Lch>( colour, white_from, white_to ) ; 
    case LUV: return convert_dispatch_impl<From, ColorSpace::Luv>( colour, white_from, white_to ) ; 
    case RGB: return convert_dispatch_impl<From, ColorSpace::Rgb>( colour, white_from, white_to ) ; 
    case XYZ: return convert_dispatch_impl<From, ColorSpace::Xyz>( colour, white_from, white_to ) ; 
    case YXY: return convert_dispatch_impl<From, ColorSpace::Yxy>( colour, white_from, white_to ) ; 
  }
  
  // never happens
  return colour ;
}

// same trick, but for the `from` 
NumericMatrix convert_dispatch_from( const NumericMatrix& colour, int from, int to, const NumericVector& white_from, const NumericVector& white_to){
  switch(from){
    case CMY: return convert_dispatch_to<ColorSpace::Cmy>( colour, to, white_from, white_to ) ; 
    case CMYK: return convert_dispatch_to<ColorSpace::Cmyk>( colour, to, white_from, white_to ) ; 
    case HSL: return convert_dispatch_to<ColorSpace::Hsl>( colour, to, white_from, white_to ) ; 
    case HSB: return convert_dispatch_to<ColorSpace::Hsb>( colour, to, white_from, white_to ) ; 
    case HSV: return convert_dispatch_to<ColorSpace::Hsv>( colour, to, white_from, white_to ) ; 
    case LAB: return convert_dispatch_to<ColorSpace::Lab>( colour, to, white_from, white_to ) ; 
    case HUNTERLAB: return convert_dispatch_to<ColorSpace::HunterLab>( colour, to, white_from, white_to ) ; 
    case LCH: return convert_dispatch_to<ColorSpace::Lch>( colour, to, white_from, white_to ) ; 
    case LUV: return convert_dispatch_to<ColorSpace::Luv>( colour, to, white_from, white_to ) ; 
    case RGB: return convert_dispatch_to<ColorSpace::Rgb>( colour, to, white_from, white_to ) ; 
    case XYZ: return convert_dispatch_to<ColorSpace::Xyz>( colour, to, white_from, white_to ) ; 
    case YXY: return convert_dispatch_to<ColorSpace::Yxy>( colour, to, white_from, white_to ) ; 
  }
  
  // never happens so we just return the input to quiet the compiler
  return colour ;
}

//[[Rcpp::export]]
NumericMatrix convert_c(NumericMatrix colour, int from, int to, NumericVector white_from, NumericVector white_to) {
  return convert_dispatch_from(colour, from, to, white_from, white_to) ;
}

#define EUCLIDEAN 1
#define CIE1976 2
#define CIE94 3
#define CIE2000 4
#define CMC 5

double get_colour_dist(ColorSpace::Rgb& from, ColorSpace::Rgb& to, int dist) {
  switch(dist){
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
NumericMatrix compare_dispatch_impl(const NumericMatrix& from, const NumericMatrix& to, int dist, bool sym, const NumericVector& white_from, const NumericVector& white_to){
  
  // check that the dimensions of the input match the colour space
  if( dimension<Space_From>() != from.ncol() ){
    stop("colourspace requires %d values", dimension<Space_From>() ) ;    
  }
  if( dimension<Space_To>() != to.ncol() ){
    stop("colourspace requires %d values", dimension<Space_From>() ) ;    
  }
  int n = from.nrow(), m = to.nrow(), i, j;
  NumericMatrix out(n, m);
  
  ColorSpace::Rgb from_rgb, to_rgb;
  
  for (i = 0; i < n; ++i) {
    ColorSpace::XyzConverter::whiteReference = {white_from[0], white_from[1], white_from[2]};
    fill_rgb<Space_From>(from(i, _), &from_rgb);
    ColorSpace::XyzConverter::whiteReference = {white_to[0], white_to[1], white_to[2]};
    for (j = sym ? i + 1 : 0; j < m; ++j) {
      fill_rgb<Space_To>(to(j, _), &to_rgb);
      out(i, j) = get_colour_dist(from_rgb, to_rgb, dist);
    }
  }
  
  return out ;
}

template <typename From>
NumericMatrix compare_dispatch_to(const NumericMatrix& from, const NumericMatrix& to, int to_space, int dist, bool sym, const NumericVector& white_from, const NumericVector& white_to) {
  switch(to_space){
    case CMY: return compare_dispatch_impl<From, ColorSpace::Cmy>(from, to, dist, sym, white_from, white_to) ; 
    case CMYK: return compare_dispatch_impl<From, ColorSpace::Cmyk>(from, to, dist, sym, white_from, white_to) ; 
    case HSL: return compare_dispatch_impl<From, ColorSpace::Hsl>(from, to, dist, sym, white_from, white_to) ; 
    case HSB: return compare_dispatch_impl<From, ColorSpace::Hsb>(from, to, dist, sym, white_from, white_to) ; 
    case HSV: return compare_dispatch_impl<From, ColorSpace::Hsv>(from, to, dist, sym, white_from, white_to) ; 
    case LAB: return compare_dispatch_impl<From, ColorSpace::Lab>(from, to, dist, sym, white_from, white_to) ; 
    case HUNTERLAB: return compare_dispatch_impl<From, ColorSpace::HunterLab>(from, to, dist, sym, white_from, white_to) ; 
    case LCH: return compare_dispatch_impl<From, ColorSpace::Lch>(from, to, dist, sym, white_from, white_to) ; 
    case LUV: return compare_dispatch_impl<From, ColorSpace::Luv>(from, to, dist, sym, white_from, white_to) ; 
    case RGB: return compare_dispatch_impl<From, ColorSpace::Rgb>(from, to, dist, sym, white_from, white_to) ; 
    case XYZ: return compare_dispatch_impl<From, ColorSpace::Xyz>(from, to, dist, sym, white_from, white_to) ; 
    case YXY: return compare_dispatch_impl<From, ColorSpace::Yxy>(from, to, dist, sym, white_from, white_to) ; 
  }
  
  // never happens
  return from ;
}

NumericMatrix compare_dispatch_from(const NumericMatrix& from, const NumericMatrix& to, int from_space, int to_space, int dist, bool sym, const NumericVector& white_from, const NumericVector& white_to) {
  switch(from_space){
    case CMY: return compare_dispatch_to<ColorSpace::Cmy>(from, to, to_space, dist, sym, white_from, white_to) ; 
    case CMYK: return compare_dispatch_to<ColorSpace::Cmyk>(from, to, to_space, dist, sym, white_from, white_to) ; 
    case HSL: return compare_dispatch_to<ColorSpace::Hsl>(from, to, to_space, dist, sym, white_from, white_to) ; 
    case HSB: return compare_dispatch_to<ColorSpace::Hsb>(from, to, to_space, dist, sym, white_from, white_to) ; 
    case HSV: return compare_dispatch_to<ColorSpace::Hsv>(from, to, to_space, dist, sym, white_from, white_to) ; 
    case LAB: return compare_dispatch_to<ColorSpace::Lab>(from, to, to_space, dist, sym, white_from, white_to) ; 
    case HUNTERLAB: return compare_dispatch_to<ColorSpace::HunterLab>(from, to, to_space, dist, sym, white_from, white_to) ; 
    case LCH: return compare_dispatch_to<ColorSpace::Lch>(from, to, to_space, dist, sym, white_from, white_to) ; 
    case LUV: return compare_dispatch_to<ColorSpace::Luv>(from, to, to_space, dist, sym, white_from, white_to) ; 
    case RGB: return compare_dispatch_to<ColorSpace::Rgb>(from, to, to_space, dist, sym, white_from, white_to) ; 
    case XYZ: return compare_dispatch_to<ColorSpace::Xyz>(from, to, to_space, dist, sym, white_from, white_to) ; 
    case YXY: return compare_dispatch_to<ColorSpace::Yxy>(from, to, to_space, dist, sym, white_from, white_to) ; 
  }
  
  // never happens so we just return the input to quiet the compiler
  return from ;
}

//[[Rcpp::export]]
NumericMatrix compare_c(NumericMatrix from, NumericMatrix to, int from_space, int to_space, int dist, bool sym, NumericVector white_from, NumericVector white_to) {
  return compare_dispatch_from(from, to, from_space, to_space, dist, sym, white_from, white_to);
}