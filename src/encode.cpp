#include "encode.h"
#include "ColorSpace.h"
#include "farver.h"

static char hex8[] = "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F606162636465666768696A6B6C6D6E6F707172737475767778797A7B7C7D7E7F808182838485868788898A8B8C8D8E8F909192939495969798999A9B9C9D9E9FA0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBCBDBEBFC0C1C2C3C4C5C6C7C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8D9DADBDCDDDEDFE0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";
static char buffer[] = "#000000";
static char buffera[] = "#00000000";

inline int double2int(double d) {
  d += 6755399441055744.0;
  return reinterpret_cast<int&>(d);
}

template <typename From>
SEXP encode_impl(SEXP colour, SEXP alpha, SEXP white) {
  if (Rf_ncols(colour) < 3) {
    Rf_error("Colour in this format must contain at least 3 columns");
  }
  static ColorSpace::Rgb rgb;
  ColorSpace::XyzConverter::SetWhiteReference(REAL(white)[0], REAL(white)[1], REAL(white)[2]);
  int n = Rf_nrows(colour);
  SEXP codes = PROTECT(Rf_allocVector(STRSXP, n));
  bool has_alpha = !Rf_isNull(alpha);
  char alpha1, alpha2;
  bool alpha_is_int, one_alpha;
  char* buf;
  int* alpha_i;
  double* alpha_d;
  if (has_alpha) {
    buf = buffera;
    alpha_is_int = Rf_isInteger(alpha);
    one_alpha = LENGTH(alpha) == 1;
    int first_alpha;
    if (alpha_is_int) {
      alpha_i = INTEGER(alpha);
      first_alpha = alpha_i[0] * 2;
    } else {
      alpha_d = REAL(alpha);
      first_alpha = double2int(alpha_d[0]) * 2;
    }
    alpha1 = hex8[first_alpha];
    alpha2 = hex8[first_alpha + 1];
  } else {
    buf = buffer;
  }
  int offset1 = 0;
  int offset2 = offset1 + n;
  int offset3 = offset2 + n;
  
  int* colour_i;
  double* colour_d;
  bool colour_is_int = Rf_isInteger(colour);
  int num;
  if (colour_is_int) {
    colour_i = INTEGER(colour);
  } else {
    colour_d = REAL(colour);
  }
  for (int i = 0; i < n; ++i) {
    if (colour_is_int) {
      From(colour_i[offset1 + i], colour_i[offset2 + i], colour_i[offset3 + i]).ToRgb(&rgb);
    } else {
      From(colour_d[offset1 + i], colour_d[offset2 + i], colour_d[offset3 + i]).ToRgb(&rgb);
    }
    num = double2int(rgb.r);
    num = num < 0 ? 0 : (num > 255 ? 255 : num);
    num *= 2;
    buf[1] = hex8[num];
    buf[2] = hex8[num + 1];
    
    num = double2int(rgb.g);
    num = num < 0 ? 0 : (num > 255 ? 255 : num);
    num *= 2;
    buf[3] = hex8[num];
    buf[4] = hex8[num + 1];
    
    num = double2int(rgb.b);
    num = num < 0 ? 0 : (num > 255 ? 255 : num);
    num *= 2;
    buf[5] = hex8[num];
    buf[6] = hex8[num + 1];
    
    if (has_alpha) {
      if (one_alpha) {
        buf[7] = alpha1;
        buf[8] = alpha2;
      } else {
        if (alpha_is_int) {
          num = alpha_i[i];
        } else {
          num = double2int(alpha_d[i]);
        }
        num = num < 0 ? 0 : (num > 255 ? 255 : num);
        num *= 2;
        buf[7] = hex8[num];
        buf[8] = hex8[num + 1];
      }
    }
    
    SET_STRING_ELT(codes, i, Rf_mkChar(buf));
  }
  
  UNPROTECT(1);
  return codes;
}

