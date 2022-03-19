#include "Model.h"
#include<freeglut.h>
#include <iostream>


//#define debug_draw
//#define out_position

void Model::setupDots(std::vector<dot> dots) {
	this->dots = dots;
}
void Model::setupPlanes(std::vector<std::vector<int>> planes) {
	this->polygons = planes;
}

void Model::setupLines(std::vector<std::pair<int, int>> lines) {
	this->lines = lines;
}

void Model::setupAxis(double x1, double y1, double z1, double x2, double y2, double z2) {
	dot a{ x1,y1,z1 };
	dot b{ x2,y2,z2 };
	uselessAxis = (b - a).norm();
	zeroCoord = a+ (b-a)/2;
	for (int i = 0; i < dots.size(); i++) {
		dots[i] = dots[i] - zeroCoord;
	}
}


bool getPlane(std::vector<dot> points , double out[]) {
	if (points.size() < 3) {
		for (int i = 0; i < 4; i++)
			out[i] = NAN;
		return false;
	}
		dot vek1 = points[1] - points[0];
	dot vek2 = points[2] - points[0];
	double additionX = vek1.Y() * vek2.Z() - vek1.Z() * vek2.Y();
	double additionY = -( vek1.X() * vek2.Z() - vek2.X() * vek1.Z());
	double additionZ = vek1.X() * vek2.Y() - vek2.X() * vek1.Y();
	double D = -points[0].X() * additionX - points[0].Y() * additionY - points[0].Z() * additionZ;
	
	out[0] = additionX;
	out[1] = additionY;
	out[2] = additionZ;
	out[3] = D;
	return true;
}

double getPlaneZ(double x, double y, double plane[]) {
	return (0 -( x * plane[0] + y * plane[1] + plane[3])) / plane[2];
}



dot intersection2D(dot A, dot B, dot C, dot D) {
	dot delta1 = B - A;
	dot delta2 = D - C;

	auto dx1 = delta1.X();
	auto dy1 = delta1.Y();
	auto m1 = dy1 / dx1;
	auto c1 = A.Y() - m1 * A.X(); // which is same as y2 - slope * x2


	auto dx2 = delta2.X();
	auto dy2 = delta2.Y();
	auto m2 = dy2 / dx2;
	auto c2 = C.Y() - m2 * C.X();

	if ((m1 - m2) == 0) {
	//	std::cout << "No Intersection between the lines\n";
		return { NULL,NULL,NULL };

	}

	else

	{
		auto intersection_X = (c2 - c1) / (m1 - m2);
		auto intersection_Y = m1 * intersection_X + c1;
		return { intersection_X,intersection_Y,0 };
	}
}



bool ccw(dot A, dot B, dot C) {
	return (C.Y() - A.Y()) * (B.X() - A.X()) > (B.Y() - A.Y()) * (C.X() - A.X());
}




bool ifLinesCrosing2D(dot A, dot B, dot C, dot D) {
	/*dot i = intersection2D(A, B, C, D);
	return(i.X() == i.X());*/
	return ccw(A, C, D) != ccw(B, C, D) and ccw(A, B, C) != ccw(A, B, D);
}


bool isDotOnLine2D(dot a, dot b, dot c) {
/*	double zero = 0.01;
	dot delta = c - b;
	double coefA = delta.Y() / delta.X();
	double coefB = b.Y();

	return (fabs((a.Y() - coefB) / coefA - a.X()) < zero);
	*/
	dot a1{ 0.001, 0.001, 0 };
	return ifLinesCrosing2D(a-a1, a+a1 , b, c);
}

bool dotInPolyline(dot point, std::vector<dot> polyline) {
	dot addition{ 500.f,0,0 };
	dot lineEnd = point + addition;
	int intercect = 0;
	for (int i = 0; i < polyline.size(); i++) {
		if (ifLinesCrosing2D(point, lineEnd, polyline[i], polyline[(i + 1) % polyline.size()]))
			intercect++;
	}
	return intercect & 1;
}

double range2D(dot a, dot b) {
	dot r = a-b;
	return sqrt(r.X() * r.X() + r.Y() * r.Y());
}

dot getLineMiddle(dot a, dot b) {
	return a + (b - a)/2;
}



