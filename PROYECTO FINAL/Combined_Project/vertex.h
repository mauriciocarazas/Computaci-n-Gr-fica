struct Vertex {
    float x, y, z;
    float r, g, b;
    float nx, ny, nz; // Assuming these are the Normal components

    Vertex() : x(0), y(0), z(0), r(0), g(0), b(0), nx(0), ny(0), nz(0) {}

    Vertex(float x, float y, float z)
        : x(x), y(y), z(z), r(0), g(0), b(0), nx(0), ny(0), nz(0) {}

    Vertex(float x, float y, float z, float r, float g, float b)
        : x(x), y(y), z(z), r(r), g(g), b(b), nx(0), ny(0), nz(0) {}

    Vertex(float posX, float posY, float posZ, float normalX, float normalY, float normalZ, float colorR, float colorG, float colorB)
        : x(posX), y(posY), z(posZ), nx(normalX), ny(normalY), nz(normalZ), r(colorR), g(colorG), b(colorB) {}

    void printVertex() {
        std::cout << "x: " << x << " y: " << y << " z: " << z << " | r: " << r << " g: " << g << " b: " << b << std::endl;
    }
};
