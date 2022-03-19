#include "customGeometry3D.h"
#include "cstring"
#include <math.h>
#define DEG_TO_RAD (M_PI / 180.f)

double cot(double degrees) {
	return tan(M_PI / 2.f - degrees * DEG_TO_RAD);
}


int customGeometry3D::coord2Dto1D(int x, int y, int xMax) {
	return y * xMax + x;
}
std::pair<int, int> customGeometry3D::coord1Dto2D(int i, int xMax) {
	return(std::pair<int, int>(i%xMax, i/xMax));
}


void customGeometry3D::multiplyMatrix4(double a[], double b[], double out[]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			double c = 0;
			for (int n = 0; n < 4; n++) {
				c += a[coord2Dto1D(n, j)] * b[coord2Dto1D(i, n)];
			}
				out[coord2Dto1D(i, j)]=c;
		}
	}
}

void customGeometry3D::multiplyMatrixDouble(double a[], double value) {
	for (int i = 0; i < 16; i++) {
		a[i] *= value;
	}
}

void customGeometry3D::multiplyMatrix4Dot( double matrix[], double dot[], double out[]) {

	for (int i = 0; i < 4; i++) {
			double c = 0;
			for (int n = 0; n < 4; n++) {
				c += matrix[coord2Dto1D(n, i)] * dot[n];
			}
		    out[i]=c;
	}
}


void customGeometry3D::makeTransposeMatrix(double x, double y, double z, double out[]) {
	//memset((double*)out, 0, 16);
	for (int i = 0; i < 16; i++)
		out[i] = 0;
	for (int i = 0; i < 4; i++) {
		out[coord2Dto1D(i, i)] = 1;
	}
	out[coord2Dto1D(3, 0)] = x;
	out[coord2Dto1D(3, 1)] = y;
	out[coord2Dto1D(3, 2)] = z;
}

void customGeometry3D::makeScaleMatrix(double x, double y, double z, double out[]) {
	for (int i = 0; i < 16; i++)
		out[i] = 0;
	//memset((double*)out, 0, 16);
	out[coord2Dto1D(0, 0)] = x;
	out[coord2Dto1D(1, 1)] = y;
	out[coord2Dto1D(2, 2)] = z;
	out[coord2Dto1D(3, 3)] = 1;
}


void customGeometry3D::makeRotationMatrix(double x, double y, double z, double out[]) {
	double xMatrix[16] = { 0 };
	double yMatrix[16] = { 0 };
	double zMatrix[16] = { 0 };

	for (int i = 0; i < 16; i++)
		out[i] = 0;
	
	double sinZ = sin(z * DEG_TO_RAD);
	double cosZ = cos(z * DEG_TO_RAD);
	zMatrix[coord2Dto1D(0, 0)] =
		zMatrix[coord2Dto1D(1, 1)] = cosZ;
	zMatrix[coord2Dto1D(0, 1)] = -sinZ;
	zMatrix[coord2Dto1D(1, 0)]  = sinZ;
	zMatrix[coord2Dto1D(2, 2)] =
		zMatrix[coord2Dto1D(3, 3)] = 1;


	double sinY = sin(y * DEG_TO_RAD);
	double cosY = cos(y * DEG_TO_RAD);
	yMatrix[coord2Dto1D(0, 0)] =
		yMatrix[coord2Dto1D(2, 2)] = cosY;
	yMatrix[coord2Dto1D(1, 1)] =
		yMatrix[coord2Dto1D(3, 3)] = 1;
	yMatrix[coord2Dto1D(2, 0)] = -sinY;
	yMatrix[coord2Dto1D(0, 2)] = sinY;

	double sinX = sin(x * DEG_TO_RAD);
	double cosX = cos(x * DEG_TO_RAD);
	xMatrix[coord2Dto1D(1, 1)] =
		xMatrix[coord2Dto1D(2, 2)] = cosX;
	xMatrix[coord2Dto1D(0, 0)] =
		xMatrix[coord2Dto1D(3, 3)] = 1;
	xMatrix[coord2Dto1D(1, 2)] = -sinX;
	xMatrix[coord2Dto1D(2, 1)] = sinX;


	double bufer[16];
	multiplyMatrix4(zMatrix, yMatrix, bufer);
	multiplyMatrix4(bufer, xMatrix, out);


}


