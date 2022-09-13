#include "encode.h"
#include "ColorSpace.h"
#include "farver.h"
#include <algorithm>
#include <string>
#include <cctype>

#include <R_ext/GraphicsEngine.h>

static char hex8[] = "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F606162636465666768696A6B6C6D6E6F707172737475767778797A7B7C7D7E7F808182838485868788898A8B8C8D8E8F909192939495969798999A9B9C9D9E9FA0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBCBDBEBFC0C1C2C3C4C5C6C7C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8D9DADBDCDDDEDFE0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";
static char buffer[] = "#000000";
static char buffera[] = "#00000000";

#ifdef WORDS_BIGENDIAN 
#include <cmath>
inline int double2int(double d) {
  return (int)std::round(d);
}
#else
inline int double2int(double d) {
  d += 6755399441055744.0;
  return reinterpret_cast<int&>(d);
}
#endif

inline int cap0255(int x) {
  return x < 0 ? 0 : (x > 255 ? 255 : x);
}
inline int hex2int(const int x) {
  if (!std::isxdigit(x)) {
    Rf_errorcall(R_NilValue, "Invalid hexadecimal digit");
  }
  return (x & 0xf) + (x >> 6) + ((x >> 6) << 3);
}
inline std::string prepare_code(const char* col) {
  std::string code(col);
  if (isdigit(col[0])) {
    int col_num = atoi(col);
    if (col_num == 0) {
      code = "0";
    } else {
      code = std::to_string(((col_num - 1) % 8) + 1);
    }
  }
  // Remove whitespace
  code.erase(std::remove(code.begin(), code.end(), ' '), code.end());
  std::transform(code.begin(), code.end(), code.begin(),
                 [](unsigned char c){ return std::tolower(c); });
  return code;
}


struct colour_channels {
  int n;
  int *colour_i1;
  int *colour_i2;
  int *colour_i3;
  int *colour_i4;
  double *colour_d1;
  double *colour_d2;
  double *colour_d3;
  double *colour_d4;
  bool colour_is_int;
};

struct alpha_channel {
  char alpha1;
  char alpha2;
  bool has_alpha;
  bool one_alpha;
  bool alpha_is_int;
  bool alpha_is_lgl;
  int *alpha_i;
  double *alpha_d;
  int first_alpha;
  int na_alpha;
};

static inline int get_alpha_value(struct alpha_channel *ac, int i, char *b1 = NULL, char *b2 = NULL) {
  if (!ac->has_alpha) {
    /* Opaque */
    if (b1 != NULL) {
      *b1 = '\0';
    }
    return(ac->na_alpha);
  }
  if (ac->one_alpha) {
    if (b1 != NULL && b2 != NULL) {
      *b1 = ac->alpha1;
      *b2 = ac->alpha2;
    }
    return(ac->first_alpha);
  }
  int alpha;
  if (ac->alpha_is_int) {
    alpha = ac->alpha_i[i];
    alpha = alpha == R_NaInt ? ac->na_alpha : cap0255(alpha);
  } else if (ac->alpha_is_lgl) {
    alpha = ac->alpha_i[i];
    alpha = alpha == R_NaInt ? ac->na_alpha : cap0255(255*alpha);
  }else {
    if (!R_finite(ac->alpha_d[i])) {
      alpha = ac->na_alpha;
    } else {
      alpha = double2int(ac->alpha_d[i]);
      alpha = cap0255(alpha);
    }
  }
  if (b1 != NULL && b2 != NULL) {
    if (alpha == 255 || alpha == R_NaInt) { // opaque
      *b1 = '\0';
      *b2 = '\0';
    } else {
      *b1 = hex8[2*alpha];
      *b2 = hex8[2*alpha+1];
    }
  }
  return alpha;
}


static void get_input_channels(struct colour_channels *cc, SEXP colour, int n_channels) {
  if (TYPEOF(colour) == VECSXP) {
    int input_n_channels = Rf_length(colour);
    if (input_n_channels < n_channels) {
      Rf_errorcall(R_NilValue, "Colour, if given as a list, must contain at least %i elements (channels)", n_channels);
    }
    input_n_channels = n_channels;
    SEXP a_channel = VECTOR_ELT(colour, 0);
    cc->colour_is_int = Rf_isInteger(a_channel);
    if (!(cc->colour_is_int)) {
      if (TYPEOF(a_channel) != REALSXP) {
        Rf_error("All channels must be either integers or reals");
      }
      cc->colour_d1 = REAL(a_channel);
    } else {
      cc->colour_i1 = INTEGER(a_channel);
    }
    cc->n = Rf_length(a_channel);
    for (int i=1; i<n_channels;i++) {
      a_channel = VECTOR_ELT(colour, i);
      if (cc->colour_is_int) {
        if (TYPEOF(a_channel) != INTSXP) {
          Rf_error("The first channel was integer, channel %d is real. All channels should be of the same type", i+1);
        }
      } else {
        if (TYPEOF(a_channel) != REALSXP) {
          Rf_error("The first channel was real, channel %d is integer. All channels should be of the same type", i+1);
        }
      }
      int n_thisch = Rf_length(a_channel);
      if (n_thisch != cc->n) {
        Rf_error("The first channel was of length %d. Channel %d is of length %d. All channels should be of the same length", cc->n, i+1, n_thisch);
      }
      switch(i) {
      case 1:
        if (cc->colour_is_int) {
          cc->colour_i2 = INTEGER(a_channel);
        } else {
          cc->colour_d2 = REAL(a_channel);
        }
        break;
      case 2:
        if (cc->colour_is_int) {
          cc->colour_i3 = INTEGER(a_channel);
        } else {
          cc->colour_d3 = REAL(a_channel);
        }
        break;
      case 3:
        if (cc->colour_is_int) {
          cc->colour_i4 = INTEGER(a_channel);
        } else {
          cc->colour_d4 = REAL(a_channel);
        }
        break;
      }
    }
  } else if (Rf_isMatrix(colour)) {
    cc->colour_is_int = Rf_isInteger(colour);
    if (Rf_ncols(colour) < n_channels) {
      Rf_errorcall(R_NilValue, "Colour in this format must contain at least %i columns", n_channels);
    }
    cc->n = Rf_nrows(colour);
    if (cc->colour_is_int) {
      cc->colour_i1 = INTEGER(colour);
      cc->colour_i2 = cc->colour_i1 + cc->n;
      cc->colour_i3 = cc->colour_i1 + 2*cc->n;
      cc->colour_i4 = cc->colour_i1 + 3*cc->n;
    } else {
      cc->colour_d1 = REAL(colour);
      cc->colour_d2 = cc->colour_d1 + cc->n;
      cc->colour_d3 = cc->colour_d1 + 2*cc->n;
      cc->colour_d4 = cc->colour_d1 + 3*cc->n;
    }
  } else {
    Rf_error("invalid input format, expected a matrix or a list of vectors");
  }
  return;
}

