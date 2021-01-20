#include "ColorSpace.h"
#include "Conversion.h"
#include <R.h>
#define R_NO_REMAP
#include <Rinternals.h>

namespace ColorSpace {
	Rgb::Rgb() {}
	Rgb::Rgb(double r, double g, double b) : r(r), g(g), b(b) {
	  valid = R_finite(r) && R_finite(g) && R_finite(b);
	}
  Rgb::Rgb(int r, int g, int b) : r(r), g(g), b(b) {
    valid = !(r == R_NaInt || g == R_NaInt || b == R_NaInt);
  }
	void Rgb::Initialize(Rgb *color) {
		RgbConverter::ToColorSpace(color, this);
	}
	void Rgb::ToRgb(Rgb *color) {
		RgbConverter::ToColor(color, this);
	}
	void Rgb::Copy(IColorSpace *color) {
		Rgb *rgb = static_cast<Rgb*>(color);
		rgb->r = r;
		rgb->g = g;
		rgb->b = b;
		rgb->valid = valid;
	}
  void Rgb::Cap() {
    if (!valid) return;
    r = r < 0.0 ? 0.0 : (r > 255.0 ? 255.0 : r);
    g = g < 0.0 ? 0.0 : (g > 255.0 ? 255.0 : g);
    b = b < 0.0 ? 0.0 : (b > 255.0 ? 255.0 : b);
  }


	Xyz::Xyz() {}
	Xyz::Xyz(double x, double y, double z) : x(x), y(y), z(z) {
	  valid = R_finite(x) && R_finite(y) && R_finite(z);
	}
  Xyz::Xyz(int x, int y, int z) : x(x), y(y), z(z) {
    valid = !(x == R_NaInt || y == R_NaInt || z == R_NaInt);
  }
	void Xyz::Initialize(Rgb *color) {
		XyzConverter::ToColorSpace(color, this);
	}
	void Xyz::ToRgb(Rgb *color) {
		XyzConverter::ToColor(color, this);
	}
	void Xyz::Copy(IColorSpace *color) {
		Xyz *xyz = static_cast<Xyz*>(color);
		xyz->x = x;
		xyz->y = y;
		xyz->z = z;
		xyz->valid = valid;
	}
  void Xyz::Cap() {
    if (!valid) return;
    x = x < 0.0 ? 0.0 : x;
    y = y < 0.0 ? 0.0 : y;
    z = z < 0.0 ? 0.0 : z;
  }

	Hsl::Hsl() {}
	Hsl::Hsl(double h, double s, double l) : h(h), s(s), l(l) {
	  valid = R_finite(h) && R_finite(s) && R_finite(l);
	}
  Hsl::Hsl(int h, int s, int l) : h(h), s(s), l(l) {
    valid = !(h == R_NaInt || s == R_NaInt || l == R_NaInt);
  }
	void Hsl::Initialize(Rgb *color) {
		HslConverter::ToColorSpace(color, this);
	}
	void Hsl::ToRgb(Rgb *color) {
		HslConverter::ToColor(color, this);
	}
	void Hsl::Copy(IColorSpace *color) {
		Hsl *hsl = static_cast<Hsl*>(color);
		hsl->h = h;
		hsl->s = s;
		hsl->l = l;
		hsl->valid = valid;
	}
  void Hsl::Cap() {
    if (!valid) return;
	  h = h < 0.0 ? 0.0 : (h > 360.0 ? 360.0 : h);
	  s = s < 0.0 ? 0.0 : (s > 100.0 ? 100.0 : s);
	  l = l < 0.0 ? 0.0 : (l > 100.0 ? 100.0 : l);
	}

	Lab::Lab() {}
	Lab::Lab(double l, double a, double b) : l(l), a(a), b(b) {
	  valid = R_finite(l) && R_finite(a) && R_finite(b);
	}
  Lab::Lab(int l, int a, int b) : l(l), a(a), b(b) {
    valid = !(l == R_NaInt || a == R_NaInt || b == R_NaInt);
  }
	void Lab::Initialize(Rgb *color) {
		LabConverter::ToColorSpace(color, this);
	}
	void Lab::ToRgb(Rgb *color) {
		LabConverter::ToColor(color, this);
	}
	void Lab::Copy(IColorSpace *color) {
		Lab *lab = static_cast<Lab*>(color);
		lab->l = l;
		lab->a = a;
		lab->b = b;
		lab->valid = valid;
	}
  void Lab::Cap() {
    if (!valid) return;
    l = l < 0.0 ? 0.0 : (l > 100.0 ? 100.0 : l);
  }

