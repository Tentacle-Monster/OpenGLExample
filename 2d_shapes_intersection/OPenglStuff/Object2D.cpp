#include "Object2D.h"

	void Object2D::recalculate() {
		recalculatedShape.clear();
		recalculatedShape.resize(conture.size());
		double sina = sin(angle * DEG_TO_RAD);
		double cosa =  cos(angle * DEG_TO_RAD);

		for (int i = 0; i < conture.size(); i++) {
			double dot_x = conture[i].X();
			double dot_y = conture[i].Y();
			recalculatedShape[i ].X() = position.X() + (scale * (dot_x * cosa - dot_y * sina));
			recalculatedShape[i ].Y() = position.Y() + (scale *( dot_x *  sina + dot_y * cosa));
		}
	}



	double Object2D::rangeToLine(double x, double y, double x1, double y1, double x2, double y2) {
		double a, b, c;
		getLine(x1, y1, x2, y2, a, b, c);
		return abs(a * x + b * y + c) / sqrt(a * a + b * b);
	}


	bool Object2D::dotInPolyline(dot point, std::vector<dot> polyline) {
		dot addition{ 500.f,0};
		dot lineEnd = point + addition;
		int intercect = 0;
		for (int i = 0; i < polyline.size(); i++) {
			if (ifLinesCrosing2D(point, lineEnd, polyline[i],polyline[(i+1)%polyline.size()]))
				intercect++;
		}
		return intercect & 1;
	}

	bool Object2D::ifLinesCrosing2D(dot A, dot B, dot C, dot D) {
		return ccw(A, C, D) != ccw(B, C, D) and ccw(A, B, C) != ccw(A, B, D);
	}

	void Object2D::init_graphics() {
		dotsToDraw.clear();
		linesToDraw.clear();
		dotsToDraw.insert(dotsToDraw.begin(), recalculatedShape.begin(), recalculatedShape.end());
		linesToDraw.resize(dotsToDraw.size());
		for (int i = 0; i < dotsToDraw.size(); i++) {
			LineToDraw line = { i, (i + 1) % linesToDraw.size() ,true };
			linesToDraw[i] = line;
		}
	}


	Object2D Object2D::ellipse(double xToY, int dots) {
		Object2D obj;
		obj.conture.resize(dots);
			double angle = (2.f * M_PI) / (double)dots;
			for (int i = 0; i < dots; i++) {
				double a = angle * i;
				float x = cos(a);
				float y = sin(a)/xToY;
				dot vector = {x,y};
				obj.conture[i] = vector;
			}
			return obj;
	}

	void Object2D::getLine(double x1, double y1, double x2, double y2, double& a, double& b, double& c)
	{
		// (x- p1X) / (p2X - p1X) = (y - p1Y) / (p2Y - p1Y) 
		a = y1 - y2; // Note: this was incorrectly "y2 - y1" in the original answer
		b = x2 - x1;
		c = x1 * y2 - x2 * y1;
	}

	bool Object2D::ccw(dot A, dot B, dot C) {
		return (C.Y() - A.Y()) * (B.X() - A.X()) > (B.Y() - A.Y()) * (C.X() - A.X());
	}
