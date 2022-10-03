#include "../include/ch01/tgaimage.h"
#include "../include/ch01/model.h"
#include "../include/ch01/geometry.h"
#include <memory>

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

/*
渲染所需的只是从文件中读取以下类型的顶点数组：
v 0.608654 -0.568839 -0.416318
是 x,y,z 坐标，每个文件行一个顶点 和面孔
f 1193/1240/1193 1180/1227/1180 1179/1226/1179
我们对每个空格后的第一个数字感兴趣。 它是我们之前读取的数组中顶点的数量。
因此，这条线表示 1193、1180 和 1179 个顶点形成一个三角形。 
请注意，在 obj 文件中，索引从 1 开始，这意味着您应该分别查找 1192、1179 和 1178 个顶点。
*/

Model* model = nullptr;
const int width = 800;
const int height = 800;

int main(int argc, char** argv) {

	TGAImage image(width, height, TGAImage::RGB);
	model = new Model("../obj/ch01/african_head.obj");

	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++) {
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.) * width / 2.;
			int y0 = (v0.y + 1.) * height / 2.;

			int x1 = (v1.x + 1.) * width / 2.;
			int y1 = (v1.y + 1.) * height / 2.;
			line3({ x0, y0 }, { x1, y1 }, image, white);
		}
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output1.tga");

	delete model;
	return 0;
}

