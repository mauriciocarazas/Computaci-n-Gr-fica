struct Vertex {
    float x, y, z;
    float r, g, b;
    Vertex() : x(0), y(0), z(0), r(0), g(0), b(0) {}
    Vertex(float x, float y, float z) : x(x), y(y), z(z), r(0), g(0), b(0) {}
    Vertex(float x, float y, float z, float r, float g, float b) : x(x), y(y), z(z), r(r), g(g), b(b) {}
    
    void printVertex() {
        std::cout << "x: " << x << " y: " << y << " z: " << z << std::endl;
    }
};