static void get_alpha_channel(struct alpha_channel *ac, SEXP alpha, int n, int na_alpha = 255) {
  int alpha_length;
  ac->na_alpha = na_alpha;
  ac->has_alpha = !Rf_isNull(alpha);
  if (!ac->has_alpha) {
    ac->alpha1 = '\0';
    ac->alpha2 = '\0';
    return;
  }
  ac->alpha_is_int =  Rf_isInteger(alpha);
  ac->alpha_is_lgl = Rf_isLogical(alpha);
  alpha_length = Rf_length(alpha);
  if (alpha_length != 1 && alpha_length != n) {
    Rf_error("alpha should be a numeric vector of length 1 or length %d", n);
  }
  ac->one_alpha = alpha_length == 1;
  if (ac->alpha_is_int) {
    ac->alpha_i = INTEGER(alpha);
    ac->first_alpha = ac->alpha_i[0];
    ac->first_alpha = ac->first_alpha == R_NaInt ? ac->na_alpha : cap0255(ac->first_alpha);
  } else if (ac->alpha_is_lgl) {
    ac->alpha_i= LOGICAL(alpha);
    ac->first_alpha = ac->alpha_i[0];
    ac->first_alpha = ac->first_alpha == R_NaInt ? ac->na_alpha : cap0255(255*ac->first_alpha);
  } else {
    ac->alpha_d = REAL(alpha);
    if (!R_finite(ac->alpha_d[0])) {
      ac->first_alpha = ac->na_alpha;
    } else {
      ac->first_alpha = cap0255(double2int(ac->alpha_d[0]));
    }
  }
  if (ac->first_alpha == R_NaInt || ac->first_alpha == 255) {
    ac->alpha1 = '\0';
    ac->alpha2 = '\0';
  } else {
    ac->alpha1 = hex8[2*ac->first_alpha];
    ac->alpha2 = hex8[2*ac->first_alpha + 1];
  }
  return;
}

static SEXP get_na_colour(SEXP s_na_colour, int *na_colour_int) {
  SEXP na_colour_sexp;
  const char *na_txt;
  if (TYPEOF(s_na_colour) == STRSXP) {
    na_colour_sexp = STRING_ELT(s_na_colour, 0);
  } else {
    na_colour_sexp = s_na_colour;
  }
  na_txt = CHAR(na_colour_sexp);

  if (s_na_colour == R_NaString || strcmp("NA", na_txt) == 0) {
    na_colour_sexp = R_NaString;
    *na_colour_int = R_NaInt;
  } else {
    SEXP tmp = encode_native_c(na_colour_sexp, R_NaString);
    *na_colour_int = INTEGER(tmp)[0];
  }
  return(na_colour_sexp);
}

template <typename From>
SEXP encode_impl(SEXP colour, SEXP alpha, SEXP white, SEXP s_out_fmt, SEXP s_na_colour) {
  struct colour_channels cc;
  struct alpha_channel ac;
  int out_fmt = INTEGER(s_out_fmt)[0]; // 1 is character vector, 2 is native format
  if (out_fmt != 1 && out_fmt != 2) {
    Rf_error("invalid output format.");
  }
  int na_colour_int;
  SEXP na_colour_sexp = get_na_colour(s_na_colour, &na_colour_int);

  int *codes_int; 
  int n_channels = dimension<From>();
  get_input_channels(&cc, colour, n_channels);
  static ColorSpace::Rgb rgb;
  ColorSpace::XyzConverter::SetWhiteReference(REAL(white)[0], REAL(white)[1], REAL(white)[2]);
  SEXP codes;
  if (out_fmt == 1) {
    codes = PROTECT(Rf_allocVector(STRSXP, cc.n));
  } else {
    codes = PROTECT(Rf_allocVector(INTSXP, cc.n));
    codes_int = INTEGER(codes);
  }
  get_alpha_channel(&ac, alpha, cc.n);
  
  char* buf = NULL;
  if (ac.has_alpha) {
    buf = buffera;
  } else {
    buf = buffer;
  }

  for (int i = 0; i < cc.n; ++i) {
    if (cc.colour_is_int) {
      fill_rgb<From>(&rgb, cc.colour_i1[i], cc.colour_i2[i], cc.colour_i3[i], n_channels == 4 ? cc.colour_i4[i] : 0);
    } else {
      fill_rgb<From>(&rgb, cc.colour_d1[i], cc.colour_d2[i], cc.colour_d3[i], n_channels == 4 ? cc.colour_d4[i] : 0.0);
    }
    if (!rgb.valid) {
      if (out_fmt == 1) {
        SET_STRING_ELT(codes, i, na_colour_sexp);
      } else {
        codes_int[i] = na_colour_int;
      }
      continue;
    }
    
    if (out_fmt == 2) { /* native output */
      int alpha = get_alpha_value(&ac, i);
      /* This can overflow and become negative, but R just cares about the bytes */
      codes_int[i] = (
        cap0255(double2int(rgb.r)) +
        (cap0255(double2int(rgb.g)) << 8) +
        (cap0255(double2int(rgb.b)) << 16) +
        (alpha << 24)
      );
    } else { /* character output */
      int num;
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
      
      if (ac.has_alpha) {
        get_alpha_value(&ac, i, &(buf[7]), &(buf[8]));
      }
      SET_STRING_ELT(codes, i, Rf_mkChar(buf));
    }
  }
  
  copy_names(colour, codes);
  UNPROTECT(1);
  return codes;
}

