#include <limits>
#include <cmath>
#include <functional>
#include <iostream>
#include "Objet.h"

using namespace std;
class Renderer {
private :
    static constexpr int width    = 1920;
    static constexpr int height   = 1080;
    std::vector<Vec3f> framebuffer = std::vector<Vec3f>(width*height);
    std::vector<float> zbuffer = std::vector<float>(width*height);


    typedef std::function<Vec3f(Vec3f const&, Vec3f const&)> Func;

    Func current = [&](Vec3f const&, Vec3f const& newP) {
        return newP ;
    };

    bool red = true;
    bool green = true;
    bool blue = true;

    Vec3f world2screen(Vec3f v) {
        return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
    }

    Matrix viewport(int x, int y, int w, int h) {
        Matrix m = Matrix::identity();
        m[0][3] = x+w/2.f;
        m[1][3] = y+h/2.f;
        m[2][3] = 255.f/2.f;

        m[0][0] = w/2.f;
        m[1][1] = h/2.f;
        m[2][2] = 255.f/2.f;
        return m;
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
    Vec3f m2v(mat<4, 1, float> m) {
        return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
    }
    mat<4, 1, float> v2m(Vec3f v) {
        mat<4, 1, float> m;
        m[0][0] = v.x;
        m[1][0] = v.y;
        m[2][0] = v.z;
        m[3][0] = 1.f;
        return m;
    }
    Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
        Vec3f z = (eye-center).normalize();
        Vec3f x = cross(up,z).normalize();
        Vec3f y = cross(z,x).normalize();
        Matrix Minv = Matrix::identity();
        Matrix Tr   = Matrix::identity();
        for (int i=0; i<3; i++) {
            Minv[0][i] = x[i];
            Minv[1][i] = y[i];
            Minv[2][i] = z[i];
            Tr[i][3] = -center[i];
        }
        return Minv*Tr;
    }
    void resetZbuffer(){
        for (size_t j = 0; j<height; j++) {
            for (size_t i = 0; i<width; i++) {
                zbuffer[i+j*width] = -std::numeric_limits<int>::max();
            }
        }
    }
    void drawCamera(Matrix  const& camera ,Objet const& o, Vec3f const& lumiere, Matrix const&Projection){
        for(int i = 0 ;  i < o.nbTriangle(); i++) {
            std::vector<int> f = o.faceVertices(i);
            Vec3f v0 = o.sommet(f[0]);
            Vec3f v1 = o.sommet(f[1]);
            Vec3f v2 = o.sommet(f[2]);

            std::vector<Vec3f> vs;

            vs.emplace_back(world2screen(m2v(Projection* camera * v2m(v0))));
            vs.emplace_back(world2screen(m2v(Projection * camera * v2m(v1))));
            vs.emplace_back(world2screen(m2v(Projection * camera * v2m(v2))));

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
                    //std::cout << currentPixel << "      " << bc_screen <<endl;

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
                            auto newP = o.get_pixels(texturePixel) * intensity;
                            if (!red) newP.x = 0;
                            if(!green) newP.y = 0 ;
                            if(!blue) newP.z = 0 ;
                            framebuffer[int(currentPixel.x + (height - 1 - currentPixel.y) * width)] = current(framebuffer[int(currentPixel.x + (height - 1 - currentPixel.y) * width)], newP);
                        }
                    }
                }
            }

        }
    }
    void drawTriangle(Objet const& o){
        float dist = 3.f;
        Matrix ViewPort   = viewport(0, 0, width, height);
        Vec3f  lumiere = Vec3f(0,-1,-1);
        lumiere = lumiere.normalize();
        Matrix Projection = Matrix::identity();
        Projection[3][2] = -1.f/dist ;
        //  Matrix camera1 = lookat(Vec3f(0,0,dist),Vec3f(0,0,0),Vec3f(0,1,0));
        Matrix cameraL = lookat(Vec3f(0,0,dist) + Vec3f(0.1,0,0),Vec3f(0,0,0),Vec3f(0,1,0));
        Matrix cameraR = lookat(Vec3f(0,0,dist) - Vec3f(0.1,0,0),Vec3f(0,0,0),Vec3f(0,1,0));
        blue = false; green = false; red = true;
        drawCamera(cameraL,o,lumiere,Projection);
        resetZbuffer();
        current = [&](Vec3f const& old, Vec3f const& newP) {
            Vec3f c  = old * 0.5 + newP * 0.5;
            return c ;
        };
        blue = true ; green = true; red = false;
        drawCamera(cameraR,o,lumiere,Projection);
    }
public :
    void render() {

        for (size_t j = 0; j<height; j++) {
            for (size_t i = 0; i<width; i++) {
                framebuffer[i+j*width] = Vec3f(0,0,0);
                zbuffer[i+j*width] = -std::numeric_limits<int>::max();
            }
        }
        Objet objet("./../diablo3.obj","./../diablo3_pose_diffuse.tga");
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
