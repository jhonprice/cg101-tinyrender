#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

namespace air {
	struct Point {
		int x{};
		int y{};
	};
	void swap(Point& a, Point& b) {
		std::swap(a.x, b.x);
		std::swap(a.y, b.y);
	}
	void transpose(Point& a) {
		std::swap(a.x, a.y);
	}
}

//问题：如果取0.1 线段会不连续
void line(air::Point p0, air::Point p1, TGAImage& image, TGAColor color) {
	for (float t{ 0 }; t < 1.; t += 0.01) {
		int x = p0.x + (p1.x - p0.x) * t;
		int y = p0.y + (p1.y - p0.y) * t;
		image.set(x, y, color);
	}
}


//问题1：通过交换点来修复缺失的红线，因此 x0 始终低于 x1。 
//问题2：由于其高度大于宽度的事实，其中一条线段中有孔
void line2(air::Point p0, air::Point p1, TGAImage& image, TGAColor color) {
	for (int x{ p0.x }; x <= p1.x; x++) {
		float t = (x - p0.x) / (float)(p1.x - p0.x);
		int y = p0.y * (1. - t) + p1.y * t;
		image.set(x, y, color);
	}
}

//修复line2问题2：if (dx>dy) {for (int x)} else {for (int y)}
//问题：由于除法的舍入，一个像素会设置很多次
void line3(air::Point p0, air::Point p1, TGAImage& image, TGAColor color) {
	bool steep = false;
	if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) { // if the line is steep, we transpose the image 
		air::transpose(p0);
		air::transpose(p1);
		steep = true;
	}
	if (p0.x > p1.x) { // make it left−to−right 
		air::swap(p0, p1);
	}
	for (int x{ p0.x }; x <= p1.x; x++) {
		float t = (x - p0.x) / (float)(p1.x - p0.x);
		int y = p0.y * (1. - t) + p1.y * t;
		if (steep) {
			image.set(y, x, color); // if transposed, de−transpose 
		}
		else {
			image.set(x, y, color);
		}
	}
}

int main(int argc, char** argv) {
	TGAImage image(100, 100, TGAImage::RGB);
	line({ 0, 100 }, { 100, 90 }, image, green);


	//正确结果：蓝线被覆盖
	line3({ 13, 20 }, { 80, 40 }, image, blue);
	line3({ 20, 13 }, { 40, 80 }, image, white);
	line3({ 80, 40 }, { 13, 20 }, image, red);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