template<>
SEXP encode_impl<ColorSpace::Rgb>(SEXP colour, SEXP alpha, SEXP white, SEXP s_out_fmt, SEXP s_na_colour) {
  struct colour_channels cc;
  struct alpha_channel ac;
  int out_fmt = INTEGER(s_out_fmt)[0]; // 1 is character vector, 2 is native format
  if (out_fmt != 1 && out_fmt != 2) {
    Rf_error("invalid output format.");
  }
  get_input_channels(&cc, colour, 3);
  int na_colour_int;
  SEXP na_colour_sexp = get_na_colour(s_na_colour, &na_colour_int);
  
  int *codes_int;
  SEXP codes;
  if (out_fmt == 1) {
    codes = PROTECT(Rf_allocVector(STRSXP, cc.n));
  } else {
    codes = PROTECT(Rf_allocVector(INTSXP, cc.n));
    codes_int = INTEGER(codes);
  }
  get_alpha_channel(&ac, alpha, cc.n);
  
  char* buf = NULL;
  if (ac.has_alpha) {
    buf = buffera;
  } else {
    buf = buffer;
  }

  if (cc.colour_is_int) {
    int r, g, b;
    for (int i = 0; i < cc.n; ++i) {
      r = cc.colour_i1[i];
      g = cc.colour_i2[i];
      b = cc.colour_i3[i];
      if (r == R_NaInt || g == R_NaInt || b == R_NaInt) {
        if (out_fmt == 1) {
          SET_STRING_ELT(codes, i, na_colour_sexp);
        } else {
          codes_int[i] = na_colour_int;
        }
        continue;
      }
      if (out_fmt == 2) {
        int alpha = get_alpha_value(&ac, i);
        codes_int[i] = (
          cap0255(r) +
          (cap0255(g) << 8) +
          (cap0255(b) << 16) +
          (alpha << 24)
        );
        
      } else {
        int num;
        num = cap0255(r) * 2;
        buf[1] = hex8[num];
        buf[2] = hex8[num + 1];
        
        num = cap0255(g) * 2;
        buf[3] = hex8[num];
        buf[4] = hex8[num + 1];
        
        num = cap0255(b) * 2;
        buf[5] = hex8[num];
        buf[6] = hex8[num + 1];
        
        if (ac.has_alpha) {
          get_alpha_value(&ac, i, &(buf[7]), &(buf[8]));
        }
        SET_STRING_ELT(codes, i, Rf_mkChar(buf));
      }
    }
  } else {
    double r, g, b;
    for (int i = 0; i < cc.n; ++i) {
      r = cc.colour_d1[i];
      g = cc.colour_d2[i];
      b = cc.colour_d3[i];
      if (!(R_finite(r) && R_finite(g) && R_finite(b))) {
        if (out_fmt == 1) {
          SET_STRING_ELT(codes, i, na_colour_sexp);
        } else {
          codes_int[i] = na_colour_int;
        }
        continue;
      }
      if (out_fmt == 2)  {
        int alpha = get_alpha_value(&ac, i);
        /* This can overflow and become negative, but R just cares about the bytes */
        codes_int[i] = (
          cap0255(double2int(r)) +
          (cap0255(double2int(g)) << 8) +
          (cap0255(double2int(b)) << 16) +
          (alpha << 24)
        );
      } else {
        int num;
        num = cap0255(double2int(r)) * 2;
        buf[1] = hex8[num];
        buf[2] = hex8[num + 1];
        
        num = cap0255(double2int(g)) * 2;
        buf[3] = hex8[num];
        buf[4] = hex8[num + 1];
        
        num = cap0255(double2int(b)) * 2;
        buf[5] = hex8[num];
        buf[6] = hex8[num + 1];
        
        if (ac.has_alpha) {
          get_alpha_value(&ac, i, &(buf[7]), &(buf[8]));
        }
      }
      SET_STRING_ELT(codes, i, Rf_mkChar(buf));
    }
  }
  copy_names(colour, codes);
  UNPROTECT(1);
  return codes;
}

SEXP replace_alpha_native_c(SEXP colour, SEXP alpha) {
  struct alpha_channel ac;
  int n;
  if (TYPEOF(colour) != INTSXP) {
    Rf_error("colour must be an integer vector.");
  }
  n = Rf_length(colour);
  get_alpha_channel(&ac, alpha, n, R_NaInt);
  if (!ac.has_alpha || (ac.one_alpha && ac.first_alpha == R_NaInt)) {
    return colour;
  }
  int *colour_i = INTEGER(colour);
  int i;
  int alpha_value;
  SEXP codes = PROTECT(Rf_allocVector(INTSXP, n));
  int *codes_i = INTEGER(codes);
  int mask = 0xff << 24;
  for (i = 0; i<n;i++) {
    alpha_value = get_alpha_value(&ac, i);
    if (alpha_value != R_NaInt) {
      codes_i[i] = (~mask & colour_i[i]) | (alpha_value << 24);
    } else {
      codes_i[i] = colour_i[i];
    }
  }
  UNPROTECT(1);
  return codes;
}


SEXP encode_c(SEXP colour, SEXP alpha, SEXP from, SEXP white, SEXP out_fmt, SEXP na_colour) {
  switch (INTEGER(from)[0]) {
    case CMY: return encode_impl<ColorSpace::Cmy>(colour, alpha, white, out_fmt, na_colour);
    case CMYK: return encode_impl<ColorSpace::Cmyk>(colour, alpha, white, out_fmt, na_colour);
    case HSL: return encode_impl<ColorSpace::Hsl>(colour, alpha, white, out_fmt, na_colour);
    case HSB: return encode_impl<ColorSpace::Hsb>(colour, alpha, white, out_fmt, na_colour);
    case HSV: return encode_impl<ColorSpace::Hsv>(colour, alpha, white, out_fmt, na_colour);
    case LAB: return encode_impl<ColorSpace::Lab>(colour, alpha, white, out_fmt, na_colour);
    case HUNTERLAB: return encode_impl<ColorSpace::HunterLab>(colour, alpha, white, out_fmt, na_colour);
    case LCH: return encode_impl<ColorSpace::Lch>(colour, alpha, white, out_fmt, na_colour);
    case LUV: return encode_impl<ColorSpace::Luv>(colour, alpha, white, out_fmt, na_colour);
    case RGB: return encode_impl<ColorSpace::Rgb>(colour, alpha, white, out_fmt, na_colour);
    case XYZ: return encode_impl<ColorSpace::Xyz>(colour, alpha, white, out_fmt, na_colour);
    case YXY: return encode_impl<ColorSpace::Yxy>(colour, alpha, white, out_fmt, na_colour);
    case HCL: return encode_impl<ColorSpace::Hcl>(colour, alpha, white, out_fmt, na_colour);
    case OKLAB: return encode_impl<ColorSpace::OkLab>(colour, alpha, white, out_fmt, na_colour);
    case OKLCH: return encode_impl<ColorSpace::OkLch>(colour, alpha, white, out_fmt, na_colour);
  }
  
  // never happens
  return R_NilValue;
}