//==============================================================splitting lines============
void Model::render() {
	auto transposed = getTransformedDots();
	linesToDraw.clear();
	for (int i = 0; i < lines.size(); i++) {
		linesToDraw.push_back({ lines[i].first,lines[i].second,true });
	}
	for(int i=0; i< linesToDraw.size();i++)
		for (int i1 = i; i1 < linesToDraw.size(); i1++) {
			
			dot a1 = transposed[linesToDraw[i].start];
			dot a2 = transposed[linesToDraw[i].end];
			dot b1 = transposed[linesToDraw[i1].start];
			dot b2 = transposed[linesToDraw[i1].end];
			if (range2D(a1, b1) < 0.0001 || range2D(a2, b1) < 0.0001 || range2D(a1, b2) < 0.0001 || range2D(a2, b2) < 0.0001)
				continue;

			if (isDotOnLine2D(a1, b1, b2)
				|| isDotOnLine2D(a2, b1, b2)
				|| isDotOnLine2D(b1, a1, a2)
				|| isDotOnLine2D(b2, a1, a2)
				)continue;

			if (!ifLinesCrosing2D(a1, a2, b1, b2))
				continue;

			
			dot crossover1 = intersection2D(transposed[linesToDraw[i].start], transposed[linesToDraw[i].end], transposed[linesToDraw[i1].start], transposed[linesToDraw[i1].end]);
			dot crossover2 = crossover1;

			if (crossover1.X() == crossover1.X() && crossover1.Y() == crossover1.Y() && isDotOnLine2D(crossover1, transposed[linesToDraw[i].start], transposed[linesToDraw[i].end])) {
				dot delta1 = transposed[linesToDraw[i].end] - transposed[linesToDraw[i].start];
				dot delta2 = transposed[linesToDraw[i1].end] - transposed[linesToDraw[i1].start];
				crossover1.Z() = (transposed[linesToDraw[i].start].Z() + (delta1.Z() * (crossover1.X() - transposed[linesToDraw[i].start].X()) / delta1.X()));
				crossover2.Z() = (transposed[linesToDraw[i1].start].Z() + (delta2.Z() * (crossover2.X() - transposed[linesToDraw[i1].start].X()) / delta2.X()));
				transposed.push_back(crossover1);
				transposed.push_back(crossover2);
				linesToDraw.push_back({ (int)transposed.size() - 2 ,linesToDraw[i].end,true });
				linesToDraw.push_back({ (int)transposed.size() - 1 ,linesToDraw[i1].end,true });
				linesToDraw[i].end = transposed.size() - 2;
				linesToDraw[i1].end = transposed.size() - 1;
			}


		}


	//===================================checking 4 z-index===================
	planes.clear();
	planes.resize(polygons.size() * 4);  // getting planes equations
	for (int i = 0; i < polygons.size(); i++) {
		std::vector<dot> c;
		for (auto f : polygons[i]) {
			c.push_back(transposed[f]);
			}
		getPlane(c, planes.data()+i*4);
	}
	
	this->vision.resize(dots.size());
	for (int i = 0; i < dots.size(); i++)
		vision[i] = 1;
	
	for (int dot1 = 0; dot1 < dots.size(); dot1++) {
		for (int plane1 = 0; plane1 < polygons.size(); plane1++) {
			if (std::find(polygons[plane1].begin(), polygons[plane1].end(), dot1) != polygons[plane1].end())continue;

			std::vector<dot> c;
			for (auto f : polygons[plane1]) {
				c.push_back(transposed[f]);
			}
			if (dotInPolyline(transposed[dot1], c)) {
				double planeZ = getPlaneZ(transposed[dot1].X(), transposed[dot1].Y(), planes.data() +( plane1 * 4));
				if (planeZ > transposed[dot1].Z())
					vision[dot1] = 0;
			}
		}
	}
	

	for (int line1 = 0; line1 < linesToDraw.size(); line1++) {
		for (int plane1 = 0; plane1 < polygons.size(); plane1++) {

			std::vector<dot> c;

			for (auto f : polygons[plane1]) {
				c.push_back(transposed[f]);
			}

			dot a = transposed[linesToDraw[line1].start];
			dot b = transposed[linesToDraw[line1].end];
			dot mid = getLineMiddle(a,b);
		
			if (dotInPolyline(mid, c)) {
				double planeZ = getPlaneZ(mid.X(), mid.Y(), planes.data() + (plane1 * 4));
				if (planeZ - mid.Z() >0.0001)
					linesToDraw[line1].seen = false;
			}
		
		}
	}


	//========================================================================

#ifdef debug_draw


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	gluLookAt(0, 0, 0, 0, 0, -500, 0, 1, 0);

	glDepthFunc(GL_LEQUAL);
	glTranslatef(position.X(), position.Y(), position.Z());
	glScalef(scale.X() * 2, scale.Y() * 2, scale.Z() * 2);

	glRotatef(rotation.Z(), 0, 0, 1);
	glRotatef(rotation.Y(), 0, 1, 0);
	glRotatef(-rotation.X(), 1, 0, 0);

	glFrustum(-4, -6, -1, 1, 1, -1);
	glPushMatrix();

	double f = 1.f / polygons.size();
	double l = 0;

	for (auto n : polygons) {
		glColor3f(l, 1 , 1.f - 4.f * fmod(l, 0.25));
		l += f;
		glBegin(GL_POLYGON);
		for (auto dot : n) {
			glVertex3d(dots[dot].X(), dots[dot].Y(), dots[dot].Z());
		}
		glEnd();
	}



#endif // debug_draw



#ifndef debug_draw
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 0, 0);
	glPushAttrib(GL_ENABLE_BIT);
	glLineStipple(1, 0xA0A0);
	glEnable(GL_LINE_STIPPLE);
	for (auto n : linesToDraw) {
		if (!n.seen)
		{
			glBegin(GL_LINES);
			glVertex2d(transposed[n.start].X(), transposed[n.start].Y());
			glVertex2d(transposed[n.end].X(), transposed[n.end].Y());
			glEnd();
		}
	}
	glPopAttrib();
	glColor3f(0, 1, 0.2);
	for (auto n : linesToDraw) {
		if (n.seen)
		{
			glBegin(GL_LINES);
			glVertex2d(transposed[n.start].X(), transposed[n.start].Y());
			glVertex2d(transposed[n.end].X(), transposed[n.end].Y());
			glEnd();
		}
	}
