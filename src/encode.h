#ifndef ENCODE_INCLUDED
#define ENCODE_INCLUDED

#include <unordered_map>
#include <string>
#include "ColorSpace.h"
#include <R.h>
#define R_NO_REMAP
#include <Rinternals.h>

#define OP_SET 1
#define OP_ADD 2
#define OP_MULT 3
#define OP_LEAST 4
#define OP_MOST 5

inline double mod_val(double val, double mod, int op) {
  switch (op) {
  case OP_SET: return mod;
  case OP_ADD: return val + mod;
  case OP_MULT: return val * mod;
  case OP_LEAST: return val < mod ? mod : val;
  case OP_MOST: return val > mod ? mod : val;
  }
  return val;
}

struct rgb_colour {
  int r;
  int g;
  int b;
  int a;
};
typedef std::unordered_map<std::string, rgb_colour> ColourMap;

// Defined in init.cpp
ColourMap& get_named_colours();

SEXP encode_c(SEXP colour, SEXP alpha, SEXP from, SEXP white, SEXP out_fmt, SEXP na_colour);
SEXP decode_c(SEXP codes, SEXP alpha, SEXP to, SEXP white, SEXP na);
SEXP encode_channel_c(SEXP codes, SEXP channel, SEXP value, SEXP space, SEXP op, SEXP white, SEXP na);
SEXP decode_channel_c(SEXP codes, SEXP channel, SEXP space, SEXP white, SEXP na);
SEXP load_colour_names_c(SEXP name, SEXP value);
SEXP encode_native_c(SEXP color, SEXP na_colour);
SEXP decode_native_c(SEXP native);
SEXP replace_alpha_native_c(SEXP colour, SEXP alpha);

template <typename Space>
inline void modify_channel(Space&, double value, int channel, int op);

