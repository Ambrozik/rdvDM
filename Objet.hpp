//
// Created by huga on 22/02/2020.

inline void Objet::chargerObjet( std::string const& filename){
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
inline int Objet::nbTriangle() const{
return triangles.size();
}
inline std::vector<int> Objet::faceVertices(int idx) const {
std::vector<int> face;
for (int i=0; i<(int)triangles[idx].size(); i++) face.push_back(triangles[idx][i][0]);
return face;
}
inline std::vector<int> Objet::faceNormales(int idx) const {
    std::vector<int> face;
    for (int i=0; i<(int)triangles[idx].size(); i++) face.push_back(triangles[idx][i][2]);
    return face;
}
inline Vec3f Objet::sommet(int i) const {
    return sommets[i];}
inline Vec3f Objet::norm(int i) const {
    return norms[i];
}
