#include "Polygon.h" // Incluye el archivo de encabezado "Polygon.h"
using namespace std; // Usa el espacio de nombres estándar de C++

typedef vector<float> vec; // Crea un alias para el tipo vector<float> como 'vec'

// Función para obtener los vértices de las caras laterales del cubo
void get_lados(vec &l1, vec &l2, vec &l3, vec &l4, vec A, vec B)
{
    // Cara del frente
    l1.push_back(B[0]); l1.push_back(A[1]); l1.push_back(A[2]);
    l1.push_back(B[0]); l1.push_back(B[1]); l1.push_back(A[2]);
    l1.push_back(A[0]); l1.push_back(B[1]); l1.push_back(A[2]);
    l1.push_back(A[0]); l1.push_back(A[1]); l1.push_back(A[2]);

    // Cara de atrás
    l2.push_back(A[0]); l2.push_back(A[1]); l2.push_back(B[2]);
    l2.push_back(A[0]); l2.push_back(B[1]); l2.push_back(B[2]);
    l2.push_back(B[0]); l2.push_back(B[1]); l2.push_back(B[2]);
    l2.push_back(B[0]); l2.push_back(A[1]); l2.push_back(B[2]);

    // Caras lado derecho
    l3.push_back(B[0]); l3.push_back(A[1]); l3.push_back(B[2]);
    l3.push_back(B[0]); l3.push_back(B[1]); l3.push_back(B[2]);
    l3.push_back(B[0]); l3.push_back(B[1]); l3.push_back(A[2]);
    l3.push_back(B[0]); l3.push_back(A[1]); l3.push_back(A[2]);

    // Caras lado izquierdo
    l4.push_back(A[0]); l4.push_back(A[1]); l4.push_back(A[2]);
    l4.push_back(A[0]); l4.push_back(B[1]); l4.push_back(A[2]);
    l4.push_back(A[0]); l4.push_back(B[1]); l4.push_back(B[2]);
    l4.push_back(A[0]); l4.push_back(A[1]); l4.push_back(B[2]);
}

// Función para obtener los vértices de las tapas (superior e inferior) del cubo
void get_tapa(vec &l5, vec &l6, vec A, vec B)
{
    // Tapa
    l5.push_back(A[0]); l5.push_back(B[1]); l5.push_back(B[2]);
    l5.push_back(A[0]); l5.push_back(B[1]); l5.push_back(A[2]);
    l5.push_back(B[0]); l5.push_back(B[1]); l5.push_back(A[2]);
    l5.push_back(B[0]); l5.push_back(B[1]); l5.push_back(B[2]);

    // Base
    l6.push_back(A[0]); l6.push_back(A[1]); l6.push_back(A[2]);
    l6.push_back(A[0]); l6.push_back(A[1]); l6.push_back(B[2]);
    l6.push_back(B[0]); l6.push_back(A[1]); l6.push_back(B[2]);
    l6.push_back(B[0]); l6.push_back(A[1]); l6.push_back(A[2]);
}

// Definición de la estructura Cubo
class Cubo
{
public:
    vector<Polygon> caras; // Almacena las seis caras del cubo como objetos de tipo Polygon
    vector<float> eje_x; // Almacena información sobre el eje X del cubo
    vector<float> eje_y; // Almacena información sobre el eje Y del cubo
    vector<float> eje_z; // Almacena información sobre el eje Z del cubo

    // Constructor del Cubo
    Cubo(vec A, vec B)
    {
        vec cara[6]; // Arreglo para almacenar los vértices de las caras del cubo
        caras.reserve(6); // Reserva espacio para las seis caras
        eje_x.reserve(2); // Reserva espacio para el eje X
        eje_y.reserve(2); // Reserva espacio para el eje Y
        eje_z.reserve(2); // Reserva espacio para el eje Z
        eje_x.resize(2); // Establece el tamaño del vector del eje X en 2
        eje_y.resize(2); // Establece el tamaño del vector del eje Y en 2
        eje_z.resize(2); // Establece el tamaño del vector del eje Z en 2
        for (int w = 0; w < 6; ++w)
            cara[w].reserve(12); // Reserva espacio para los vértices de cada cara

        // Obtiene los vértices de las caras laterales y las tapas
        get_lados(cara[0], cara[1], cara[2], cara[3], A, B);
        get_tapa(cara[4], cara[5], A, B);

        // Crea las seis caras del cubo con el color gris predeterminado
        for (int i = 0; i < 6; ++i)
        {
            Polygon *ptr_poly = new Polygon(cara[i], 0.512, 0.512, 0.512);
            caras.push_back(*ptr_poly);
        }

        // Actualiza la información sobre los ejes
        actualizar_ejes();
    }

