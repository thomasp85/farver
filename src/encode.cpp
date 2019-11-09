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
inline int cap0255(int x) {
  return x < 0 ? 0 : (x > 255 ? 255 : x);
}
inline int hex2int(const int x) {
  return (x & 0xf) + (x >> 6) + ((x >> 6) << 3);
}

template <typename From>
SEXP encode_impl(SEXP colour, SEXP alpha, SEXP white) {
  int n_channels = dimension<From>();
  if (Rf_ncols(colour) < n_channels) {
    Rf_error("Colour in this format must contain at least %i columns", n_channels);
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
      first_alpha = alpha_i[0];
      first_alpha = first_alpha == R_NaInt ? 255 : first_alpha;
    } else {
      alpha_d = REAL(alpha);
      if (!R_finite(alpha_d[0])) {
        first_alpha = 255;
      } else {
        first_alpha = double2int(alpha_d[0]);
      }
    }
    first_alpha = cap0255(first_alpha) * 2;
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
      fill_rgb<From>(&rgb, colour_i[offset1 + i], colour_i[offset2 + i], colour_i[offset3 + i], n_channels == 4 ? colour_i[offset4 + i] : 0);
    } else {
      fill_rgb<From>(&rgb, colour_d[offset1 + i], colour_d[offset2 + i], colour_d[offset3 + i], n_channels == 4 ? colour_d[offset4 + i] : 0.0);
    }
    if (!rgb.valid) {
      SET_STRING_ELT(codes, i, R_NaString);
      continue;
    }
    num = double2int(rgb.r);
    num = cap0255(num) * 2;
    buf[1] = hex8[num];
    buf[2] = hex8[num + 1];
    
    num = double2int(rgb.g);
    num = cap0255(num) * 2;
    buf[3] = hex8[num];
    buf[4] = hex8[num + 1];
    
    num = double2int(rgb.b);
    num = cap0255(num) * 2;
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
        num = cap0255(num) * 2;
        buf[7] = hex8[num];
        buf[8] = hex8[num + 1];
      }
    }
    
    
    SET_STRING_ELT(codes, i, Rf_mkChar(buf));
  }
  
  UNPROTECT(1);
  return copy_names(colour, codes);
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
      first_alpha = alpha_i[0];
      first_alpha = first_alpha == R_NaInt ? 255 : first_alpha;
    } else {
      alpha_d = REAL(alpha);
      if (!R_finite(alpha_d[0])) {
        first_alpha = 255;
      } else {
        first_alpha = double2int(alpha_d[0]);
      }
    }
    first_alpha = cap0255(first_alpha) * 2;
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
    int r, g, b;
    colour_i = INTEGER(colour);
    for (int i = 0; i < n; ++i) {
      r = colour_i[offset1 + i];
      g = colour_i[offset2 + i];
      b = colour_i[offset3 + i];
      if (r == R_NaInt || g == R_NaInt || b == R_NaInt) {
        SET_STRING_ELT(codes, i, R_NaString);
        continue;
      }
      num = cap0255(r) * 2;
      buf[1] = hex8[num];
      buf[2] = hex8[num + 1];
      
      num = cap0255(g) * 2;
      buf[3] = hex8[num];
      buf[4] = hex8[num + 1];
      
      num = cap0255(b) * 2;
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
          num = cap0255(num) * 2;
          buf[7] = hex8[num];
          buf[8] = hex8[num + 1];
        }
      }
      
      SET_STRING_ELT(codes, i, Rf_mkChar(buf));
    }
  } else {
    double r, g, b;
    colour_d = REAL(colour);
    for (int i = 0; i < n; ++i) {
      r = colour_d[offset1 + i];
      g = colour_d[offset2 + i];
      b = colour_d[offset3 + i];
      if (!(R_finite(r) && R_finite(g) && R_finite(b))) {
        SET_STRING_ELT(codes, i, R_NaString);
        continue;
      }
      num = cap0255(double2int(r)) * 2;
      buf[1] = hex8[num];
      buf[2] = hex8[num + 1];
      
      num = cap0255(double2int(g)) * 2;
      buf[3] = hex8[num];
      buf[4] = hex8[num + 1];
      
      num = cap0255(double2int(b)) * 2;
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
          num = cap0255(num) * 2;
          buf[7] = hex8[num];
          buf[8] = hex8[num + 1];
        }
      }
      
      SET_STRING_ELT(codes, i, Rf_mkChar(buf));
    }
  }
  
  UNPROTECT(1);
  return copy_names(colour, codes);
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

