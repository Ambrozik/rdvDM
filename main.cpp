#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include "geometry.h"
using namespace std;
const int width    = 1024;
const int height   = 768;

void line(Vec2i t0, Vec2i t1, Vec3f color, std::vector<Vec3f> &framebuffer) {
    bool steep = false;
    int x0 = t0.x, y0 = t0.y ,x1 = t1.x, y1 = t1.y;

    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            framebuffer[x+y*width] = color;

        } else {
            framebuffer[y+x*width] = color;

        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
    for (int x=x0; x<=x1; x++) {
        float t = (x-x0)/(float)(x1-x0);
        int y = y0*(1.-t) + y1*t;
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2,Vec3f color,   std::vector<Vec3f>  &framebuffer) {
    line(t0, t1, color, framebuffer);
    line(t1, t2, color, framebuffer);
    line(t2, t0, color, framebuffer);
}

    void render() {
    std::vector<Vec3f> framebuffer(width*height);
    //on creait le fond
    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {
        framebuffer[i,j*width] = Vec3f(0,0,0);
        }
    }
    triangle(Vec2i(0,0), Vec2i(0,140), Vec2i(140,140), Vec3f(0,0,255),framebuffer);

             std::ofstream ofs("./out.ppm", std::ios_base::out | std::ios_base::binary); // save the framebuffer to file
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height*width; ++i) {
        for (size_t j = 0; j<3; j++) {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}

int main() {
    render();

    return 0;
}
