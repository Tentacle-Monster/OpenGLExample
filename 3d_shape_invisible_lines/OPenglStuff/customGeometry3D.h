#pragma once
#include <utility>
#define _USE_MATH_DEFINES

class customGeometry3D
{
public:
	static inline int coord2Dto1D(int x, int y, int xMax  =4);
	static inline std::pair<int,int> coord1Dto2D(int i,int xMax = 4);
	static void multiplyMatrix4(double a[], double b[], double out[]);
	static void multiplyMatrixDouble(double a[], double value);
	static void multiplyMatrix4Dot( double matrix[], double dot[], double out[]);
	static void makeTransposeMatrix(double x, double y, double z, double out[]);
	static void makeScaleMatrix(double x, double y, double z, double out[]);
	static void makeRotationMatrix(double x, double y, double z, double out[]);
	static void makeAxisRotationMatrix(double x, double y, double z, double angle, double out[]);
	//static void makeProjectionMatrix(double FOV, double Aspect, double NearZ, double FarZ, double out[]);
	//static void makeProjectionMatrix(double x, double y, double z, double out[]);
	//static void makeProjectionMatrix(double fovY, double zf, double zn, double aspect_ratio, double W, double H, double X, double Y, double out[]);
	static void makeProjectionMatrix(double fov, double ar, double n, double f,double out[]);
	static void flFrustum(double n, double f, double l, double r, double t, double b, double out[]);
	static void makeOrientationMatrix(double sideX, double sideY, double sideZ, double upX, double upY, double upZ, double forwardX, double forwardY, double forwardZ, double out[] );
};