void customGeometry3D::makeAxisRotationMatrix(double x, double y, double z, double angle, double out[]) {
	double sinf = sin(DEG_TO_RAD * angle);
	double cosf = cos(DEG_TO_RAD * angle);
	double Vf = 1 -cosf;

	out[coord2Dto1D(0, 0)] = x * x * Vf + cosf;
	out[coord2Dto1D(1, 0)] = x * y * Vf - z*sinf;
	out[coord2Dto1D(2, 0)] = x * z * Vf + y* sinf;
	out[coord2Dto1D(3, 0)] = 0;
	out[coord2Dto1D(0, 1)] = x * y * Vf + z*sinf;
	out[coord2Dto1D(1, 1)] = y * y * Vf + cosf;
	out[coord2Dto1D(2, 1)] = y * z * Vf - x*sinf;
	out[coord2Dto1D(3, 1)] = 0;
	out[coord2Dto1D(0, 2)] = z * x * Vf - y*sinf;
	out[coord2Dto1D(1, 2)] = z * y * Vf + x*sinf;
	out[coord2Dto1D(2, 2)] = z * z * Vf + cosf;
	out[coord2Dto1D(3, 2)] = 0;
	out[coord2Dto1D(0, 3)] = 0;
	out[coord2Dto1D(1, 3)] = 0;
	out[coord2Dto1D(2, 3)] = 0;
	out[coord2Dto1D(3, 3)] = 1;
}




void customGeometry3D::makeProjectionMatrix(double fov, double ar, double n, double f, double out[]) {
	double d = cot(fov / 2);
	for (int i = 0; i < 16; i++)
		out[i] = 0;
	double deltaz = n - f;
	out[coord2Dto1D(0, 0)] = d/ar;
	out[coord2Dto1D(1,1)] = d;
	out[coord2Dto1D(2, 2)] = (n+f)/deltaz;
	out[coord2Dto1D(3, 2)] = 2*n*f/ deltaz;
	out[coord2Dto1D(2, 3)] = 1;
	
}
void customGeometry3D::flFrustum(double n, double f, double l, double r, double t, double b, double out[]) {
	for (int i = 0; i < 16; i++)
		out[i] = 0;
	out[coord2Dto1D(0, 0)] = 2*n/(r-l);
	out[coord2Dto1D(1, 1)] = 2*n/(t-b);
	out[coord2Dto1D(2, 2)] = (n + f) / (n-f);
	out[coord2Dto1D(3, 2)] = 2 * n * f / (n-f);
	out[coord2Dto1D(2, 3)] = -1;
	out[coord2Dto1D(2, 0)] = (r+l)/(r-l);
	out[coord2Dto1D(2, 1)] = (t + b) / (t - b);
}

void customGeometry3D::makeOrientationMatrix(double sideX, double sideY, double sideZ, double upX, double upY, double upZ, double forwardX, double forwardY, double forwardZ, double out[]) {
	for (int i = 0; i < 16; i++)
		out[i] = 0;
	out[coord2Dto1D(0, 0)] = sideX;
	out[coord2Dto1D(0, 1)] = sideY;
	out[coord2Dto1D(0, 2)] = sideZ;

	out[coord2Dto1D(1, 0)] = upX;
	out[coord2Dto1D(1, 1)] = upY;
	out[coord2Dto1D(1, 2)] = upZ;

	out[coord2Dto1D(2, 0)] = forwardX;
	out[coord2Dto1D(2, 1)] = forwardY;
	out[coord2Dto1D(2, 2)] = forwardZ;
	out[coord2Dto1D(3, 3)] = 1;

}







