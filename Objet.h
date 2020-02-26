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
    void chargerObjet(std::string const& filename);
    int nbTriangle() const;
    Vec3f sommet(int i) const;
    Vec3f norm(int i) const;
    std::vector<int> faceVertices(int idx) const;
    std::vector<int> faceNormales(int idx) const ;
};

#include "Objet.hpp"