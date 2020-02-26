//
// Created by huga on 21/02/2020.
//

#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include "geometry.h"

#ifndef PROJET_OBJET_H
#define PROJET_OBJET_H

#endif //PROJET_OBJET_H



class Objet{
private :
    int width    = 1024;
    int height   = 768;
    std::vector<Vec3f> sommets;
    std::vector<Vec3f> norms;
    std::vector<Vec2f> uv;
    std::vector<std::vector<Vec3i> > triangles;

public :
    void line(Vec2i t0, Vec2i t1, Vec3f color, std::vector<Vec3f> &framebuffer);
    void triangle(Vec2i t0, Vec2i t1, Vec2i t2,Vec3f color,   std::vector<Vec3f>  &framebuffer);
    void chargerObjet(std::string const& filename);
    int nbTriangle();
    Vec3f sommet(int i);
    Vec3f norm(int i);
    std::vector<int> face(int idx);
    void drawTriangle(std::vector<Vec3f> &framebuffer);
};