#include <cmath> // Para funciones trigonométricas

struct CuboEsferas {
    std::vector<Esfera> esferas;
    Vertex center;
    int numEsferas = 9;
    float angle; // Variable para el ángulo de rotación en grados

    CuboEsferas(Vertex c, float angleDeg) : center(c), angle(angleDeg) {
        c.printVertex();
        std::cout << "angle: " << angle << " degrees" << std::endl;
        generateCuboEsferas();
    }

    void generateCuboEsferas() {
        float radius = 0.02f;
        std::cout << "Generate matrix" << std::endl;
        center.printVertex();

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
        esferas.push_back(Esfera(radius, center.r, center.g, center.b, center.x, center.y, center.z));

        float spaceY = 0.12;
        auto [x, y] = rotate(center.x, center.y + spaceY);
        esferas.push_back(Esfera(radius, center.r, center.g, center.b, x, y, center.z));
        std::tie(x, y) = rotate(center.x, center.y - spaceY);
        esferas.push_back(Esfera(radius, center.r, center.g, center.b, x, y, center.z));

        float spaceX = 0.18;
        std::tie(x, y) = rotate(center.x + spaceX, center.y - 0.02);
        esferas.push_back(Esfera(radius, center.r, center.g, center.b, x, y, center.z));
        std::tie(x, y) = rotate(center.x - spaceX, center.y - 0.02);
        esferas.push_back(Esfera(radius, center.r, center.g, center.b, x, y, center.z));

        std::tie(x, y) = rotate(center.x - (spaceX / 2), center.y + (spaceY / 2));
        esferas.push_back(Esfera(radius, center.r, center.g, center.b, x, y, center.z));
        std::tie(x, y) = rotate(center.x + (spaceX / 2), center.y + (spaceY / 2));
        esferas.push_back(Esfera(radius, center.r, center.g, center.b, x, y, center.z));

        std::tie(x, y) = rotate(center.x - (spaceX / 2), center.y - (spaceY / 2));
        esferas.push_back(Esfera(radius, center.r, center.g, center.b, x, y, center.z));
        std::tie(x, y) = rotate(center.x + (spaceX / 2), center.y - (spaceY / 2));
        esferas.push_back(Esfera(radius, center.r, center.g, center.b, x, y, center.z));
    }
};
