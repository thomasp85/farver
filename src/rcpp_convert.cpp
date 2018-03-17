#include <Rcpp.h>
#include "ColorSpace.h"
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
NumericMatrix convert_dispatch_impl( const NumericMatrix& colour ){
  
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
    fill_rgb<Space_From>( colour(i,_), &rgb) ; 
    
    // ... convert the color to the `to` color space
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
NumericMatrix convert_dispatch_to( const NumericMatrix& colour, int to ){
  switch(to){
    case CMY: return convert_dispatch_impl<From, ColorSpace::Cmy>( colour ) ; 
    case CMYK: return convert_dispatch_impl<From, ColorSpace::Cmyk>( colour ) ; 
    case HSL: return convert_dispatch_impl<From, ColorSpace::Hsl>( colour ) ; 
    case HSB: return convert_dispatch_impl<From, ColorSpace::Hsb>( colour ) ; 
    case HSV: return convert_dispatch_impl<From, ColorSpace::Hsv>( colour ) ; 
    case LAB: return convert_dispatch_impl<From, ColorSpace::Lab>( colour ) ; 
    case HUNTERLAB: return convert_dispatch_impl<From, ColorSpace::HunterLab>( colour ) ; 
    case LCH: return convert_dispatch_impl<From, ColorSpace::Lch>( colour ) ; 
    case LUV: return convert_dispatch_impl<From, ColorSpace::Luv>( colour ) ; 
    case RGB: return convert_dispatch_impl<From, ColorSpace::Rgb>( colour ) ; 
    case XYZ: return convert_dispatch_impl<From, ColorSpace::Xyz>( colour ) ; 
    case YXY: return convert_dispatch_impl<From, ColorSpace::Yxy>( colour ) ; 
  }
  
  // never happens
  return colour ;
}

// same trick, but for the `from` 
NumericMatrix convert_dispatch_from( const NumericMatrix& colour, int from, int to){
  switch(from){
    case CMY: return convert_dispatch_to<ColorSpace::Cmy>( colour, to ) ; 
    case CMYK: return convert_dispatch_to<ColorSpace::Cmyk>( colour, to ) ; 
    case HSL: return convert_dispatch_to<ColorSpace::Hsl>( colour, to ) ; 
    case HSB: return convert_dispatch_to<ColorSpace::Hsb>( colour, to ) ; 
    case HSV: return convert_dispatch_to<ColorSpace::Hsv>( colour, to ) ; 
    case LAB: return convert_dispatch_to<ColorSpace::Lab>( colour, to ) ; 
    case HUNTERLAB: return convert_dispatch_to<ColorSpace::HunterLab>( colour, to ) ; 
    case LCH: return convert_dispatch_to<ColorSpace::Lch>( colour, to ) ; 
    case LUV: return convert_dispatch_to<ColorSpace::Luv>( colour, to ) ; 
    case RGB: return convert_dispatch_to<ColorSpace::Rgb>( colour, to ) ; 
    case XYZ: return convert_dispatch_to<ColorSpace::Xyz>( colour, to ) ; 
    case YXY: return convert_dispatch_to<ColorSpace::Yxy>( colour, to ) ; 
  }
  
  // never happens so we just return the input to quiet the compiler
  return colour ;
}

//[[Rcpp::export]]
NumericMatrix convert_c(NumericMatrix colour, int from, int to) {
  return convert_dispatch_from(colour, from, to) ;
}