template<>
SEXP encode_impl<ColorSpace::Rgb>(SEXP colour, SEXP alpha, SEXP white) {
  if (Rf_ncols(colour) < 3) {
    Rf_error("Colour in RGB format must contain at least 3 columns");
  }
  int n = Rf_nrows(colour);
  SEXP codes = PROTECT(Rf_allocVector(STRSXP, n));
  bool has_alpha = !Rf_isNull(alpha);
  char alpha1, alpha2;
  bool alpha_is_int, one_alpha;
  char* buf;
  int* alpha_i;
  double* alpha_d;
  if (has_alpha) {
    buf = buffera;
    alpha_is_int = Rf_isInteger(alpha);
    one_alpha = LENGTH(alpha) == 1;
    int first_alpha;
    if (alpha_is_int) {
      alpha_i = INTEGER(alpha);
      first_alpha = alpha_i[0] * 2;
    } else {
      alpha_d = REAL(alpha);
      first_alpha = double2int(alpha_d[0]) * 2;
    }
    alpha1 = hex8[first_alpha];
    alpha2 = hex8[first_alpha + 1];
  } else {
    buf = buffer;
  }
  int offset1 = 0;
  int offset2 = offset1 + n;
  int offset3 = offset2 + n;
  
  int* colour_i;
  double* colour_d;
  bool colour_is_int = Rf_isInteger(colour);
  int num;
  if (colour_is_int) {
    colour_i = INTEGER(colour);
    for (int i = 0; i < n; ++i) {
      num = colour_i[offset1 + i];
      num = num < 0 ? 0 : (num > 255 ? 255 : num);
      num *= 2;
      buf[1] = hex8[num];
      buf[2] = hex8[num + 1];
      
      num = colour_i[offset2 + i];
      num = num < 0 ? 0 : (num > 255 ? 255 : num);
      num *= 2;
      buf[3] = hex8[num];
      buf[4] = hex8[num + 1];
      
      num = colour_i[offset3 + i];
      num = num < 0 ? 0 : (num > 255 ? 255 : num);
      num *= 2;
      buf[5] = hex8[num];
      buf[6] = hex8[num + 1];
      
      if (has_alpha) {
        if (one_alpha) {
          buf[7] = alpha1;
          buf[8] = alpha2;
        } else {
          if (alpha_is_int) {
            num = alpha_i[i];
          } else {
            num = double2int(alpha_d[i]);
          }
          num = num < 0 ? 0 : (num > 255 ? 255 : num);
          num *= 2;
          buf[7] = hex8[num];
          buf[8] = hex8[num + 1];
        }
      }
      
      SET_STRING_ELT(codes, i, Rf_mkChar(buf));
    }
  } else {
    colour_d = REAL(colour);
    for (int i = 0; i < n; ++i) {
      num = double2int(colour_d[offset1 + i]);
      num = num < 0 ? 0 : (num > 255 ? 255 : num);
      num *= 2;
      buf[1] = hex8[num];
      buf[2] = hex8[num + 1];
      
      num = double2int(colour_d[offset2 + i]);
      num = num < 0 ? 0 : (num > 255 ? 255 : num);
      num *= 2;
      buf[3] = hex8[num];
      buf[4] = hex8[num + 1];
      
      num = double2int(colour_d[offset3 + i]);
      num = num < 0 ? 0 : (num > 255 ? 255 : num);
      num *= 2;
      buf[5] = hex8[num];
      buf[6] = hex8[num + 1];
      
      if (has_alpha) {
        if (one_alpha) {
          buf[7] = alpha1;
          buf[8] = alpha2;
        } else {
          if (alpha_is_int) {
            num = alpha_i[i];
          } else {
            num = double2int(alpha_d[i]);
          }
          num = num < 0 ? 0 : (num > 255 ? 255 : num);
          num *= 2;
          buf[7] = hex8[num];
          buf[8] = hex8[num + 1];
        }
      }
      
      SET_STRING_ELT(codes, i, Rf_mkChar(buf));
    }
  }
  
  UNPROTECT(1);
  return codes;
}

