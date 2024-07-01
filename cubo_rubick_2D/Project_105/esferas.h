


struct Esfera {
    int longitudeSegments = 10;
    int latitudeSegments = 10;
    int numVertices = (longitudeSegments + 1) * (latitudeSegments + 1);
    int numIndices = longitudeSegments * latitudeSegments * 6;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    float colorR, colorG, colorB;
    float centerX, centerY, centerZ;

    Esfera(float radius, float r, float g, float b, float x, float y, float z) : colorR(r), colorG(g), colorB(b), centerX(x), centerY(y), centerZ(z) {
        generateSphere(radius);
    }

    void generateSphere(float radius) {
        int vertexIndex = 0;
        int index = 0;

        vertices.resize(numVertices);
        indices.resize(numIndices);

        for (int i = 0; i <= latitudeSegments; ++i) {
            float theta = i * M_PI / latitudeSegments;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (int j = 0; j <= longitudeSegments; ++j) {
                float phi = j * 2 * M_PI / longitudeSegments;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                float x = cosPhi * sinTheta;
                float y = cosTheta;
                float z = sinPhi * sinTheta;

                vertices[vertexIndex++] = Vertex(radius * x + centerX, radius * y + centerY, radius * z + centerZ, colorR, colorG, colorB);
            }
        }

        for (int i = 0; i < latitudeSegments; ++i) {
            for (int j = 0; j < longitudeSegments; ++j) {
                int first = (i * (longitudeSegments + 1)) + j;
                int second = first + longitudeSegments + 1;

                indices[index++] = first;
                indices[index++] = second;
                indices[index++] = first + 1;

                indices[index++] = second;
                indices[index++] = second + 1;
                indices[index++] = first + 1;
            }
        }
    }
};