template <>
inline void modify_channel<ColorSpace::Rgb>(ColorSpace::Rgb& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.r = mod_val(color.r, value, op);
    break;
  case 2:
    color.g = mod_val(color.g, value, op);
    break;
  case 3:
    color.b = mod_val(color.b, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::Xyz>(ColorSpace::Xyz& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.x = mod_val(color.x, value, op);
    break;
  case 2:
    color.y = mod_val(color.y, value, op);
    break;
  case 3:
    color.z = mod_val(color.z, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::Hsl>(ColorSpace::Hsl& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.h = mod_val(color.h, value, op);
    break;
  case 2:
    color.s = mod_val(color.s, value, op);
    break;
  case 3:
    color.l = mod_val(color.l, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::Lab>(ColorSpace::Lab& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.l = mod_val(color.l, value, op);
    break;
  case 2:
    color.a = mod_val(color.a, value, op);
    break;
  case 3:
    color.b = mod_val(color.b, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::Lch>(ColorSpace::Lch& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.l = mod_val(color.l, value, op);
    break;
  case 2:
    color.c = mod_val(color.c, value, op);
    break;
  case 3:
    color.h = mod_val(color.h, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::Luv>(ColorSpace::Luv& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.l = mod_val(color.l, value, op);
    break;
  case 2:
    color.u = mod_val(color.u, value, op);
    break;
  case 3:
    color.v = mod_val(color.v, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::Yxy>(ColorSpace::Yxy& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.y1 = mod_val(color.y1, value, op);
    break;
  case 2:
    color.x = mod_val(color.x, value, op);
    break;
  case 3:
    color.y2 = mod_val(color.y2, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::Cmy>(ColorSpace::Cmy& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.c = mod_val(color.c, value, op);
    break;
  case 2:
    color.m = mod_val(color.m, value, op);
    break;
  case 3:
    color.y = mod_val(color.y, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::Cmyk>(ColorSpace::Cmyk& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.c = mod_val(color.c, value, op);
    break;
  case 2:
    color.m = mod_val(color.m, value, op);
    break;
  case 3:
    color.y = mod_val(color.y, value, op);
    break;
  case 4:
    color.k = mod_val(color.k, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::Hsv>(ColorSpace::Hsv& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.h = mod_val(color.h, value, op);
    break;
  case 2:
    color.s = mod_val(color.s, value, op);
    break;
  case 3:
    color.v = mod_val(color.v, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::Hsb>(ColorSpace::Hsb& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.h = mod_val(color.h, value, op);
    break;
  case 2:
    color.s = mod_val(color.s, value, op);
    break;
  case 3:
    color.b = mod_val(color.b, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::HunterLab>(ColorSpace::HunterLab& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.l = mod_val(color.l, value, op);
    break;
  case 2:
    color.a = mod_val(color.a, value, op);
    break;
  case 3:
    color.b = mod_val(color.b, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::Hcl>(ColorSpace::Hcl& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.h = mod_val(color.h, value, op);
    break;
  case 2:
    color.c = mod_val(color.c, value, op);
    break;
  case 3:
    color.l = mod_val(color.l, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::OkLab>(ColorSpace::OkLab& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.l = mod_val(color.l, value, op);
    break;
  case 2:
    color.a = mod_val(color.a, value, op);
    break;
  case 3:
    color.b = mod_val(color.b, value, op);
    break;
  }
}

template <>
inline void modify_channel<ColorSpace::OkLch>(ColorSpace::OkLch& color, double value, int channel, int op){
  switch (channel) {
  case 1: 
    color.l = mod_val(color.l, value, op);
    break;
  case 2:
    color.c = mod_val(color.c, value, op);
    break;
  case 3:
    color.h = mod_val(color.h, value, op);
    break;
  }
}


template <typename Space>
inline double grab_channel(Space&,int channel);

template <>
inline double grab_channel<ColorSpace::Rgb>(ColorSpace::Rgb& color, int channel){
  switch (channel) {
  case 1: return color.r;
  case 2: return color.g;
  case 3: return color.b;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::Xyz>(ColorSpace::Xyz& color, int channel){
  switch (channel) {
  case 1: return color.x;
  case 2: return color.y;
  case 3: return color.z;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::Hsl>(ColorSpace::Hsl& color, int channel){
  switch (channel) {
  case 1: return color.h;
  case 2: return color.s;
  case 3: return color.l;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::Lab>(ColorSpace::Lab& color, int channel){
  switch (channel) {
  case 1: return color.l;
  case 2: return color.a;
  case 3: return color.b;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::Lch>(ColorSpace::Lch& color, int channel){
  switch (channel) {
  case 1: return color.l;
  case 2: return color.c;
  case 3: return color.h;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::Luv>(ColorSpace::Luv& color, int channel){
  switch (channel) {
  case 1: return color.l;
  case 2: return color.u;
  case 3: return color.v;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::Yxy>(ColorSpace::Yxy& color, int channel){
  switch (channel) {
  case 1: return color.y1;
  case 2: return color.x;
  case 3: return color.y2;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::Cmy>(ColorSpace::Cmy& color, int channel){
  switch (channel) {
  case 1: return color.c;
  case 2: return color.m;
  case 3: return color.y;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::Cmyk>(ColorSpace::Cmyk& color, int channel){
  switch (channel) {
  case 1: return color.c;
  case 2: return color.m;
  case 3: return color.y;
  case 4: return color.k;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::Hsv>(ColorSpace::Hsv& color, int channel){
  switch (channel) {
  case 1: return color.h;
  case 2: return color.s;
  case 3: return color.v;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::Hsb>(ColorSpace::Hsb& color, int channel){
  switch (channel) {
  case 1: return color.h;
  case 2: return color.s;
  case 3: return color.b;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::HunterLab>(ColorSpace::HunterLab& color, int channel){
  switch (channel) {
  case 1: return color.l;
  case 2: return color.a;
  case 3: return color.b;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::Hcl>(ColorSpace::Hcl& color, int channel){
  switch (channel) {
  case 1: return color.h;
  case 2: return color.c;
  case 3: return color.l;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::OkLab>(ColorSpace::OkLab& color, int channel){
  switch (channel) {
  case 1: return color.l;
  case 2: return color.a;
  case 3: return color.b;
  }
  return 0.0;
}

template <>
inline double grab_channel<ColorSpace::OkLch>(ColorSpace::OkLch& color, int channel){
  switch (channel) {
  case 1: return color.l;
  case 2: return color.c;
  case 3: return color.h;
  }
  return 0.0;
}

#endif