SEXP load_colour_names_c(SEXP name, SEXP value) {
  ColourMap& named_colours = get_named_colours();
  int n = LENGTH(name);
  if (n != Rf_ncols(value)) {
    Rf_error("name and value must have the same length");
  }
  int* values = INTEGER(value);
  int it = 0;
  for (int i = 0; i < n; ++i) {
    std::string colour_name(Rf_translateCharUTF8(STRING_ELT(name, i)));
    rgb_colour col;
    col.r = values[it++];
    col.g = values[it++];
    col.b = values[it++];
    named_colours[colour_name] = col;
  }
  return R_NilValue;
}

template <typename To>
SEXP decode_impl(SEXP codes, SEXP alpha, SEXP white) {
  bool get_alpha = LOGICAL(alpha)[0];
  int n_channels = dimension<To>();
  int n_cols = get_alpha ? n_channels + 1 : n_channels;
  int n = LENGTH(codes);
  ColourMap& named_colours = get_named_colours();
  SEXP colours = PROTECT(Rf_allocMatrix(REALSXP, n, n_cols));
  double* colours_p = REAL(colours);
  int offset1 = 0;
  int offset2 = offset1 + n;
  int offset3 = offset2 + n;
  int offset4 = offset3 + n;
  int offset5 = offset4 + n;
  
  ColorSpace::Rgb rgb;
  ColorSpace::XyzConverter::SetWhiteReference(REAL(white)[0], REAL(white)[1], REAL(white)[2]);
  To to;
  
  int nchar;
  double a;
  bool has_alpha;
  for (int i = 0; i < n; ++i) {
    SEXP code = STRING_ELT(codes, i);
    if (code == R_NaString) {
      colours_p[offset1 + i] = R_NaReal;
      colours_p[offset2 + i] = R_NaReal;
      colours_p[offset3 + i] = R_NaReal;
      if (n_cols > 3) colours_p[offset4 + i] = R_NaReal;
      if (n_cols > 4) colours_p[offset5 + i] = R_NaReal;
      continue;
    }
    const char* col = Rf_translateCharUTF8(code);
    if (col[0] == '#') {
      nchar = strlen(col);
      has_alpha = nchar == 9;
      if (!has_alpha && nchar != 7) {
        Rf_error("Malformed colour string. Must contain either 6 or 8 hex values");
      }
      rgb.r = hex2int(col[1]) * 16 + hex2int(col[2]);
      rgb.g = hex2int(col[3]) * 16 + hex2int(col[4]);
      rgb.b = hex2int(col[5]) * 16 + hex2int(col[6]);
      if (has_alpha) {
        a = hex2int(col[7]) * 16 + hex2int(col[8]);
        a /= 255;
      } else {
        a = 1.0;
      }
    } else {
      ColourMap::iterator it = named_colours.find(std::string(col));
      if (it == named_colours.end()) {
        colours_p[offset1 + i] = R_NaReal;
        colours_p[offset2 + i] = R_NaReal;
        colours_p[offset3 + i] = R_NaReal;
        if (n_cols > 3) colours_p[offset4 + i] = R_NaReal;
        if (n_cols > 4) colours_p[offset5 + i] = R_NaReal;
        continue;
      } else {
        rgb.r = it->second.r;
        rgb.g = it->second.g;
        rgb.b = it->second.b;
        a = 1.0;
      }
    }
    ColorSpace::IConverter<To>::ToColorSpace(&rgb, &to);
    grab<To>(to, colours_p + offset1 + i, colours_p + offset2 + i, colours_p + offset3 + i, n_channels == 4 ? colours_p + offset4 + i : colours_p);
    if (get_alpha) {
      colours_p[n_cols == 4 ? offset4 + i : offset5 + i] = a;
    }
  }
  
  UNPROTECT(1);
  return copy_names(codes, colours);
}