SEXP load_colour_names_c(SEXP name, SEXP value) {
  ColourMap& named_colours = get_named_colours();
  int n = Rf_length(name);
  if (n != Rf_ncols(value)) {
    Rf_errorcall(R_NilValue, "name and value must have the same length");
  }
  int* values = INTEGER(value);
  int it = 0;
  for (int i = 0; i < n; ++i) {
    std::string colour_name(Rf_translateCharUTF8(STRING_ELT(name, i)));
    rgb_colour col;
    col.r = values[it++];
    col.g = values[it++];
    col.b = values[it++];
    col.a = values[it++];
    named_colours[colour_name] = col;
  }
  return R_NilValue;
}

template <typename To>
SEXP decode_impl(SEXP codes, SEXP alpha, SEXP white, SEXP na) {
  bool get_alpha = LOGICAL(alpha)[0];
  int n_channels = dimension<To>();
  int n_cols = get_alpha ? n_channels + 1 : n_channels;
  int n = Rf_length(codes);
  ColourMap& named_colours = get_named_colours();
  SEXP colours = PROTECT(Rf_allocMatrix(REALSXP, n, n_cols));
  double* colours_p = REAL(colours);
  SEXP na_code = STRING_ELT(na, 0);
  bool na_is_na = na_code == R_NaString;
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
    if (code == R_NaString || strcmp("NA", CHAR(code)) == 0) {
      if (na_is_na) {
        colours_p[offset1 + i] = R_NaReal;
        colours_p[offset2 + i] = R_NaReal;
        colours_p[offset3 + i] = R_NaReal;
        if (n_cols > 3) colours_p[offset4 + i] = R_NaReal;
        if (n_cols > 4) colours_p[offset5 + i] = R_NaReal;
        continue;
      }
      code = na_code;
    }
    const char* col = Rf_translateCharUTF8(code);
    if (col[0] == '#') {
      nchar = strlen(col);
      has_alpha = nchar == 9;
      if (!has_alpha && nchar != 7) {
        Rf_errorcall(R_NilValue, "Malformed colour string `%s`. Must contain either 6 or 8 hex values", col);
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
      ColourMap::iterator it = named_colours.find(prepare_code(col));
      if (it == named_colours.end()) {
        Rf_errorcall(R_NilValue, "Unknown colour name: %s", col);
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
        a = it->second.a;
      }
    }
    ColorSpace::IConverter<To>::ToColorSpace(&rgb, &to);
    grab<To>(to, colours_p + offset1 + i, colours_p + offset2 + i, colours_p + offset3 + i, n_channels == 4 ? colours_p + offset4 + i : colours_p);
    if (get_alpha) {
      colours_p[n_cols == 4 ? offset4 + i : offset5 + i] = a;
    }
  }
  
  copy_names(codes, colours);
  UNPROTECT(1);
  return colours;
}

