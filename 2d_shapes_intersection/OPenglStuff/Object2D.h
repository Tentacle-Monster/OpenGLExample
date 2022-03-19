#pragma once

#include <vector>
#include <utility>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#define DEG_TO_RAD 0.0174532925199

struct dot
{
	double data[2]{ 0,0 };
	double& X() { return data[0]; };
	double& Y() { return data[1]; };
	dot operator+(dot other) {
		return { X() + other.X(),Y() + other.Y()};
	}
	dot operator-(dot other) {
		return { X() - other.X(),Y() - other.Y() };
	}
	dot operator*(double value) {
		return { X() * value,Y() * value };
	}
	dot operator/(double value) {
		return { X() / value,Y() / value };
	}
	double module() {
		return sqrt(X() * X() + Y() * Y() );
	}
	dot norm() {
		return this->operator/(module());
	}
	double operator*(dot other) {
		return  X() * other.X() + Y() * other.Y() ;
	}
};

typedef struct Color {
	double R{ 0 };
	double G{ 0 };
	double B{ 0 };
};
typedef struct LineToDraw {
	int dot1;
	int dot2;
	bool visible;
};
class Object2D
{
public:
	std::vector<dot> conture;
	std::vector<dot> recalculatedShape;
	std::vector<dot> dotsToDraw;
	std::vector<LineToDraw> linesToDraw;
	Color color;
	dot position{0,0};
	double angle{0};
	double scale{1};

	void recalculate();
	static double rangeToLine(double x, double y, double x1, double y1, double x2, double y2);


	static bool dotInPolyline(dot point, std::vector<dot> polyline);
	static bool ifLinesCrosing2D(dot A, dot B, dot C, dot D);
	void init_graphics();
	static Object2D ellipse(double xToY, int dots);

private:

	static void getLine(double x1, double y1, double x2, double y2, double& a, double& b, double& c);
	static bool ccw(dot A, dot B, dot C);


	
};

