//
// Created by huga on 21/02/2020.
//

#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include "../geometry.h"

#ifndef PROJET_OBJET_H
#define PROJET_OBJET_H

#endif //PROJET_OBJET_H


const int width    = 1024;
const int height   = 1000;

typedef struct CObjet{
private :
    std::vector<Vec3f> sommets;
    std::vector<Vec3f> norms;
    std::vector<Vec2f> uv;
    std::vector<std::vector<Vec3i> > triangles;

public :

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
                if(y+x*width <= framebuffer.size()) framebuffer[y+x*width] = color;


            } else {
                if(x+y*width <= framebuffer.size()) framebuffer[x+y*width] = color;
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
    void triangleColorie(){}
    void chargerObjet(char *filename){
        std::ifstream in;
        in.open (filename, std::ifstream::in);
        if (in.fail()) return;
        std::string line;
        while (!in.eof()) {
            std::getline(in, line);
            std::istringstream iss(line.c_str());
            char trash;
            if (!line.compare(0, 2, "v ")) {
                iss >> trash;
                Vec3f v;
                for (int i=0;i<3;i++) iss >> v[i];
                sommets.push_back(v);
            } else if (!line.compare(0, 3, "vn ")) {
                iss >> trash >> trash;
                Vec3f n;
                for (int i=0;i<3;i++) iss >> n[i];
                norms.push_back(n);
            } else if (!line.compare(0, 3, "vt ")) {
                iss >> trash >> trash;
                Vec2f u;
                for (int i=0;i<2;i++) iss >> u[i];
                uv.push_back(u);
            }  else if (!line.compare(0, 2, "f ")) {
                std::vector<Vec3i> f;
                Vec3i tmp;
                iss >> trash;
                while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                    for (int i=0; i<3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                    f.push_back(tmp);
                }
                triangles.push_back(f);
            }
        }
    }
    int nbTriangle(){
        return triangles.size();
}
    //recupurer une face
    std::vector<int> face(int idx) {
        std::vector<int> face;
        for (int i=0; i<(int)triangles[idx].size(); i++) face.push_back(triangles[idx][i][0]);
        return face;
    }
    //recuperer un sommet
    Vec3f sommet(int i){
        return sommets[i];}
    //dessiner l'objet en ligne
    void drawline(std::vector<Vec3f> &framebuffer){
        for(int i = 0 ;  i < nbTriangle(); i++){
            std::vector<int> f = face(i);

            for (int j=0; j<3; j++) {
                Vec3f v0 = sommet(f[0]);
                Vec3f v1 = sommet(f[1]);
                Vec3f v2= sommet(f[2]);
                Vec2i t0 = Vec2i((v0.x+1.)*width/2.,(v0.y+1.)*height/2.);
                Vec2i t1 = Vec2i((v1.x+1.)*width/2.,(v1.y+1.)*height/2.);
                Vec2i t2 = Vec2i((v2.x+1.)*width/2.,(v2.y+1.)*height/2.);
                triangle(t0, t1,t2, Vec3f(255,255,255), framebuffer);
            }
        }
    }
};