// ch02_Rasterization.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../include/ch02/tgaimage.h"
#include "../include/ch02/geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

const int width = 200;
const int height = 200;


void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color) {
	bool steep = false;
	if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) { // if the line is steep, we transpose the image 
		std::swap(p0.x, p0.y);
		std::swap(p1.x, p1.y);
		steep = true;
	}
	if (p0.x > p1.x) { // make it left−to−right 
		std::swap(p0, p1);
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


using Triangle = Vec2i[3];
const float zero = 1e-9;

void triangle_line(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
}

//求重心坐标（TODO
Vec3f barycentric(const Triangle pts, Vec2i P) {
	Vec3f u = Vec3f(pts[2].x - pts[0].x, 
		pts[1].x - pts[0].x, 
		pts[0].x - P.x) ^ 
		Vec3f(pts[2].y - pts[0].y, 
			pts[1].y - pts[0].y, 
			pts[0].y - P.y);
	/* `pts` and `P` has integer value as coordinates
	   so `abs(u[2])` < 1 means `u[2]` is 0, that means
	   triangle is degenerate, in this case return something with negative coordinates */
	if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void triangle(const Triangle pts, TGAImage& image, TGAColor color) {
	//构建包围盒
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2i bboxmax(0, 0);
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; i++) {
		bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
		bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

		bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
		bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
	}
	//光栅化
	Vec2i P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts, P);
			if (bc_screen.x <= zero || bc_screen.y <= zero || bc_screen.z <= zero) continue;
			image.set(P.x, P.y, color);
		}
	}
}

int main()
{
    TGAImage image(width, height, TGAImage::RGB);

	Triangle t0 = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Triangle t1 = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Triangle t2 = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };

	triangle_line(t0[0], t0[1], t0[2], image, white);
	triangle_line(t1[0], t1[1], t1[2], image, white);
	triangle_line(t2[0], t2[1], t2[2], image, white);

	triangle(t0, image, red);
	triangle(t1, image, green);
	triangle(t2, image, blue);


    image.flip_vertically();
    image.write_tga_file("output1.tga");


    return 0;
}

