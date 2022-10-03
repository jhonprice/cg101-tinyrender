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
}


void line(air::Point p0, air::Point p1, TGAImage& image, TGAColor color) {
	//问题：如果取0.1 线段会不连续
	for (float t{ 0 }; t < 1.; t += 0.01) {
		int x = p0.x + (p1.x - p0.x) * t;
		int y = p0.y + (p1.y - p0.y) * t;
		image.set(x, y, color);
	}
}

void line2(air::Point p0, air::Point p1, TGAImage& image, TGAColor color) {
	for (int x{ p0.x }; x <= p1.x; x++) {
		float t = (x - p0.x) / (float)(p1.x - p0.x);
		int y = p0.y * (1. - t) + p1.y * t;
		image.set(x, y, color);
	}
}

int main(int argc, char** argv) {
	TGAImage image(100, 100, TGAImage::RGB);
	line({ 0, 100 }, { 100, 90 }, image, green);

	line2({ 13, 20 }, { 80, 40 }, image, white);
	line2({ 20, 13 }, { 40, 80 }, image, red);
	line2({ 80, 40 }, { 13, 20 }, image, blue);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