	Lch::Lch() {}
	Lch::Lch(double l, double c, double h) : l(l), c(c), h(h) {
	  valid = R_finite(l) && R_finite(c) && R_finite(h);
	}
  Lch::Lch(int l, int c, int h) : l(l), c(c), h(h) {
    valid = !(l == R_NaInt || c == R_NaInt || h == R_NaInt);
  }
	void Lch::Initialize(Rgb *color) {
		LchConverter::ToColorSpace(color, this);
	}
	void Lch::ToRgb(Rgb *color) {
		LchConverter::ToColor(color, this);
	}
	void Lch::Copy(IColorSpace *color) {
		Lch *lch = static_cast<Lch*>(color);
		lch->l = l;
		lch->c = c;
		lch->h = h;
		lch->valid = valid;
	}
  void Lch::Cap() {
    if (!valid) return;
    l = l < 0.0 ? 0.0 : (l > 100.0 ? 100.0 : l);
    c = c < 0.0 ? 0.0 : c;
    h = h < 0.0 ? 0.0 : (h > 360.0 ? 360.0 : h);
  }

	Luv::Luv() {}
	Luv::Luv(double l, double u, double v) : l(l), u(u), v(v) {
	  valid = R_finite(l) && R_finite(u) && R_finite(v);
	}
  Luv::Luv(int l, int u, int v) : l(l), u(u), v(v) {
    valid = !(l == R_NaInt || u == R_NaInt || v == R_NaInt);
  }
	void Luv::Initialize(Rgb *color) {
		LuvConverter::ToColorSpace(color, this);
	}
	void Luv::ToRgb(Rgb *color) {
		LuvConverter::ToColor(color, this);
	}
	void Luv::Copy(IColorSpace *color) {
		Luv *luv = static_cast<Luv*>(color);
		luv->l = l;
		luv->u = u;
		luv->v = v;
		luv->valid = valid;
	}
  void Luv::Cap() {
    if (!valid) return;
    l = l < 0.0 ? 0.0 : (l > 100.0 ? 100.0 : l);
    
  }

	Yxy::Yxy() {}
	Yxy::Yxy(double y1, double x, double y2) : y1(y1), x(x), y2(y2) {
	  valid = R_finite(y1) && R_finite(x) && R_finite(y2);
	}
  Yxy::Yxy(int y1, int x, int y2) : y1(y1), x(x), y2(y2) {
    valid = !(y1 == R_NaInt || x == R_NaInt || y2 == R_NaInt);
  }
	void Yxy::Initialize(Rgb *color) {
		YxyConverter::ToColorSpace(color, this);
	}
	void Yxy::ToRgb(Rgb *color) {
		YxyConverter::ToColor(color, this);
	}
	void Yxy::Copy(IColorSpace *color) {
		Yxy *yxy = static_cast<Yxy*>(color);
		yxy->y1 = y1;
		yxy->x = x;
		yxy->y2 = y2;
		yxy->valid = valid;
	}
  void Yxy::Cap() {
    if (!valid) return;
    y1 = y1 < 0.0 ? 0.0 : (y1 > 100.0 ? 100.0 : y1);
    x = x < 0.0 ? 0.0 : x;
    y2 = y2 < 0.0 ? 0.0 : y2;
  }

	Cmy::Cmy() {}
	Cmy::Cmy(double c, double m, double y) : c(c), m(m), y(y) {
	  valid = R_finite(c) && R_finite(m) && R_finite(y);
	}
  Cmy::Cmy(int c, int m, int y) : c(c), m(m), y(y) {
    valid = !(c == R_NaInt || m == R_NaInt || y == R_NaInt);
  }
	void Cmy::Initialize(Rgb *color) {
		CmyConverter::ToColorSpace(color, this);
	}
	void Cmy::ToRgb(Rgb *color) {
		CmyConverter::ToColor(color, this);
	}
	void Cmy::Copy(IColorSpace *color) {
		Cmy *cmy = static_cast<Cmy*>(color);
		cmy->c = c;
		cmy->m = m;
		cmy->y = y;
		cmy->valid = valid;
	}
  void Cmy::Cap() {
    if (!valid) return;
    c = c < 0.0 ? 0.0 : (c > 1.0 ? 1.0 : c);
    m = m < 0.0 ? 0.0 : (m > 1.0 ? 1.0 : m);
    y = y < 0.0 ? 0.0 : (y > 1.0 ? 1.0 : y);
  }

