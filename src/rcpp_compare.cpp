#include <Rcpp.h>
#include "ColorSpace.h"
#include "Comparison.h"
#include <memory>

using namespace Rcpp;

//[[Rcpp::export]]
NumericMatrix compare_c(NumericMatrix from, NumericMatrix to, std::string from_space, std::string to_space, std::string dist, bool sym) {
  if (from_space == "cmyk") {
    if (from.ncol() != 4) {
      stop("colour space requires four values");
    }
  } else if (from.ncol() != 3) {
    stop("colour space requires three values");
  }
  if (to_space == "cmyk") {
    if (to.ncol() != 4) {
      stop("colour space requires four values");
    }
  } else if (to.ncol() != 3) {
    stop("colour space requires three values");
  }
  NumericMatrix res(from.nrow(), to.nrow());
  std::unique_ptr<ColorSpace::IColorSpace> from_c, to_c;
  int i,j;
  
  for (i = 0; i < from.nrow(); ++i) {
    if (from_space == "rgb") {
      from_c.reset(new ColorSpace::Rgb(from(i, 0), from(i, 1), from(i, 2)));
    } else if (from_space == "xyz") {
      from_c.reset(new ColorSpace::Xyz(from(i, 0), from(i, 1), from(i, 2)));
    } else if (from_space == "hsl") {
      from_c.reset(new ColorSpace::Hsl(from(i, 0), from(i, 1), from(i, 2)));
    } else if (from_space == "lab") {
      from_c.reset(new ColorSpace::Lab(from(i, 0), from(i, 1), from(i, 2)));
    } else if (from_space == "lch") {
      from_c.reset(new ColorSpace::Lch(from(i, 0), from(i, 1), from(i, 2)));
    } else if (from_space == "luv") {
      from_c.reset(new ColorSpace::Luv(from(i, 0), from(i, 1), from(i, 2)));
    } else if (from_space == "yxy") {
      from_c.reset(new ColorSpace::Yxy(from(i, 0), from(i, 1), from(i, 2)));
    } else if (from_space == "cmy") {
      from_c.reset(new ColorSpace::Cmy(from(i, 0), from(i, 1), from(i, 2)));
    } else if (from_space == "cmyk") {
      from_c.reset(new ColorSpace::Cmyk(from(i, 0), from(i, 1), from(i, 2), from(i, 3)));
    } else if (from_space == "hsv") {
      from_c.reset(new ColorSpace::Hsv(from(i, 0), from(i, 1), from(i, 2)));
    } else if (from_space == "hsb") {
      from_c.reset(new ColorSpace::Hsb(from(i, 0), from(i, 1), from(i, 2)));
    } else if (from_space == "hunterlab") {
      from_c.reset(new ColorSpace::HunterLab(from(i, 0), from(i, 1), from(i, 2)));
    } else {
      stop("Unknown colour space");
    }
    
    for (j = sym ? i + 1 : 0; j < to.nrow(); ++j) {
      if (to_space == "rgb") {
        to_c.reset(new ColorSpace::Rgb(to(j, 0), to(j, 1), to(j, 2)));
      } else if (to_space == "xyz") {
        to_c.reset(new ColorSpace::Xyz(to(j, 0), to(j, 1), to(j, 2)));
      } else if (to_space == "hsl") {
        to_c.reset(new ColorSpace::Hsl(to(j, 0), to(j, 1), to(j, 2)));
      } else if (to_space == "lab") {
        to_c.reset(new ColorSpace::Lab(to(j, 0), to(j, 1), to(j, 2)));
      } else if (to_space == "lch") {
        to_c.reset(new ColorSpace::Lch(to(j, 0), to(j, 1), to(j, 2)));
      } else if (to_space == "luv") {
        to_c.reset(new ColorSpace::Luv(to(j, 0), to(j, 1), to(j, 2)));
      } else if (to_space == "yxy") {
        to_c.reset(new ColorSpace::Yxy(to(j, 0), to(j, 1), to(j, 2)));
      } else if (to_space == "cmy") {
        to_c.reset(new ColorSpace::Cmy(to(j, 0), to(j, 1), to(j, 2)));
      } else if (to_space == "cmyk") {
        to_c.reset(new ColorSpace::Cmyk(to(j, 0), to(j, 1), to(j, 2), to(j, 3)));
      } else if (to_space == "hsv") {
        to_c.reset(new ColorSpace::Hsv(to(j, 0), to(j, 1), to(j, 2)));
      } else if (to_space == "hsb") {
        to_c.reset(new ColorSpace::Hsb(to(j, 0), to(j, 1), to(j, 2)));
      } else if (to_space == "hunterlab") {
        to_c.reset(new ColorSpace::HunterLab(to(j, 0), to(j, 1), to(j, 2)));
      } else {
        stop("Unknown colour space");
      }
      
      if (dist == "euclidean") {
        res(i, j) = ColorSpace::EuclideanComparison::Compare(from_c.get(), to_c.get());
      } else if (dist == "cie1976") {
        res(i, j) = ColorSpace::Cie1976Comparison::Compare(from_c.get(), to_c.get());
      } else if (dist == "cie94") {
        res(i, j) = ColorSpace::Cie94Comparison::Compare(from_c.get(), to_c.get());
      } else if (dist == "cie2000") {
        res(i, j) = ColorSpace::Cie2000Comparison::Compare(from_c.get(), to_c.get());
      } else if (dist == "cmc") {
        res(i, j) = ColorSpace::CmcComparison::Compare(from_c.get(), to_c.get());
      } else {
        stop("Unknown distance function");
      }
    }
  }
  
  return res;
}