





struct Anillo {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    float colorR, colorG, colorB;
    float centerX, centerY;
    float radius;

    Anillo(float x, float y, float radius, float red, float green, float blue) 
        : centerX(x), centerY(y), radius(radius), colorR(red), colorG(green), colorB(blue) {
        generateCircle(250);
    }

    void generateCircle(int segments) {
        float angle = 2.0f * M_PI / segments;
        vertices.resize(segments + 1);
        indices.resize(segments * 2);

        vertices[0] = Vertex(centerX, centerY, 0.0f, colorR, colorG, colorB);
        for (int i = 0; i < segments; ++i) {
            float x = radius * cos(i * angle) + centerX;
            float y = radius * sin(i * angle) + centerY;
            vertices[i + 1] = Vertex(x, y, 0.0f, colorR, colorG, colorB);
        }

        for (int i = 0; i < segments; ++i) {
            indices[i * 2] = i + 1;
            indices[i * 2 + 1] = (i + 1) % segments + 1;
        }
    }
};