	Cmyk::Cmyk() {}
	Cmyk::Cmyk(double c, double m, double y, double k) : c(c), m(m), y(y), k(k) {
	  valid = R_finite(c) && R_finite(m) && R_finite(y) && R_finite(k);
	}
  Cmyk::Cmyk(int c, int m, int y, int k) : c(c), m(m), y(y), k(k) {
    valid = !(c == R_NaInt || m == R_NaInt || y == R_NaInt || k == R_NaInt);
  }
	void Cmyk::Initialize(Rgb *color) {
		CmykConverter::ToColorSpace(color, this);
	}
	void Cmyk::ToRgb(Rgb *color) {
		CmykConverter::ToColor(color, this);
	}
	void Cmyk::Copy(IColorSpace *color) {
		Cmyk *cmyk = static_cast<Cmyk*>(color);
		cmyk->c = c;
		cmyk->m = m;
		cmyk->y = y;
		cmyk->k = k;
		cmyk->valid = valid;
	}
  void Cmyk::Cap() {
    if (!valid) return;
    c = c < 0.0 ? 0.0 : (c > 1.0 ? 1.0 : c);
    m = m < 0.0 ? 0.0 : (m > 1.0 ? 1.0 : m);
    y = y < 0.0 ? 0.0 : (y > 1.0 ? 1.0 : y);
    k = k < 0.0 ? 0.0 : (k > 1.0 ? 1.0 : k);
  }

