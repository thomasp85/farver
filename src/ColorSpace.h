#ifndef COLOR_SPACE_H
#define COLOR_SPACE_H

#include <typeinfo>
#include "Conversion.h"

namespace ColorSpace {
	struct IColorSpace {
		IColorSpace() {}
		virtual ~IColorSpace() {}

		virtual void Initialize(Rgb *color) = 0;
		virtual void ToRgb(Rgb *color) = 0;
		virtual void Copy(IColorSpace *color) = 0;
		virtual void Cap() = 0;
		template <typename TColorSpace>
		void To(TColorSpace *color);
		
		bool valid = true;
	};


	struct Rgb : public IColorSpace {
		double r, g, b;

		Rgb();
		Rgb(double r, double g, double b);
		Rgb(int r, int g, int b);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};
	
	template <typename TColorSpace>
	void IColorSpace::To(TColorSpace *color) {
	  Rgb rgb;
	  
	  if (typeid(*this) == typeid(*color)) {
	    this->Copy(color);
	  }
	  else {
	    this->ToRgb(&rgb);
	    IConverter<TColorSpace>::ToColorSpace(&rgb, color);
	  }
	}
	
	struct Xyz : public IColorSpace {
		double x, y, z;

		Xyz();
		Xyz(double x, double y, double z);
		Xyz(int x, int y, int z);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};

	struct Hsl : public IColorSpace {
		double h, s, l;

		Hsl();
		Hsl(double h, double s, double l);
		Hsl(int h, int s, int l);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};

	struct Lab : public IColorSpace {
		double l, a, b;

		Lab();
		Lab(double l, double a, double b);
		Lab(int l, int a, int b);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};

	struct Lch : public IColorSpace {
		double l, c, h;

		Lch();
		Lch(double l, double c, double h);
		Lch(int l, int c, int h);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};

	struct Luv : public IColorSpace {
		double l, u, v;

		Luv();
		Luv(double l, double u, double v);
		Luv(int l, int u, int v);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};

	struct Yxy : public IColorSpace {
		double y1, x, y2;

		Yxy();
		Yxy(double y1, double x, double y2);
		Yxy(int y1, int x, int y2);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};

	struct Cmy : public IColorSpace {
		double c, m, y;

		Cmy();
		Cmy(double c, double m, double y);
		Cmy(int c, int m, int y);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};

	struct Cmyk : public IColorSpace {
		double c, m, y, k;

		Cmyk();
		Cmyk(double c, double m, double y, double k);
		Cmyk(int c, int m, int y, int k);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};

	struct Hsv : public IColorSpace {
		double h, s, v;

		Hsv();
		Hsv(double h, double s, double v);
		Hsv(int h, int s, int v);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};

	struct Hsb : public IColorSpace {
		double h, s, b;

		Hsb();
		Hsb(double h, double s, double b);
		Hsb(int h, int s, int b);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};

	struct HunterLab : public IColorSpace {
		double l, a, b;

		HunterLab();
		HunterLab(double l, double a, double b);
		HunterLab(int l, int a, int b);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
		virtual void Cap();
	};
	
	struct Hcl : public IColorSpace {
	  double h, c, l;
	  
	  Hcl();
	  Hcl(double h, double c, double l);
	  Hcl(int h, int c, int l);
	  
	  virtual void Initialize(Rgb *color);
	  virtual void ToRgb(Rgb *color);
	  virtual void Copy(IColorSpace *color);
	  virtual void Cap();
	};
	
	struct OkLab : public IColorSpace {
	  double l, a, b;
	  
	  OkLab();
	  OkLab(double l, double a, double b);
	  OkLab(int l, int a, int b);
	  
	  virtual void Initialize(Rgb *color);
	  virtual void ToRgb(Rgb *color);
	  virtual void Copy(IColorSpace *color);
	  virtual void Cap();
	};
	
	struct OkLch : public IColorSpace {
	  double l, c, h;
	  
	  OkLch();
	  OkLch(double l, double c, double h);
	  OkLch(int l, int c, int h);
	  
	  virtual void Initialize(Rgb *color);
	  virtual void ToRgb(Rgb *color);
	  virtual void Copy(IColorSpace *color);
	  virtual void Cap();
	};
	
}

#endif // COLOR_SPACE_H

