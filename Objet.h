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
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
class Objet{
private :
    std::vector<Vec3f> sommets;
    std::vector<Vec3f> norms;
    std::vector<Vec2f> uv;
    std::vector<std::vector<Vec3i> > triangles;
    unsigned char* pixels;
    int widthTexture;
    int heightTexture;
    int bytesperpixels;


public :
    Objet(std::string const& filename,std::string const& texture );
    ~Objet() ;
    Vec3f get_pixels(Vec2f const& uv) const;
    int nbTriangle() const;
    Vec3f sommet(int i) const;
    Vec3f norm(int i) const;
    Vec2f get_uv(int i ) const;
    std::vector<int> faceVertices(int idx) const;
    std::vector<int> faceNormales(int idx) const ;
    std::vector<int> faceUV(int idx) const ;
};

#include "Objet.hpp"