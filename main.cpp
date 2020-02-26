#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include "Objet.h"

using namespace std;
class Renderer {
private :
    static constexpr int width    = 1024;
    static constexpr int height   = 768;
    std::vector<Vec3f> framebuffer = std::vector<Vec3f>(width*height);
    std::vector<float> zbuffer = std::vector<float>(width*height);
    void line(Vec2i t0, Vec2i t1, Vec3f color) {

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
    void triangle(Vec2i t0, Vec2i t1, Vec2i t2,Vec3f color) {
        line(t0, t1, color);
        line(t1, t2, color);
        line(t2, t0, color);
    }
    Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
        Vec3f s[2];
        for (int i=2; i--; ) {
            s[i][0] = C[i]-A[i];
            s[i][1] = B[i]-A[i];
            s[i][2] = A[i]-P[i];
        }
        Vec3f u = cross(s[0], s[1]);
        if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
            return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
        return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
    }
    Vec3f world2screen(Vec3f v) {
        return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
    }
    void drawTriangle(Objet const& o){
        Vec3f  lumiere = Vec3f(0,-1,-1);
        lumiere = lumiere.normalize();
        for(int i = 0 ;  i < o.nbTriangle(); i++) {

            std::vector<int> f = o.faceVertices(i);
            Vec3f v0 = o.sommet(f[0]);
            Vec3f v1 = o.sommet(f[1]);
            Vec3f v2 = o.sommet(f[2]);

            std::vector<Vec3f> vs;
            vs.emplace_back(world2screen(v0));
            vs.emplace_back(world2screen(v1));
            vs.emplace_back(world2screen(v2));

            std::vector<int> n = o.faceNormales(i);
            Vec3f n0 = o.norm(n[0]);
            Vec3f n1 = o.norm(n[1]);
            Vec3f n2 = o.norm(n[2]);
            std::vector<Vec3f> ns;
            ns.emplace_back(n0);
            ns.emplace_back(n1);
            ns.emplace_back(n2);

            std::vector<int> uv = o.faceUV(i);
            Vec2f uv0 = o.get_uv(uv[0]);
            Vec2f uv1 = o.get_uv(uv[1]);
            Vec2f uv2 = o.get_uv(uv[2]);
            std::vector<Vec2f> uvs;
            uvs.emplace_back(uv0);
            uvs.emplace_back(uv1);
            uvs.emplace_back(uv2);

            Vec2f bboxmin = Vec2f(numeric_limits<float>::max(),numeric_limits<float>::max());
            Vec2f bboxmax = Vec2f(-numeric_limits<float>::max(),-numeric_limits<float>::max());
            Vec2f clamp = Vec2f(width-1, height-1);

            for (int i=0; i<3; i++) {
                for (int j=0; j<2; j++) {
                    bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], vs[i][j]));
                    bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], vs[i][j]));
                }
            }
            Vec3f currentPixel;
            for (currentPixel.x=bboxmin.x; currentPixel.x<=bboxmax.x; currentPixel.x++) {
                for (currentPixel.y=bboxmin.y; currentPixel.y<=bboxmax.y; currentPixel.y++) {
                    Vec3f bc_screen  = barycentric(vs[0], vs[1], vs[2], currentPixel);
                    if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
                    currentPixel.z = 0;
                    Vec3f normal(0, 0, 0);
                    Vec2f texturePixel(0,0);
                    for (int i=0; i<3; i++) currentPixel.z += vs[i][2]*bc_screen[i];
                    for (int i=0; i<3; i++) normal = normal + ns[i]*bc_screen[i];
                    for (int i=0; i<3; i++) texturePixel = texturePixel + uvs[i]*bc_screen[i];
                    normal = normal.normalize();

                    float intensity = fabs(normal * lumiere);
                    if(intensity>0) {
                        if (zbuffer[int(currentPixel.x + currentPixel.y * width)] < currentPixel.z) {
                            zbuffer[int(currentPixel.x + currentPixel.y * width)] = currentPixel.z;
                            framebuffer[int(currentPixel.x + (height - 1 - currentPixel.y) * width)] = o.get_pixels(texturePixel) * intensity;
                        }
                    }
                }
            }

        }
    }
public :
    void render() {

        for (size_t j = 0; j<height; j++) {
            for (size_t i = 0; i<width; i++) {
                framebuffer[i+j*width] = Vec3f(0,0,0);
                zbuffer[i+j*width] = -std::numeric_limits<int>::max();
            }
        }
        Objet objet("./../african_head.obj","./../african_head_diffuse.tga");
        drawTriangle(objet);
        std::ofstream ofs("./out.ppm", std::ios_base::out | std::ios_base::binary); // save the framebuffer to file
        ofs << "P6\n" << width << " " << height << "\n255\n";
        for (size_t i = 0; i < height*width; ++i) {
            for (size_t j = 0; j<3; j++) {
                ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
            }
        }
        ofs.close();
    }
};

int main() {
    Renderer r;
    r.render();

    return 0;
}
