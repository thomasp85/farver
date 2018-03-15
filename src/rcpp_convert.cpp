#include <Rcpp.h>
#include "ColorSpace.h"
#include <memory.h>

using namespace Rcpp;

//[[Rcpp::export]]
NumericMatrix convert_c(NumericMatrix colour, std::string from, std::string to) {
  if (from == "cmyk") {
    if (colour.ncol() != 4) {
      stop("colour space requires four values");
    }
  } else if (colour.ncol() != 3) {
    stop("colour space requires three values");
  }
  int outdim = to == "cmyk" ? 4 : 3;
  int i;
  NumericMatrix res(colour.nrow(), outdim);
  
  std::unique_ptr<ColorSpace::IColorSpace> from_c;

  for (i = 0; i < colour.nrow(); ++i) {
    if (from == "rgb") {
      from_c.reset(new ColorSpace::Rgb(colour(i, 0), colour(i, 1), colour(i, 2)));
    } else if (from == "xyz") {
      from_c.reset(new ColorSpace::Xyz(colour(i, 0), colour(i, 1), colour(i, 2)));
    } else if (from == "hsl") {
      from_c.reset(new ColorSpace::Hsl(colour(i, 0), colour(i, 1), colour(i, 2)));
    } else if (from == "lab") {
      from_c.reset(new ColorSpace::Lab(colour(i, 0), colour(i, 1), colour(i, 2)));
    } else if (from == "lch") {
      from_c.reset(new ColorSpace::Lch(colour(i, 0), colour(i, 1), colour(i, 2)));
    } else if (from == "luv") {
      from_c.reset(new ColorSpace::Luv(colour(i, 0), colour(i, 1), colour(i, 2)));
    } else if (from == "yxy") {
      from_c.reset(new ColorSpace::Yxy(colour(i, 0), colour(i, 1), colour(i, 2)));
    } else if (from == "cmy") {
      from_c.reset(new ColorSpace::Cmy(colour(i, 0), colour(i, 1), colour(i, 2)));
    } else if (from == "cmyk") {
      from_c.reset(new ColorSpace::Cmyk(colour(i, 0), colour(i, 1), colour(i, 2), colour(i, 3)));
    } else if (from == "hsv") {
      from_c.reset(new ColorSpace::Hsv(colour(i, 0), colour(i, 1), colour(i, 2)));
    } else if (from == "hsb") {
      from_c.reset(new ColorSpace::Hsb(colour(i, 0), colour(i, 1), colour(i, 2)));
    } else if (from == "hunterlab") {
      from_c.reset(new ColorSpace::HunterLab(colour(i, 0), colour(i, 1), colour(i, 2)));
    } else {
      stop("Unknown colour space");
    }

    if (to == "rgb") {
      ColorSpace::Rgb to_c;
      from_c->To<ColorSpace::Rgb>(&to_c);
      res(i, 0) = to_c.r;
      res(i, 1) = to_c.g;
      res(i, 2) = to_c.b;
    } else if (to == "xyz") {
      ColorSpace::Xyz to_c;
      from_c->To<ColorSpace::Xyz>(&to_c);
      res(i, 0) = to_c.x;
      res(i, 1) = to_c.y;
      res(i, 2) = to_c.z;
    } else if (to == "hsl") {
      ColorSpace::Hsl to_c;
      from_c->To<ColorSpace::Hsl>(&to_c);
      res(i, 0) = to_c.h;
      res(i, 1) = to_c.s;
      res(i, 2) = to_c.l;
    } else if (to == "lab") {
      ColorSpace::Lab to_c;
      from_c->To<ColorSpace::Lab>(&to_c);
      res(i, 0) = to_c.l;
      res(i, 1) = to_c.a;
      res(i, 2) = to_c.b;
    } else if (to == "lch") {
      ColorSpace::Lch to_c;
      from_c->To<ColorSpace::Lch>(&to_c);
      res(i, 0) = to_c.l;
      res(i, 1) = to_c.c;
      res(i, 2) = to_c.h;
    } else if (to == "luv") {
      ColorSpace::Luv to_c;
      from_c->To<ColorSpace::Luv>(&to_c);
      res(i, 0) = to_c.l;
      res(i, 1) = to_c.u;
      res(i, 2) = to_c.v;
    } else if (to == "yxy") {
      ColorSpace::Yxy to_c;
      from_c->To<ColorSpace::Yxy>(&to_c);
      res(i, 0) = to_c.y1;
      res(i, 1) = to_c.x;
      res(i, 2) = to_c.y2;
    } else if (to == "cmy") {
      ColorSpace::Cmy to_c;
      from_c->To<ColorSpace::Cmy>(&to_c);
      res(i, 0) = to_c.c;
      res(i, 1) = to_c.m;
      res(i, 2) = to_c.y;
    } else if (to == "cmyk") {
      ColorSpace::Cmyk to_c;
      from_c->To<ColorSpace::Cmyk>(&to_c);
      res(i, 0) = to_c.c;
      res(i, 1) = to_c.m;
      res(i, 2) = to_c.y;
      res(i, 3) = to_c.k;
    } else if (to == "hsv") {
      ColorSpace::Hsv to_c;
      from_c->To<ColorSpace::Hsv>(&to_c);
      res(i, 0) = to_c.h;
      res(i, 1) = to_c.s;
      res(i, 2) = to_c.v;
    } else if (to == "hsb") {
      ColorSpace::Hsb to_c;
      from_c->To<ColorSpace::Hsb>(&to_c);
      res(i, 0) = to_c.h;
      res(i, 1) = to_c.s;
      res(i, 2) = to_c.b;
    } else if (to == "hunterlab") {
      ColorSpace::HunterLab to_c;
      from_c->To<ColorSpace::HunterLab>(&to_c);
      res(i, 0) = to_c.l;
      res(i, 1) = to_c.a;
      res(i, 2) = to_c.b;
    } else {
      stop("Unknowm colour space");
    }
  }
  
  return res;
}
