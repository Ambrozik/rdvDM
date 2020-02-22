#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include "geometry.h"
#include "cmake-build-debug/Objet.h"


void render() {
    std::vector<Vec3f> framebuffer(width*height);
    //on creait le fond
    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {
        framebuffer[i,j*width] = Vec3f(0,0,0);
        }
    }

    CObjet objet;
    objet.chargerObjet("./../diablo3.obj");
    objet.drawTriangle(framebuffer,false);
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
