#include <cmath> // Para funciones trigonométricas

struct CuboEsferas {
    std::vector<Esfera> esferas;
    Vertex center;
    int numEsferas = 9;
    float angle; // Variable para el ángulo de rotación en grados

    CuboEsferas(Vertex c, float r, float g, float b, float angleDeg) : center(c), angle(angleDeg) {
        c.printVertex();
        std::cout << "r: " << r << " g: " << g << " b: " << b << std::endl;
        std::cout << "angle: " << angle << " degrees" << std::endl;
        generateCuboEsferas(r, g, b);
    }

    void generateCuboEsferas(float r, float g, float b) {
        float radius = 0.02f;
        std::cout << "Generate matrix" << std::endl;
        std::cout << "x: " << center.x << " y: " << center.y << " z: " << center.z << std::endl;

        // Convertir el ángulo a radianes
        float angleRad = angle * M_PI / 180.0f;

        // Función lambda para rotar puntos alrededor del centro
        auto rotate = [&](float x, float y) {
            float s = sin(angleRad);
            float c = cos(angleRad);
            // Trasladar el punto al origen, rotar y trasladar de vuelta
            float xnew = c * (x - center.x) - s * (y - center.y) + center.x;
            float ynew = s * (x - center.x) + c * (y - center.y) + center.y;
            return std::make_pair(xnew, ynew);
        };

        // Centro
        esferas.push_back(Esfera(radius, r, g, b, center.x, center.y, center.z));

        float spaceY = 0.12;
        auto [x, y] = rotate(center.x, center.y + spaceY);
        esferas.push_back(Esfera(radius, r, g, b, x, y, center.z));
        std::tie(x, y) = rotate(center.x, center.y - spaceY);
        esferas.push_back(Esfera(radius, r, g, b, x, y, center.z));

        float spaceX = 0.18;
        std::tie(x, y) = rotate(center.x + spaceX, center.y - 0.02);
        esferas.push_back(Esfera(radius, r, g, b, x, y, center.z));
        std::tie(x, y) = rotate(center.x - spaceX, center.y - 0.02);
        esferas.push_back(Esfera(radius, r, g, b, x, y, center.z));

        std::tie(x, y) = rotate(center.x - (spaceX / 2), center.y + (spaceY / 2));
        esferas.push_back(Esfera(radius, r, g, b, x, y, center.z));
        std::tie(x, y) = rotate(center.x + (spaceX / 2), center.y + (spaceY / 2));
        esferas.push_back(Esfera(radius, r, g, b, x, y, center.z));

        std::tie(x, y) = rotate(center.x - (spaceX / 2), center.y - (spaceY / 2));
        esferas.push_back(Esfera(radius, r, g, b, x, y, center.z));
        std::tie(x, y) = rotate(center.x + (spaceX / 2), center.y - (spaceY / 2));
        esferas.push_back(Esfera(radius, r, g, b, x, y, center.z));
    }
};

/*

struct CuboEsferas {
    std::vector<Esfera> esferas;
    Vertex center;
    int numEsferas = 9;

    CuboEsferas(Vertex c, float r, float g, float b) : center(c) {
      c.printVertex();
      std::cout<< "r: " << r << " g: " << g << " b: " << b << std::endl;
      generateCuboEsferas(r, g, b);
    }

    void generateCuboEsferas(float r, float g, float b) {
        // float radius = 0.025f;
        // float spacing = 0.09f;
        float radius = 0.02f;
        std::cout<< "Generate matrix"<<std::endl;
        std::cout<< "x: " << center.x << " y: " << center.y << " z: " << center.z << std::endl;

        //centro
        esferas.push_back(Esfera(radius, r, g, b, center.x, center.y, center.z));

        float spaceY = 0.12;
        esferas.push_back(Esfera(radius, r, g, b, center.x, center.y + spaceY, center.z));
        esferas.push_back(Esfera(radius, r, g, b, center.x, center.y - spaceY, center.z));

        float spaceX = 0.18;
        esferas.push_back(Esfera(radius, r, g, b, center.x + spaceX, center.y - 0.02, center.z));
        esferas.push_back(Esfera(radius, r, g, b, center.x - spaceX, center.y - 0.02, center.z));


        esferas.push_back(Esfera(radius, r, g, b, center.x - (spaceX/2), center.y + (spaceY/2), center.z));
        esferas.push_back(Esfera(radius, r, g, b, center.x + (spaceX/2), center.y + (spaceY/2), center.z));

        esferas.push_back(Esfera(radius, r, g, b, center.x - (spaceX/2), center.y - (spaceY/2), center.z));
        esferas.push_back(Esfera(radius, r, g, b, center.x + (spaceX/2), center.y - (spaceY/2), center.z));
    }
};


*/





        // esferas.push_back(Esfera(radius, 1.0, 0, 0, startX, startY, startZ));
        // esferas.push_back(Esfera(radius, 1.0, 0, 0, startX, startY, startZ));

        // for (int i = 0; i < 1; ++i) {
        //     for (int j = 0; j < 3; j++) {
        //         float x = startX + j * spacing;
        //         float y = startY + i * spacing;
        //         float z = startZ;
        //         std::cout<< "Generate matrix"<<std::endl;
        //         std::cout<< "x: " << x << " y: " << y << " z: " << z << std::endl;
        //         esferas.push_back(Esfera(radius, 1.0, 0, 0, x, y, z));
        //     }
        // }
        // for (int i = 1; i < 2; ++i) {
        //     for (int j = 0; j < 3; j++) {
        //         float x = startX + j * spacing;
        //         float y = startY + i * spacing;
        //         float z = startZ;
        //         std::cout<< "Generate matrix"<<std::endl;
        //         std::cout<< "x: " << x << " y: " << y << " z: " << z << std::endl;
        //         esferas.push_back(Esfera(radius, 0, 1.0, 0, x, y, z));
        //     }
        // }
        // for (int i = 2; i < 3; ++i) {
        //     for (int j = 0; j < 3; j++) {
        //         float x = startX + j * spacing;
        //         float y = startY + i * spacing;
        //         float z = startZ;
        //         std::cout<< "Generate matrix"<<std::endl;
        //         std::cout<< "x: " << x << " y: " << y << " z: " << z << std::endl;
        //         esferas.push_back(Esfera(radius, 0, 0, 1.0, x, y, z));
        //     }
        // }