template <>
SEXP encode_impl<ColorSpace::Cmyk>(SEXP colour, SEXP alpha, SEXP white) {
  if (Rf_ncols(colour) < 4) {
    Rf_error("Colour in CMYK format must contain at least 3 columns");
  }
  static ColorSpace::Rgb rgb;
  ColorSpace::XyzConverter::SetWhiteReference(REAL(white)[0], REAL(white)[1], REAL(white)[2]);
  int n = Rf_nrows(colour);
  SEXP codes = PROTECT(Rf_allocVector(STRSXP, n));
  bool has_alpha = !Rf_isNull(alpha);
  char alpha1, alpha2;
  bool alpha_is_int, one_alpha;
  char* buf;
  int* alpha_i;
  double* alpha_d;
  if (has_alpha) {
    buf = buffera;
    alpha_is_int = Rf_isInteger(alpha);
    one_alpha = LENGTH(alpha) == 1;
    int first_alpha;
    if (alpha_is_int) {
      alpha_i = INTEGER(alpha);
      first_alpha = alpha_i[0] * 2;
    } else {
      alpha_d = REAL(alpha);
      first_alpha = double2int(alpha_d[0]) * 2;
    }
    alpha1 = hex8[first_alpha];
    alpha2 = hex8[first_alpha + 1];
  } else {
    buf = buffer;
  }
  int offset1 = 0;
  int offset2 = offset1 + n;
  int offset3 = offset2 + n;
  int offset4 = offset3 + n;
  
  int* colour_i;
  double* colour_d;
  bool colour_is_int = Rf_isInteger(colour);
  int num;
  if (colour_is_int) {
    colour_i = INTEGER(colour);
  } else {
    colour_d = REAL(colour);
  }
  for (int i = 0; i < n; ++i) {
    if (colour_is_int) {
      ColorSpace::Cmyk(colour_i[offset1 + i], colour_i[offset2 + i], colour_i[offset3 + i], colour_i[offset4 + i]).ToRgb(&rgb);
    } else {
      ColorSpace::Cmyk(colour_d[offset1 + i], colour_d[offset2 + i], colour_d[offset3 + i], colour_d[offset4 + i]).ToRgb(&rgb);
    }
    num = double2int(rgb.r);
    num = num < 0 ? 0 : (num > 255 ? 255 : num);
    num *= 2;
    buf[1] = hex8[num];
    buf[2] = hex8[num + 1];
    
    num = double2int(rgb.g);
    num = num < 0 ? 0 : (num > 255 ? 255 : num);
    num *= 2;
    buf[3] = hex8[num];
    buf[4] = hex8[num + 1];
    
    num = double2int(rgb.b);
    num = num < 0 ? 0 : (num > 255 ? 255 : num);
    num *= 2;
    buf[5] = hex8[num];
    buf[6] = hex8[num + 1];
    
    if (has_alpha) {
      if (one_alpha) {
        buf[7] = alpha1;
        buf[8] = alpha2;
      } else {
        if (alpha_is_int) {
          num = alpha_i[i];
        } else {
          num = double2int(alpha_d[i]);
        }
        num = num < 0 ? 0 : (num > 255 ? 255 : num);
        num *= 2;
        buf[7] = hex8[num];
        buf[8] = hex8[num + 1];
      }
    }
    
    SET_STRING_ELT(codes, i, Rf_mkChar(buf));
  }
  
  UNPROTECT(1);
  return codes;
}

SEXP encode_c(SEXP colour, SEXP alpha, SEXP from, SEXP white) {
  switch (INTEGER(from)[0]) {
    case CMY: return encode_impl<ColorSpace::Cmy>(colour, alpha, white);
    case CMYK: return encode_impl<ColorSpace::Cmyk>(colour, alpha, white);
    case HSL: return encode_impl<ColorSpace::Hsl>(colour, alpha, white);
    case HSB: return encode_impl<ColorSpace::Hsb>(colour, alpha, white);
    case HSV: return encode_impl<ColorSpace::Hsv>(colour, alpha, white);
    case LAB: return encode_impl<ColorSpace::Lab>(colour, alpha, white);
    case HUNTERLAB: return encode_impl<ColorSpace::HunterLab>(colour, alpha, white);
    case LCH: return encode_impl<ColorSpace::Lch>(colour, alpha, white);
    case LUV: return encode_impl<ColorSpace::Luv>(colour, alpha, white);
    case RGB: return encode_impl<ColorSpace::Rgb>(colour, alpha, white);
    case XYZ: return encode_impl<ColorSpace::Xyz>(colour, alpha, white);
    case YXY: return encode_impl<ColorSpace::Yxy>(colour, alpha, white);
    case HCL: return encode_impl<ColorSpace::Hcl>(colour, alpha, white);
  }
  
  // never happens
  return R_NilValue;
}
