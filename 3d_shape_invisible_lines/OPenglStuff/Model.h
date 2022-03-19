#pragma once
#include <vector>
#include "customGeometry3D.h"
struct dot {
	double data[3] = { 0,0,0};
	double& X() {
		return data[0];
	}
	double& Y() {
		return data[1];
	}
	double& Z() {
		return data[2];
	}
	void toDouble4(double out[]) {
		out[0] = data[0];
		out[1] = data[1];
		out[2] = data[2];
		out[3] = 1;
	}
	dot operator+(dot other) {
		return { X() + other.X(),Y() + other.Y(),Z() + other.Z() };
	}
	dot operator-(dot other) {
		return { X() - other.X(),Y() - other.Y(),Z() - other.Z() };
	}
	dot operator*(double value) {
		return { X() *value,Y() *value,Z() *value};
	}
	dot operator/(double value) {
		return { X() /value,Y() / value,Z() / value };
	}
	double module(){
		return sqrt(X() * X() + Y() * Y() + Z() * Z());
	}
	dot norm() {
		return this->operator/(module());
	}
};

struct line{
	int start;
	int end;
	bool seen;
};


class Model
{
	std::vector<dot>dots;
	std::vector<std::vector<int>> polygons;
	std::vector<std::pair<int,int>> lines;
	dot position{0,0,0};
	dot scale{1,1,1};
	dot rotation{0,0,0};

	dot uselessAxis{0,0,1};
	float uselessAngle{0};
	dot zeroCoord{ 0,0,0 };

	double aspect{1};
	double camera_start{ 2 };

	std::vector<bool> vision;
	std::vector<double>planes;
	std::vector<line >linesToDraw;
public:
	void setupDots(std::vector<dot> dots );
	void setupPlanes(std::vector<std::vector<int>> planes);
	void setupLines(std::vector<std::pair<int, int>> lines);
	void setScale(double x, double y, double z);
	void setPosition(double x, double y, double z);
	void setRotation(double x, double y, double z);
	void setAspect(double a) { aspect = a; };
	void setCamStart(double a) { camera_start = a; };
	void setupAxis(double x1, double y1, double z1, double x2, double y2, double z2);
	void setupRotAxis(double f) { uselessAngle = f; };
	std::vector<dot> getTransformedDots();
	void render();
	void debug();


};