template <>
SEXP decode_impl<ColorSpace::Rgb>(SEXP codes, SEXP alpha, SEXP white) {
  bool get_alpha = LOGICAL(alpha)[0];
  int n = LENGTH(codes);
  ColourMap& named_colours = get_named_colours();
  SEXP colours;
  double* colours_d;
  int* colours_i;
  if (get_alpha) {
    colours = PROTECT(Rf_allocMatrix(REALSXP, n, 4));
    colours_d = REAL(colours);
  } else {
    colours = PROTECT(Rf_allocMatrix(INTSXP, n, 3));
    colours_i = INTEGER(colours);
  }
  int offset1 = 0;
  int offset2 = offset1 + n;
  int offset3 = offset2 + n;
  int offset4 = offset3 + n;
  
  int r, g, b, nchar;
  double a;
  bool has_alpha;
  for (int i = 0; i < n; ++i) {
    SEXP code = STRING_ELT(codes, i);
    if (code == R_NaString) {
      if (get_alpha) {
        colours_d[offset1 + i] = R_NaReal;
        colours_d[offset2 + i] = R_NaReal;
        colours_d[offset3 + i] = R_NaReal;
        colours_d[offset4 + i] = R_NaReal;
      } else {
        colours_i[offset1 + i] = R_NaInt;
        colours_i[offset2 + i] = R_NaInt;
        colours_i[offset3 + i] = R_NaInt;
      }
      continue;
    }
    const char* col = CHAR(code);
    if (col[0] == '#') {
      nchar = strlen(col);
      has_alpha = nchar == 9;
      if (!has_alpha && nchar != 7) {
        Rf_error("Malformed colour string. Must contain either 6 or 8 hex values");
      }
      r = hex2int(col[1]) * 16 + hex2int(col[2]);
      g = hex2int(col[3]) * 16 + hex2int(col[4]);
      b = hex2int(col[5]) * 16 + hex2int(col[6]);
      if (has_alpha) {
        a = hex2int(col[7]) * 16 + hex2int(col[8]);
        a /= 255;
      } else {
        a = 1.0;
      }
    } else {
      ColourMap::iterator it = named_colours.find(std::string(Rf_translateCharUTF8(STRING_ELT(codes, i))));
      if (it == named_colours.end()) {
        if (get_alpha) {
          colours_d[offset1 + i] = R_NaReal;
          colours_d[offset2 + i] = R_NaReal;
          colours_d[offset3 + i] = R_NaReal;
          colours_d[offset4 + i] = R_NaReal;
        } else {
          colours_i[offset1 + i] = R_NaInt;
          colours_i[offset2 + i] = R_NaInt;
          colours_i[offset3 + i] = R_NaInt;
        }
        continue;
      } else {
        r = it->second.r;
        g = it->second.g;
        b = it->second.b;
        a = 1.0;
      }
    }
    if (get_alpha) {
      colours_d[offset1 + i] = (double) r;
      colours_d[offset2 + i] = (double) g;
      colours_d[offset3 + i] = (double) b;
      colours_d[offset4 + i] = a;
    } else {
      colours_i[offset1 + i] = r;
      colours_i[offset2 + i] = g;
      colours_i[offset3 + i] = b;
    }
  }
  
  UNPROTECT(1);
  return copy_names(codes, colours);
}

SEXP decode_c(SEXP codes, SEXP alpha, SEXP to, SEXP white) {
  switch (INTEGER(to)[0]) {
    case CMY: return decode_impl<ColorSpace::Cmy>(codes, alpha, white);
    case CMYK: return decode_impl<ColorSpace::Cmyk>(codes, alpha, white);
    case HSL: return decode_impl<ColorSpace::Hsl>(codes, alpha, white);
    case HSB: return decode_impl<ColorSpace::Hsb>(codes, alpha, white);
    case HSV: return decode_impl<ColorSpace::Hsv>(codes, alpha, white);
    case LAB: return decode_impl<ColorSpace::Lab>(codes, alpha, white);
    case HUNTERLAB: return decode_impl<ColorSpace::HunterLab>(codes, alpha, white);
    case LCH: return decode_impl<ColorSpace::Lch>(codes, alpha, white);
    case LUV: return decode_impl<ColorSpace::Luv>(codes, alpha, white);
    case RGB: return decode_impl<ColorSpace::Rgb>(codes, alpha, white);
    case XYZ: return decode_impl<ColorSpace::Xyz>(codes, alpha, white);
    case YXY: return decode_impl<ColorSpace::Yxy>(codes, alpha, white);
    case HCL: return decode_impl<ColorSpace::Hcl>(codes, alpha, white);
  }
  
  // never happens
  return R_NilValue;
}
