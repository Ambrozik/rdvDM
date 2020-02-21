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

typedef struct CObjet{
private :
    std::vector<Vec3f> sommets;
    std::vector<Vec3f> norms;
    std::vector<Vec2f> uv_;
    std::vector<std::vector<Vec3i> > faces_;

public :
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
                Vec2f uv;
                for (int i=0;i<2;i++) iss >> uv[i];
                uv_.push_back(uv);
            }  else if (!line.compare(0, 2, "f ")) {
                std::vector<Vec3i> f;
                Vec3i tmp;
                iss >> trash;
                while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                    for (int i=0; i<3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                    f.push_back(tmp);
                }
                faces_.push_back(f);
            }
        }
    }
    void draw(){}
};