    // Función para dibujar el cubo
    void draw(GLFWwindow *window, bool wired = false)
    {
        for (int i = 0; i < 6; ++i)
        {
            caras[i].draw(window, wired);
        }
    }

    // Función para imprimir la posición de los vértices del cubo
    void print_cubo()
    {
        for (int i = 0; i < 6; ++i)
        {
            caras[i].print_position();
        }
    }

    // Función para imprimir los valores mínimos y máximos de los ejes
    void print_min_max_axis()
    {
        cout << eje_x[0] << " " << eje_x[1] << endl;
        cout << eje_y[0] << " " << eje_y[1] << endl;
        cout << eje_z[0] << " " << eje_z[1] << endl;
        cout << "//////////////////////////////////" << endl;
    }

    // Función para obtener los valores mínimos y máximos del eje X de una cara
    vector<float> get_x_min_max_cara()
    {
        return eje_x;
    }

    // Función para obtener los valores mínimos y máximos del eje Y de una cara
    vector<float> get_y_min_max_cara()
    {
        return eje_y;
    }

    // Función para obtener los valores mínimos y máximos del eje Z de una cara
    vector<float> get_z_min_max_cara()
    {
        return eje_z;
    }

    // Función para cambiar el color de una cara del cubo
    void change_color_cara(int position, float R, float G, float B)
    {
        caras[position].change_polygon_color(R, G, B);
    }

    // Función para cambiar la posición de la textura de una cara del cubo
    void change_textura_cara(int position, float min_x, float min_y, float max_x, float max_y)
    {
        caras[position].change_texture_pos(min_x, min_y, max_x, max_y);
    }

    // Función para actualizar la información sobre los ejes del cubo
    void actualizar_ejes()
    {
        for (int i = 0; i < 6; ++i)
        {
            caras[i].actualizar_ejes();
        }

        // Almacena temporalmente las coordenadas de los ejes
        vector<float> new_x;
        vector<float> new_y;
        vector<float> new_z;

        for (int i = 0; i < 6; ++i)
        {
            new_x.insert(new_x.end(), caras[i].eje_x.begin(), caras[i].eje_x.end());
            new_y.insert(new_y.end(), caras[i].eje_y.begin(), caras[i].eje_y.end());
            new_z.insert(new_z.end(), caras[i].eje_z.begin(), caras[i].eje_z.end());
        }

        // Ordena las coordenadas para obtener los valores mínimos y máximos
        sort(new_x.begin(), new_x.end());
        sort(new_y.begin(), new_y.end());
        sort(new_z.begin(), new_z.end());

        // Actualiza los valores de los ejes X, Y y Z
        eje_x[0] = new_x[0];
        eje_x[1] = new_x[11];
        eje_y[0] = new_y[0];
        eje_y[1] = new_y[11];
        eje_z[0] = new_z[0];
        eje_z[1] = new_z[11];
    }

    // Función para realizar una rotación en el eje X del cubo
    void rotate_x(float velocity)
    {
        for (int i = 0; i < 6; ++i)
        {
            caras[i].pro_rotation_x(velocity);
        }
    }

    // Función para trasladar el cubo
    void trasladar(float x, float y, float z)
    {
        for (int i = 0; i < 6; ++i)
        {
            caras[i].noob_translate(x, y, z);
        }
    }

    // Función para realizar una rotación en el eje Y del cubo
    void rotate_y(float velocity)
    {
        for (int i = 0; i < 6; ++i)
        {
            caras[i].pro_rotation_y(velocity);
        }
    }

    // Función para realizar una rotación en el eje Z del cubo
    void rotate_z(float velocity)
    {
        for (int i = 0; i < 6; ++i)
        {
            caras[i].pro_rotation_z(velocity);
        }
    }
};