#endif // !debug_draw
}

void Model::debug() {
	auto transposed = getTransformedDots();
	for (auto n : polygons) {
		std::cout << "glBegin(GL_LINE_LOOP);" << std::endl;

		for (auto dot : n) {
			std::cout << "glVertex2d("<< transposed[dot].X()<< "," << transposed[dot].Y()<<");" << std::endl;
		}
		std::cout << "glEnd();" << std::endl;
		

	}
}


void Model::setScale(double x, double y, double z) {
	scale = { x,y,z };
}
void Model::setPosition(double x, double y, double z) {
	position = { x,y,z };
}
void Model::setRotation(double x, double y, double z) {
	rotation = { x,y,z };
}
std::vector<dot> Model::getTransformedDots() {
	std::vector<dot> result(dots.size());
	double transposeMatrix[16];
	double scaleMatrix[16];
	double projMatrix[16];
	double orientationMatrix[16]; 
	double axisRotateMatrix[16];
	double zeroCoordsMatrix[16];
	

	double rotateMatrix[16];
	customGeometry3D::makeOrientationMatrix(1,0,0,0,1,0,0,0,1  , orientationMatrix);
	customGeometry3D::makeProjectionMatrix(90.f, aspect, camera_start, camera_start -6, projMatrix);
	customGeometry3D::makeTransposeMatrix(position.X(), position.Y(), position.Z(), transposeMatrix);
	customGeometry3D::makeScaleMatrix(scale.X(), scale.Y(), scale.Z(), scaleMatrix);
	customGeometry3D::makeRotationMatrix(rotation.X(), rotation.Y(), rotation.Z(),rotateMatrix);
	customGeometry3D::makeAxisRotationMatrix(uselessAxis.X(), uselessAxis.Y(), uselessAxis.Z(), uselessAngle, axisRotateMatrix);


	double transformMatrix[16] =
	{   1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1};
	double transformBuffer[16];

	
	customGeometry3D::multiplyMatrix4(transformMatrix, transposeMatrix, transformBuffer);
	memcpy(transformMatrix, transformBuffer, 16 * sizeof(double));
	
	customGeometry3D::multiplyMatrix4(transformMatrix, rotateMatrix, transformBuffer);
	memcpy(transformMatrix, transformBuffer, 16 * sizeof(double));
	
	customGeometry3D::multiplyMatrix4(transformMatrix, axisRotateMatrix, transformBuffer);
	memcpy(transformMatrix, transformBuffer, 16 * sizeof(double));
	
	customGeometry3D::multiplyMatrix4(transformMatrix, scaleMatrix, transformBuffer);
	memcpy(transformMatrix, transformBuffer, 16 * sizeof(double));

	for (unsigned int i = 0; i < dots.size(); i++) {
		double point[4];
		double bufer[4];
		dots[i].toDouble4(point);
		customGeometry3D::multiplyMatrix4Dot(transformMatrix, point, bufer);
		memcpy(point, bufer, 4 * sizeof(double));
		customGeometry3D::multiplyMatrix4Dot(projMatrix, point, bufer);
		memcpy(point, bufer, 4 * sizeof(double));
		result[i] = { {point[0]/point[3],point[1]/point[3],point[2]/point[3]} };
	};
	return result;
}