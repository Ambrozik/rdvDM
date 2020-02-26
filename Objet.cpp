//
// Created by huga on 22/02/2020.
//

#include "geometry.h"
#include "Objet.h"

void Objet::line(Vec2i t0, Vec2i t1, Vec3f color, std::vector<Vec3f> &framebuffer) {

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
            if((height-1-y)+x*width <= framebuffer.size()) framebuffer[(height-1-y)+x*width] = color;
        } else {
            if(x+(height-1-y)*width <= framebuffer.size()) framebuffer[x+(height-1-y)*width] = color;
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
void Objet::triangle(Vec2i t0, Vec2i t1, Vec2i t2,Vec3f color,   std::vector<Vec3f>  &framebuffer) {
    line(t0, t1, color, framebuffer);
    line(t1, t2, color, framebuffer);
    line(t2, t0, color, framebuffer);
}
void Objet::chargerObjet( std::string const& filename){
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
int Objet::nbTriangle(){
return triangles.size();
}
std::vector<int> Objet::face(int idx) {
std::vector<int> face;
for (int i=0; i<(int)triangles[idx].size(); i++) face.push_back(triangles[idx][i][0]);
return face;
}
Vec3f Objet::sommet(int i){
    return sommets[i];}
Vec3f Objet::norm(int i) {
    return norms[i];
}
void Objet::drawTriangle(std::vector<Vec3f> &framebuffer){
    Vec3f  lumiere = Vec3f(0,0,1);
    for(int i = 0 ;  i < nbTriangle(); i++) {
        std::vector<int> f = face(i);
        Vec3f v0 = sommet(f[0]);
        Vec3f v1 = sommet(f[1]);
        Vec3f v2 = sommet(f[2]);
        Vec3f n = (v2-v0)^(v1-v0);
        n.normalize();
        float intensity = fabs(n * lumiere);
        if (intensity > 0) {
            Vec3f colorie = Vec3f(intensity, intensity, intensity);

            Vec2i A = Vec2i((v0.x + 1.) * width / 2., (v0.y + 1.) * height / 2.);
            Vec2i B = Vec2i((v1.x + 1.) * width / 2., (v1.y + 1.) * height / 2.);
            Vec2i C = Vec2i((v2.x + 1.) * width / 2., (v2.y + 1.) * height / 2.);
            //tri des 3 points dans l'ordre croissant des lignes
            if (A.y > B.y) std::swap(A, B);
            if (B.y > C.y) std::swap(B, C);
            if (A.y > B.y) std::swap(A, B);
            //calcule 3vecteurs qu'on parcours
            Vec2i AB = Vec2i(B.x - A.x, B.y - A.y);
            Vec2i AC = Vec2i(C.x - A.x, C.y - A.y);
            Vec2i BC = Vec2i(C.x - B.x, C.y - B.y);
            //parcours des lignes de v0 a v1
            Vec2i pAB = Vec2i(0, 0);
            Vec2i pAC = Vec2i(0, 0);

            if (AB.y > 0) {
                for (int i = 0; i < AB.y; i++) {
                    pAB.x = A.x + AB.x * i / AB.y;
                    pAB.y = A.y + i;
                    pAC.x = A.x + AC.x * i / AC.y;
                    pAC.y = pAB.y;
                    line(pAB, pAC, colorie, framebuffer);
                }
            } else {
                line(A, B, colorie, framebuffer);
            }
            Vec2i pBC = Vec2i(0, 0);
            if (BC.y > 0) {
                for (int i = AB.y; i < AC.y; i++) {
                    pBC.x = B.x + BC.x * (i - AB.y) / BC.y;
                    pBC.y = A.y + i;
                    pAC.x = A.x + AC.x * i / AC.y;
                    pAC.y = pBC.y;
                    line(pBC, pAC, colorie, framebuffer);
                }
            } else {
                line(B, C, colorie, framebuffer);
            }
        }
    }
}