template <>
SEXP decode_impl<ColorSpace::Rgb>(SEXP codes, SEXP alpha, SEXP white, SEXP na) {
  bool get_alpha = LOGICAL(alpha)[0];
  int n = Rf_length(codes);
  ColourMap& named_colours = get_named_colours();
  SEXP colours;
  double* colours_d = NULL;
  int* colours_i = NULL;
  SEXP na_code = STRING_ELT(na, 0);
  bool na_is_na = na_code == R_NaString;
  
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
    if (code == R_NaString || strcmp("NA", CHAR(code)) == 0) {
      if (na_is_na) {
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
      code = na_code;
    }
    const char* col = Rf_translateCharUTF8(code);
    if (col[0] == '#') {
      nchar = strlen(col);
      has_alpha = nchar == 9;
      if (!has_alpha && nchar != 7) {
        Rf_errorcall(R_NilValue, "Malformed colour string `%s`. Must contain either 6 or 8 hex values", col);
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
      ColourMap::iterator it = named_colours.find(prepare_code(col));
      if (it == named_colours.end()) {
        Rf_errorcall(R_NilValue, "Unknown colour name: %s", col);
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
        a = it->second.a;
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
  
  copy_names(codes, colours);
  UNPROTECT(1);
  return colours;
}

SEXP decode_c(SEXP codes, SEXP alpha, SEXP to, SEXP white, SEXP na) {
  switch (INTEGER(to)[0]) {
    case CMY: return decode_impl<ColorSpace::Cmy>(codes, alpha, white, na);
    case CMYK: return decode_impl<ColorSpace::Cmyk>(codes, alpha, white, na);
    case HSL: return decode_impl<ColorSpace::Hsl>(codes, alpha, white, na);
    case HSB: return decode_impl<ColorSpace::Hsb>(codes, alpha, white, na);
    case HSV: return decode_impl<ColorSpace::Hsv>(codes, alpha, white, na);
    case LAB: return decode_impl<ColorSpace::Lab>(codes, alpha, white, na);
    case HUNTERLAB: return decode_impl<ColorSpace::HunterLab>(codes, alpha, white, na);
    case LCH: return decode_impl<ColorSpace::Lch>(codes, alpha, white, na);
    case LUV: return decode_impl<ColorSpace::Luv>(codes, alpha, white, na);
    case RGB: return decode_impl<ColorSpace::Rgb>(codes, alpha, white, na);
    case XYZ: return decode_impl<ColorSpace::Xyz>(codes, alpha, white, na);
    case YXY: return decode_impl<ColorSpace::Yxy>(codes, alpha, white, na);
    case HCL: return decode_impl<ColorSpace::Hcl>(codes, alpha, white, na);
    case OKLAB: return decode_impl<ColorSpace::OkLab>(codes, alpha, white, na);
    case OKLCH: return decode_impl<ColorSpace::OkLch>(codes, alpha, white, na);
  }
  
  // never happens
  return R_NilValue;
}

template <typename Space>
SEXP encode_channel_impl(SEXP codes, SEXP channel, SEXP value, SEXP op, SEXP white, SEXP na) {
  int chan = INTEGER(channel)[0];
  int operation = INTEGER(op)[0];
  int n = Rf_length(codes);
  
  bool one_value = Rf_length(value) == 1;
  int first_value_i = 0;
  double first_value_d = 0.0;
  int* value_i = NULL;
  double* value_d = NULL;
  bool value_is_int = Rf_isInteger(value);
  if (value_is_int) {
    value_i = INTEGER(value);
    first_value_i = value_i[0];
  } else {
    value_d = REAL(value);
    first_value_d = value_d[0];
  }
  
  SEXP na_code = STRING_ELT(na, 0);
  bool na_is_na = na_code == R_NaString;
  
  SEXP ret = PROTECT(Rf_allocVector(STRSXP, n));
  ColorSpace::Rgb rgb;
  ColorSpace::XyzConverter::SetWhiteReference(REAL(white)[0], REAL(white)[1], REAL(white)[2]);
  Space colour;
  int num, nchar;
  ColourMap& named_colours = get_named_colours();
  
  for (int i = 0; i < n; ++i) {
    SEXP code = STRING_ELT(codes, i);
    if (code == R_NaString || strcmp("NA", CHAR(code)) == 0) {
      if (na_is_na) {
        SET_STRING_ELT(ret, i, R_NaString);
        continue;
      }
      code = na_code;
    } 
    if ((value_is_int && (one_value ? first_value_i : value_i[i]) == R_NaInt) ||
        (!value_is_int && !R_finite(one_value ? first_value_d : value_d[i]))) {
      SET_STRING_ELT(ret, i, R_NaString);
      continue;
    }
    const char* col = CHAR(code);
    if (col[0] == '#') {
      nchar = strlen(col);
      if (nchar != 9 && nchar != 7) {
        Rf_errorcall(R_NilValue, "Malformed colour string `%s`. Must contain either 6 or 8 hex values", col);
      }
      rgb.r = hex2int(col[1]) * 16 + hex2int(col[2]);
      rgb.g = hex2int(col[3]) * 16 + hex2int(col[4]);
      rgb.b = hex2int(col[5]) * 16 + hex2int(col[6]);
      strcpy(buffera, col);
    } else {
      ColourMap::iterator it = named_colours.find(prepare_code(col));
      if (it == named_colours.end()) {
        Rf_errorcall(R_NilValue, "Unknown colour name: %s", col);
        SET_STRING_ELT(ret, i, R_NaString);
        continue;
      }
      rgb.r = it->second.r;
      rgb.g = it->second.g;
      rgb.b = it->second.b;
      strcpy(buffera, buffer);
      if (it->second.a == 0) {
        buffera[7] = '0';
        buffera[8] = '0';
      }
    }
    ColorSpace::IConverter<Space>::ToColorSpace(&rgb, &colour);
    if (value_is_int) {
      modify_channel<Space>(colour, one_value ? first_value_i : value_i[i], chan, operation);
    } else {
      modify_channel<Space>(colour, one_value ? first_value_d : value_d[i], chan, operation);
    }
    colour.Cap();
    colour.ToRgb(&rgb);
    
    if (!(R_finite(rgb.r) && R_finite(rgb.g) && R_finite(rgb.b))) {
      SET_STRING_ELT(ret, i, R_NaString);
      continue;
    }
    num = cap0255(double2int(rgb.r)) * 2;
    buffera[1] = hex8[num];
    buffera[2] = hex8[num + 1];
    
    num = cap0255(double2int(rgb.g)) * 2;
    buffera[3] = hex8[num];
    buffera[4] = hex8[num + 1];
    
    num = cap0255(double2int(rgb.b)) * 2;
    buffera[5] = hex8[num];
    buffera[6] = hex8[num + 1];
    SET_STRING_ELT(ret, i, Rf_mkChar(buffera));
  }
  
  copy_names(codes, ret);
  UNPROTECT(1);
  return ret;
}

template <>
SEXP encode_channel_impl<ColorSpace::Rgb>(SEXP codes, SEXP channel, SEXP value, SEXP op, SEXP white, SEXP na) {
  int chan = INTEGER(channel)[0];
  int operation = INTEGER(op)[0];
  int n = Rf_length(codes);
  
  bool one_value = Rf_length(value) == 1;
  int first_value_i = 0;
  double first_value_d = 0.0;
  int* value_i = NULL;
  double* value_d = NULL;
  bool value_is_int = Rf_isInteger(value);
  if (value_is_int) {
    value_i = INTEGER(value);
    first_value_i = value_i[0];
  } else {
    value_d = REAL(value);
    first_value_d = value_d[0];
  }
  
  SEXP na_code = STRING_ELT(na, 0);
  bool na_is_na = na_code == R_NaString;
  
  SEXP ret = PROTECT(Rf_allocVector(STRSXP, n));
  int num, nchar;
  double new_val;
  ColourMap& named_colours = get_named_colours();
  
  for (int i = 0; i < n; ++i) {
    SEXP code = STRING_ELT(codes, i);
    if (code == R_NaString || strcmp("NA", CHAR(code)) == 0) {
      if (na_is_na) {
        SET_STRING_ELT(ret, i, R_NaString);
        continue;
      }
      code = na_code;
    } 
    if ((value_is_int && (one_value ? first_value_i : value_i[i]) == R_NaInt) ||
        (!value_is_int && !R_finite(one_value ? first_value_d : value_d[i]))) {
      SET_STRING_ELT(ret, i, R_NaString);
      continue;
    }
    const char* col = CHAR(code);
    if (col[0] == '#') {
      nchar = strlen(col);
      if (nchar != 9 && nchar != 7) {
        Rf_errorcall(R_NilValue, "Malformed colour string `%s`. Must contain either 6 or 8 hex values", col);
      }
      strcpy(buffera, col);
    } else {
      ColourMap::iterator it = named_colours.find(prepare_code(col));
      if (it == named_colours.end()) {
        Rf_errorcall(R_NilValue, "Unknown colour name: %s", col);
        SET_STRING_ELT(ret, i, R_NaString);
        continue;
      }
      num = cap0255(it->second.r) * 2;
      buffera[1] = hex8[num];
      buffera[2] = hex8[num + 1];
      
      num = cap0255(it->second.g) * 2;
      buffera[3] = hex8[num];
      buffera[4] = hex8[num + 1];
      
      num = cap0255(it->second.b) * 2;
      buffera[5] = hex8[num];
      buffera[6] = hex8[num + 1];
      
      if (it->second.a == 1) {
        buffera[7] = '\0';
      } else {
        num = cap0255(it->second.a * 255) * 2;
        buffera[7] = hex8[num];
        buffera[8] = hex8[num + 1];
      }
      
    }
    switch (chan) {
    case 1: 
      new_val = mod_val(hex2int(buffera[1]) * 16 + hex2int(buffera[2]), value_is_int ? (one_value ? first_value_i : value_i[i]) : (one_value ? first_value_d : value_d[i]), operation); // Sometimes I really hate myself
      num = cap0255(new_val) * 2;
      buffera[1] = hex8[num];
      buffera[2] = hex8[num + 1];
      break;
    case 2: 
      new_val = mod_val(hex2int(buffera[3]) * 16 + hex2int(buffera[4]), value_is_int ? (one_value ? first_value_i : value_i[i]) : (one_value ? first_value_d : value_d[i]), operation);
      num = cap0255(new_val) * 2;
      buffera[3] = hex8[num];
      buffera[4] = hex8[num + 1];
      break;
    case 3:
      new_val = mod_val(hex2int(buffera[5]) * 16 + hex2int(buffera[6]), value_is_int ? (one_value ? first_value_i : value_i[i]) : (one_value ? first_value_d : value_d[i]), operation);
      num = cap0255(new_val) * 2;
      buffera[5] = hex8[num];
      buffera[6] = hex8[num + 1];
      break;
    }
    
    SET_STRING_ELT(ret, i, Rf_mkChar(buffera));
  }
  
  copy_names(codes, ret);
  UNPROTECT(1);
  return ret;
}

SEXP encode_alpha_impl(SEXP codes, SEXP value, SEXP op, SEXP na) {
  int operation = INTEGER(op)[0];
  int n = Rf_length(codes);
  
  bool one_value = Rf_length(value) == 1;
  int first_value_i = 0;
  double first_value_d = 0.0;
  int* value_i = NULL;
  double* value_d = NULL;
  bool value_is_int = Rf_isInteger(value);
  if (value_is_int) {
    value_i = INTEGER(value);
    first_value_i = value_i[0];
  } else {
    value_d = REAL(value);
    first_value_d = value_d[0];
  }
  
  SEXP na_code = STRING_ELT(na, 0);
  bool na_is_na = na_code == R_NaString;
  
  SEXP ret = PROTECT(Rf_allocVector(STRSXP, n));
  int alpha, num, nchar;
  ColourMap& named_colours = get_named_colours();
  
  for (int i = 0; i < n; ++i) {
    SEXP code = STRING_ELT(codes, i);
    if (code == R_NaString || strcmp("NA", CHAR(code)) == 0) {
      if (na_is_na) {
        SET_STRING_ELT(ret, i, R_NaString);
        continue;
      }
      code = na_code;
    }
    
    const char* col = CHAR(code);
    if (col[0] == '#') {
      nchar = strlen(col);
      if (nchar != 9 && nchar != 7) {
        Rf_errorcall(R_NilValue, "Malformed colour string `%s`. Must contain either 6 or 8 hex values", col);
      }
      strcpy(buffera, col);
      if (strlen(buffera) == 7) {
        alpha = 255;
      } else {
        alpha = hex2int(buffera[7]) * 16 + hex2int(buffera[8]);
      }
    } else {
      ColourMap::iterator it = named_colours.find(prepare_code(col));
      if (it == named_colours.end()) {
        Rf_errorcall(R_NilValue, "Unknown colour name: %s", col);
        SET_STRING_ELT(ret, i, R_NaString);
        continue;
      }
      num = cap0255(it->second.r) * 2;
      buffera[1] = hex8[num];
      buffera[2] = hex8[num + 1];
      
      num = cap0255(it->second.g) * 2;
      buffera[3] = hex8[num];
      buffera[4] = hex8[num + 1];
      
      num = cap0255(it->second.b) * 2;
      buffera[5] = hex8[num];
      buffera[6] = hex8[num + 1];
      
      alpha = it->second.a * 255;
    }
    
    if (value_is_int) {
      alpha = double2int(mod_val(alpha / 255.0, one_value ? first_value_i : value_i[i], operation) * 255.0);
    } else {
      alpha = double2int(mod_val(alpha / 255.0, one_value ? first_value_d : value_d[i], operation) * 255.0);
    }
    alpha = cap0255(alpha);
    if (alpha == 255) {
      buffera[7] = '\0';
    } else {
      num = alpha * 2;
      buffera[7] = hex8[num];
      buffera[8] = hex8[num + 1];
    }
    SET_STRING_ELT(ret, i, Rf_mkChar(buffera));
  }
  
  copy_names(codes, ret);
  UNPROTECT(1);
  return ret;
}

SEXP encode_channel_c(SEXP codes, SEXP channel, SEXP value, SEXP space, SEXP op, SEXP white, SEXP na) {
  if (INTEGER(channel)[0] == 0) {
    return encode_alpha_impl(codes, value, op, na);
  }
  switch (INTEGER(space)[0]) {
    case CMY: return encode_channel_impl<ColorSpace::Cmy>(codes, channel, value, op, white, na);
    case CMYK: return encode_channel_impl<ColorSpace::Cmyk>(codes, channel, value, op, white, na);
    case HSL: return encode_channel_impl<ColorSpace::Hsl>(codes, channel, value, op, white, na);
    case HSB: return encode_channel_impl<ColorSpace::Hsb>(codes, channel, value, op, white, na);
    case HSV: return encode_channel_impl<ColorSpace::Hsv>(codes, channel, value, op, white, na);
    case LAB: return encode_channel_impl<ColorSpace::Lab>(codes, channel, value, op, white, na);
    case HUNTERLAB: return encode_channel_impl<ColorSpace::HunterLab>(codes, channel, value, op, white, na);
    case LCH: return encode_channel_impl<ColorSpace::Lch>(codes, channel, value, op, white, na);
    case LUV: return encode_channel_impl<ColorSpace::Luv>(codes, channel, value, op, white, na);
    case RGB: return encode_channel_impl<ColorSpace::Rgb>(codes, channel, value, op, white, na);
    case XYZ: return encode_channel_impl<ColorSpace::Xyz>(codes, channel, value, op, white, na);
    case YXY: return encode_channel_impl<ColorSpace::Yxy>(codes, channel, value, op, white, na);
    case HCL: return encode_channel_impl<ColorSpace::Hcl>(codes, channel, value, op, white, na);
    case OKLAB: return encode_channel_impl<ColorSpace::OkLab>(codes, channel, value, op, white, na);
    case OKLCH: return encode_channel_impl<ColorSpace::OkLch>(codes, channel, value, op, white, na);
  }
  
  // never happens
  return R_NilValue;
}

template <typename Space>
SEXP decode_channel_impl(SEXP codes, SEXP channel, SEXP white, SEXP na) {
  int chan = INTEGER(channel)[0];
  int n = Rf_length(codes);
  
  SEXP ret = PROTECT(Rf_allocVector(REALSXP, n));
  double* ret_p = REAL(ret);
  ColorSpace::Rgb rgb;
  ColorSpace::XyzConverter::SetWhiteReference(REAL(white)[0], REAL(white)[1], REAL(white)[2]);
  Space colour;
  ColourMap& named_colours = get_named_colours();
  int nchar;
  
  SEXP na_code = STRING_ELT(na, 0);
  bool na_is_na = na_code == R_NaString;
  
  for (int i = 0; i < n; ++i) {
    SEXP code = STRING_ELT(codes, i);
    if (code == R_NaString || strcmp("NA", CHAR(code)) == 0) {
      if (na_is_na) {
        ret_p[i] = R_NaReal;
        continue;
      }
      code = na_code;
    }
    const char* col = CHAR(code);
    if (col[0] == '#') {
      nchar = strlen(col);
      if (nchar != 9 && nchar != 7) {
        Rf_errorcall(R_NilValue, "Malformed colour string `%s`. Must contain either 6 or 8 hex values", col);
      }
      rgb.r = hex2int(col[1]) * 16 + hex2int(col[2]);
      rgb.g = hex2int(col[3]) * 16 + hex2int(col[4]);
      rgb.b = hex2int(col[5]) * 16 + hex2int(col[6]);
    } else {
      ColourMap::iterator it = named_colours.find(prepare_code(col));
      if (it == named_colours.end()) {
        Rf_errorcall(R_NilValue, "Unknown colour name: %s", col);
        ret_p[i] = R_NaReal;
        continue;
      }
      rgb.r = it->second.r;
      rgb.g = it->second.g;
      rgb.b = it->second.b;
    }
    ColorSpace::IConverter<Space>::ToColorSpace(&rgb, &colour);
    colour.Cap();
    
    ret_p[i] = grab_channel<Space>(colour, chan);
  }
  
  copy_names(codes, ret);
  UNPROTECT(1);
  return ret;
}

template <>
SEXP decode_channel_impl<ColorSpace::Rgb>(SEXP codes, SEXP channel, SEXP white, SEXP na) {
  int chan = INTEGER(channel)[0];
  int n = Rf_length(codes);
  
  SEXP ret = PROTECT(Rf_allocVector(INTSXP, n));
  int* ret_p = INTEGER(ret);
  ColourMap& named_colours = get_named_colours();
  int val = 0;
  int nchar = 0;
  
  SEXP na_code = STRING_ELT(na, 0);
  bool na_is_na = na_code == R_NaString;
  
  for (int i = 0; i < n; ++i) {
    SEXP code = STRING_ELT(codes, i);
    if (code == R_NaString || strcmp("NA", CHAR(code)) == 0) {
      if (na_is_na) {
        ret_p[i] = R_NaInt;
        continue;
      }
      code = na_code;
    }
    const char* col = CHAR(code);
    if (col[0] == '#') {
      nchar = strlen(col);
      if (nchar != 9 && nchar != 7) {
        Rf_errorcall(R_NilValue, "Malformed colour string `%s`. Must contain either 6 or 8 hex values", col);
      }
      switch (chan) {
      case 1: 
        val = hex2int(col[1]) * 16 + hex2int(col[2]);
        break;
      case 2: 
        val = hex2int(col[3]) * 16 + hex2int(col[4]);
        break;
      case 3: 
        val = hex2int(col[5]) * 16 + hex2int(col[6]);
        break;
      }
    } else {
      ColourMap::iterator it = named_colours.find(prepare_code(col));
      if (it == named_colours.end()) {
        Rf_errorcall(R_NilValue, "Unknown colour name: %s", col);
        ret_p[i] = R_NaInt;
        continue;
      }
      switch (chan) {
      case 1: 
        val = it->second.r;
        break;
      case 2: 
        val = it->second.g;
        break;
      case 3: 
        val = it->second.b;
        break;
      }
    }
    
    ret_p[i] = val;
  }
  
  copy_names(codes, ret);
  UNPROTECT(1);
  return ret;
}

SEXP decode_alpha_impl(SEXP codes, SEXP na) {
  int n = Rf_length(codes);
  
  SEXP ret = PROTECT(Rf_allocVector(REALSXP, n));
  double* ret_p = REAL(ret);
  ColourMap& named_colours = get_named_colours();
  int nchar;
  bool has_alpha;
  double val;
  
  SEXP na_code = STRING_ELT(na, 0);
  bool na_is_na = na_code == R_NaString;
  
  for (int i = 0; i < n; ++i) {
    SEXP code = STRING_ELT(codes, i);
    if (code == R_NaString || strcmp("NA", CHAR(code)) == 0) {
      if (na_is_na) {
        ret_p[i] = R_NaInt;
        continue;
      }
      code = na_code;
    }
    const char* col = CHAR(code);
    if (col[0] == '#') {
      nchar = strlen(col);
      has_alpha = nchar == 9;
      if (!has_alpha && nchar != 7) {
        Rf_errorcall(R_NilValue, "Malformed colour string `%s`. Must contain either 6 or 8 hex values", col);
      }
      if (has_alpha) {
        val = (hex2int(col[7]) * 16 + hex2int(col[8])) / 255.0;
      } else {
        val = 1.0;
      }
    } else {
      ColourMap::iterator it = named_colours.find(prepare_code(col));
      if (it == named_colours.end()) {
        Rf_errorcall(R_NilValue, "Unknown colour name: %s", col);
        ret_p[i] = R_NaReal;
        continue;
      }
      val = it->second.a;
    }
    
    ret_p[i] = val;
  }
  
  copy_names(codes, ret);
  UNPROTECT(1);
  return ret;
}

SEXP decode_channel_c(SEXP codes, SEXP channel, SEXP space, SEXP white, SEXP na) {
  if (INTEGER(channel)[0] == 0) {
    return decode_alpha_impl(codes, na);
  }
  switch (INTEGER(space)[0]) {
  case CMY: return decode_channel_impl<ColorSpace::Cmy>(codes, channel, white, na);
  case CMYK: return decode_channel_impl<ColorSpace::Cmyk>(codes, channel, white, na);
  case HSL: return decode_channel_impl<ColorSpace::Hsl>(codes, channel, white, na);
  case HSB: return decode_channel_impl<ColorSpace::Hsb>(codes, channel, white, na);
  case HSV: return decode_channel_impl<ColorSpace::Hsv>(codes, channel, white, na);
  case LAB: return decode_channel_impl<ColorSpace::Lab>(codes, channel, white, na);
  case HUNTERLAB: return decode_channel_impl<ColorSpace::HunterLab>(codes, channel, white, na);
  case LCH: return decode_channel_impl<ColorSpace::Lch>(codes, channel, white, na);
  case LUV: return decode_channel_impl<ColorSpace::Luv>(codes, channel, white, na);
  case RGB: return decode_channel_impl<ColorSpace::Rgb>(codes, channel, white, na);
  case XYZ: return decode_channel_impl<ColorSpace::Xyz>(codes, channel, white, na);
  case YXY: return decode_channel_impl<ColorSpace::Yxy>(codes, channel, white, na);
  case HCL: return decode_channel_impl<ColorSpace::Hcl>(codes, channel, white, na);
  case OKLAB: return decode_channel_impl<ColorSpace::OkLab>(codes, channel, white, na);
  case OKLCH: return decode_channel_impl<ColorSpace::OkLch>(codes, channel, white, na);
  }
  
  // never happens
  return R_NilValue;
}

SEXP encode_native_c(SEXP color, SEXP na_colour) {
  int n = Rf_length(color);
  switch(TYPEOF(color)) {
    case STRSXP:
      n = Rf_length(color);
      break;
    case CHARSXP:
      n = 1;
      break;
    default:
      Rf_error("color should be a character vector");
  }
  int na_colour_int;
  get_na_colour(na_colour, &na_colour_int);
  ColourMap& named_colours = get_named_colours();
  SEXP natives = PROTECT(Rf_allocVector(INTSXP, n));
  int* natives_p = INTEGER(natives);
  
  int nchar;
  bool has_alpha;
  for (int i = 0; i < n; ++i) {
    SEXP code;
    if (TYPEOF(color) == STRSXP) {
      code = STRING_ELT(color, i);
    } else {
      code = color;
    }
    if (code == R_NaString || strcmp("NA", CHAR(code)) == 0) {
      natives_p[i] = na_colour_int;
    }
    const char* col = Rf_translateCharUTF8(code);
    if (col[0] == '#') {
      nchar = strlen(col);
      has_alpha = nchar == 9;
      if (!has_alpha && nchar != 7) {
        Rf_errorcall(R_NilValue, "Malformed colour string `%s`. Must contain either 6 or 8 hex values", col);
      }
      natives_p[i] = R_RGBA(
        hex2int(col[1]) * 16 + hex2int(col[2]),
        hex2int(col[3]) * 16 + hex2int(col[4]),
        hex2int(col[5]) * 16 + hex2int(col[6]),
        has_alpha ? hex2int(col[7]) * 16 + hex2int(col[8]) : 255
      );
    } else {
      ColourMap::iterator it = named_colours.find(prepare_code(col));
      if (it == named_colours.end()) {
        Rf_errorcall(R_NilValue, "Unknown colour name: %s", col);
        natives_p[i] = na_colour_int;
      } else {
        natives_p[i] = R_RGB(it->second.r, it->second.g, it->second.b);
      }
    }
  }
  if (TYPEOF(color) == STRSXP) {
    copy_names(color, natives);
  }
  UNPROTECT(1);
  return natives;
}

SEXP decode_native_c(SEXP native) {
  int n = Rf_length(native);
  SEXP codes = PROTECT(Rf_allocVector(STRSXP, n));
  char* buf = buffera;
  int* native_p = INTEGER(native);
  int num;
  
  for (int i = 0; i < n; ++i) {
    if (native_p[i] == R_NaInt) {
      SET_STRING_ELT(codes, i, R_NaString);
      continue;
    }
    num = R_RED(native_p[i]) * 2;
    buf[1] = hex8[num];
    buf[2] = hex8[num + 1];
    
    num = R_GREEN(native_p[i]) * 2;
    buf[3] = hex8[num];
    buf[4] = hex8[num + 1];
    
    num = R_BLUE(native_p[i]) * 2;
    buf[5] = hex8[num];
    buf[6] = hex8[num + 1];
    
    num = R_ALPHA(native_p[i]) * 2;
    if (num == 510) { // opaque
      buf[7] = '\0';
    } else {
      buf[7] = hex8[num];
      buf[8] = hex8[num + 1];
    }
    
    SET_STRING_ELT(codes, i, Rf_mkChar(buf));
  }
  
  copy_names(native, codes);
  UNPROTECT(1);
  return codes;
}