	Hsv::Hsv() {}
	Hsv::Hsv(double h, double s, double v) : h(h), s(s), v(v) {
	  valid = R_finite(h) && R_finite(s) && R_finite(v);
	}
  Hsv::Hsv(int h, int s, int v) : h(h), s(s), v(v) {
    valid = !(h == R_NaInt || s == R_NaInt || v == R_NaInt);
  }
	void Hsv::Initialize(Rgb *color) {
		HsvConverter::ToColorSpace(color, this);
	}
	void Hsv::ToRgb(Rgb *color) {
		HsvConverter::ToColor(color, this);
	}
	void Hsv::Copy(IColorSpace *color) {
		Hsv *hsv = static_cast<Hsv*>(color);
		hsv->h = h;
		hsv->s = s;
		hsv->v = v;
		hsv->valid = valid;
	}
  void Hsv::Cap() {
    if (!valid) return;
    h = h < 0.0 ? 0.0 : (h > 360.0 ? 360.0 : h);
    s = s < 0.0 ? 0.0 : (s > 1.0 ? 1.0 : s);
    v = v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v);
  }

	Hsb::Hsb() {}
	Hsb::Hsb(double h, double s, double b) : h(h), s(s), b(b) {
	  valid = R_finite(h) && R_finite(s) && R_finite(b);
	}
  Hsb::Hsb(int h, int s, int b) : h(h), s(s), b(b) {
    valid = !(h == R_NaInt || s == R_NaInt || b == R_NaInt);
  }
	void Hsb::Initialize(Rgb *color) {
		HsbConverter::ToColorSpace(color, this);
	}
	void Hsb::ToRgb(Rgb *color) {
		HsbConverter::ToColor(color, this);
	}
	void Hsb::Copy(IColorSpace *color) {
		Hsb *hsb = static_cast<Hsb*>(color);
		hsb->h = h;
		hsb->s = s;
		hsb->b = b;
		hsb->valid = valid;
	}
  void Hsb::Cap() {
    if (!valid) return;
    h = h < 0.0 ? 0.0 : (h > 360.0 ? 360.0 : h);
    s = s < 0.0 ? 0.0 : (s > 1.0 ? 1.0 : s);
    b = b < 0.0 ? 0.0 : (b > 1.0 ? 1.0 : b);
  }

	HunterLab::HunterLab() {}
	HunterLab::HunterLab(double l, double a, double b) : l(l), a(a), b(b) {
	  valid = R_finite(l) && R_finite(a) && R_finite(b);
	}
  HunterLab::HunterLab(int l, int a, int b) : l(l), a(a), b(b) {
    valid = !(l == R_NaInt || a == R_NaInt || b == R_NaInt);
  }
	void HunterLab::Initialize(Rgb *color) {
		HunterLabConverter::ToColorSpace(color, this);
	}
	void HunterLab::ToRgb(Rgb *color) {
		HunterLabConverter::ToColor(color, this);
	}
	void HunterLab::Copy(IColorSpace *color) {
		HunterLab *lab = static_cast<HunterLab*>(color);
		lab->l = l;
		lab->a = a;
		lab->b = b;
		lab->valid = valid;
	}
  void HunterLab::Cap() {
    if (!valid) return;
    l = l < 0.0 ? 0.0 : (l > 100.0 ? 100.0 : l);
  }

  Hcl::Hcl() {}
  Hcl::Hcl(double h, double c, double l) : h(h), c(c), l(l) {
    valid = R_finite(h) && R_finite(c) && R_finite(l);
  }
  Hcl::Hcl(int h, int c, int l) : h(h), c(c), l(l) {
    valid = !(h == R_NaInt || c == R_NaInt || l == R_NaInt);
  }
  void Hcl::Initialize(Rgb *color) {
    HclConverter::ToColorSpace(color, this);
  }
  void Hcl::ToRgb(Rgb *color) {
    HclConverter::ToColor(color, this);
  }
  void Hcl::Copy(IColorSpace *color) {
    Hcl *hcl = static_cast<Hcl*>(color);
    hcl->l = l;
    hcl->c = c;
    hcl->h = h;
    hcl->valid = valid;
  }
  void Hcl::Cap() {
    if (!valid) return;
    h = h < 0.0 ? 0.0 : (h > 360.0 ? 360.0 : h);
    c = c < 0.0 ? 0.0 : c;
    l = l < 0.0 ? 0.0 : (l > 100.0 ? 100.0 : l);
  }

  OkLab::OkLab() {}
  OkLab::OkLab(double l, double a, double b) : l(l), a(a), b(b) {
    valid = R_finite(l) && R_finite(a) && R_finite(b);
  }
  OkLab::OkLab(int l, int a, int b) : l(l), a(a), b(b) {
    valid = !(l == R_NaInt || a == R_NaInt || b == R_NaInt);
  }
  void OkLab::Initialize(Rgb *color) {
    OkLabConverter::ToColorSpace(color, this);
  }
  void OkLab::ToRgb(Rgb *color) {
    OkLabConverter::ToColor(color, this);
  }
  void OkLab::Copy(IColorSpace *color) {
    OkLab *lab = static_cast<OkLab*>(color);
    lab->l = l;
    lab->a = a;
    lab->b = b;
    lab->valid = valid;
  }
  void OkLab::Cap() {
    if (!valid) return;
    l = l < 0.0 ? 0.0 : (l > 1.0 ? 1.0 : l);
  }

OkLch::OkLch() {}
OkLch::OkLch(double l, double c, double h) : l(l), c(c), h(h) {
  valid = R_finite(l) && R_finite(c) && R_finite(h);
}
OkLch::OkLch(int l, int c, int h) : l(l), c(c), h(h) {
  valid = !(l == R_NaInt || c == R_NaInt || h == R_NaInt);
}
void OkLch::Initialize(Rgb *color) {
  OkLchConverter::ToColorSpace(color, this);
}
void OkLch::ToRgb(Rgb *color) {
  OkLchConverter::ToColor(color, this);
}
void OkLch::Copy(IColorSpace *color) {
  OkLch *lch = static_cast<OkLch*>(color);
  lch->l = l;
  lch->c = c;
  lch->h = h;
  lch->valid = valid;
}
void OkLch::Cap() {
  if (!valid) return;
  l = l < 0.0 ? 0.0 : (l > 1.0 ? 1.0 : l);
}
}

