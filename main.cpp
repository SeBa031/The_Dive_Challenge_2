#include <iostream>   // Para leer y escribir en la consola (cin, cout)
#include <vector>     // Para usar arreglos dinámicos (matrices 2D que pueden cambiar de tamaño)
#include <queue>      // Para usar la estructura de datos Cola (FIFO) necesaria en BFS
#include <utility>    // Para usar std::pair (pares de coordenadas fila/columna)
#include <random>     // Para generar números aleatorios de alta calidad
#include <algorithm>  // Para usar la función shuffle() que desordena arreglos
#include <chrono>     // Para medir el tiempo exacto de ejecución

using namespace std;  // Evita tener que escribir "std::" antes de cout, vector, etc.

// Arreglos globales de dirección para CAVAR el laberinto (saltos de 2 en 2)
int dx[] = {0, 2, 0, -2}; // Eje X (Columnas): Centro, Derecha, Centro, Izquierda
int dy[] = {-2, 0, 2, 0}; // Eje Y (Filas): Arriba, Centro, Abajo, Centro

// Función auxiliar para imprimir la matriz en pantalla
// Usamos "const vector<...>&" para leer la matriz original sin copiarla, ahorrando memoria y tiempo
void imprimir_laberinto(const vector<vector<char>>& laberinto, string titulo) {
    cout << "\n" << titulo << "\n"; // Imprime el título que le pasemos
    for (int i = 0; i < laberinto.size(); i++) {        // Recorre las filas
        for (int j = 0; j < laberinto[i].size(); j++) { // Recorre las columnas de esa fila
            cout << laberinto[i][j] << " ";
        }
        cout << "\n"; // Salto de línea al terminar una fila
    }

}

// Función que usa BFS para encontrar la ruta más corta
void resolver_laberinto(vector<vector<char>>& laberinto) {
    // Inicia el cronómetro de alta resolución
    auto inicio = chrono::high_resolution_clock::now();

    int filas = laberinto.size();
    int columnas = laberinto[0].size();

    // Matriz de 'padres'. Guarda la coordenada desde la cual llegamos a cada celda.
    // Se inicializa entera con pares de (-1, -1) para indicar que nadie ha sido visitado aún.
    vector<vector<pair<int, int>>> padre(filas, vector<pair<int, int>>(columnas, {-1, -1}));

    // Marcamos la entrada (0,1) para saber que empezamos ahí
    padre[0][1] = {0, 1};

    // Creamos la cola para el BFS (FIFO - el primero que entra es el primero que sale)
    queue<pair<int, int>> cola;
    cola.push({0, 1}); // Metemos la posición inicial a la cola

    bool encontrado = false; // Bandera para saber si logramos tocar la 'S'

    // Coordenada exacta donde está la salida
    pair<int, int> pos_salida = {filas - 1, columnas - 2};

    // Bucle principal de BFS: Se ejecuta mientras haya caminos por explorar
    while (!cola.empty()) {
        pair<int, int> actual = cola.front(); // Leemos el primer elemento de la fila
        cola.pop();                           // Lo sacamos de la fila

        int f = actual.first;   // Fila actual
        int c = actual.second;  // Columna actual

        // Condición de victoria: ¿Pisamos la celda de salida?
        if (laberinto[f][c] == 'S') {
            encontrado = true; // Levantamos la bandera de victoria
            break;             // Detenemos el bucle prematuramente (¡Ya ganamos!)
        }

        // Arreglos de dirección de paso 1 en 1 para moverse por pasillos existentes
        int df[] = {-1, 1, 0, 0}; // Arriba, Abajo, Nada, Nada
        int dc[] = {0, 0, -1, 1}; // Nada, Nada, Izquierda, Derecha

        for (int i = 0; i < 4; i++) {
            int nf = f + df[i]; // Nueva Fila
            int nc = c + dc[i]; // Nueva Columna

            // Verificamos que no nos salimos del mapa general
            if (nf >= 0 && nf < filas && nc >= 0 && nc < columnas) {
                // Verificamos que la celda sea caminable ('.' o 'S') Y que NO haya sido visitada
                // Sabemos que no fue visitada si su 'padre' sigue siendo (-1, -1)
                if ((laberinto[nf][nc] == '.' || laberinto[nf][nc] == 'S') && padre[nf][nc].first == -1) {

                    padre[nf][nc] = {f, c}; // Anotamos: "A la celda nueva (nf, nc) se llega desde la actual (f, c)"
                    cola.push({nf, nc});    // Metemos la celda nueva en la cola de exploración
                }
            }
        }
    }

    // Si tocamos la meta en algún momento...
    if (encontrado) {
        // --- BACKTRACKING DEL CAMINO ---
        // Tomamos el padre inmediato de la celda de salida (para no sobreescribir la letra 'S')
        pair<int, int> paso = padre[pos_salida.first][pos_salida.second];

        // Mientras no retrocedamos hasta la entrada 'E' (0, 1)...
        while (!(paso.first == 0 && paso.second == 1)) {
            laberinto[paso.first][paso.second] = 'o'; // Dibujamos el rastro
            paso = padre[paso.first][paso.second];    // Retrocedemos pidiéndole el padre a la celda actual
        }

        // Detenemos el cronómetro justo antes de imprimir (imprimir distorsiona el tiempo real de cálculo)
        auto fin = chrono::high_resolution_clock::now();
        // Calculamos la diferencia en microsegundos
        auto duracion = chrono::duration_cast<chrono::microseconds>(fin - inicio);

        // Imprimimos el resultado final usando nuestra función auxiliar
        imprimir_laberinto(laberinto, "----- Laberinto Resuelto -----");
        // Mostramos la métrica de rendimiento
        cout << "\nTiempo de resolucion: " << duracion.count() << " microsegundos.\n";

    } else {
        // En un laberinto perfecto siempre habrá salida, pero esto es una buena práctica de seguridad
        cout << "\nNo se encontro un camino hacia la salida";
    }
}



// Función recursiva que excava los pasillos usando DFS
void cavar_caminos(int f, int c, vector<vector<char>> &laberinto, mt19937& gen) {
    laberinto[f][c] = '.'; // Transforma la pared actual en pasillo vacío

    // Lista de las 4 direcciones (indices 0, 1, 2, 3 correspondientes a dx y dy)
    vector<int> direcciones = {0, 1, 2, 3};
    // Mezcla las direcciones al azar usando el generador de números aleatorios
    shuffle(direcciones.begin(), direcciones.end(), gen);

    // Intenta avanzar en las 4 direcciones, una por una
    for (int i = 0; i < 4; i++) {
        int dir = direcciones[i];          // Toma una dirección al azar de la lista mezclada
        int nueva_fila = f + dy[dir];      // Calcula la fila destino (a 2 saltos de distancia)
        int nueva_columna = c + dx[dir];   // Calcula la columna destino (a 2 saltos de distancia)

        // Validación de límites: Verifica que el salto no se salga de los bordes del mapa
        if (nueva_fila > 0 && nueva_fila < laberinto.size() - 1 && nueva_columna > 0 && nueva_columna < laberinto[0].size() - 1) {
            // Si la celda a la que saltamos TODAVÍA es una pared virgen (no hemos pasado por ahí)
            if (laberinto[nueva_fila][nueva_columna] == '#') {

                // Magia: Rompe la pared intermedia que quedó entre el paso actual y el destino
                laberinto[f + dy[dir] / 2][c + dx[dir] / 2] = '.';

                // Llamada Recursiva: El "topo" se mueve a la nueva casilla y vuelve a empezar
                cavar_caminos(nueva_fila, nueva_columna, laberinto, gen);
            }
        }
    }
    // Cuando el bucle for termina, si no hay a dónde ir, la función termina y
    // "retrocede" a la casilla anterior (Backtracking invisible manejado por C++).
}

// Función encargada de preparar el tablero antes de cavar
vector<vector<char>> generar_laberinto(int filas, int columnas) {
    // El algoritmo requiere dimensiones impares para dejar paredes exteriores correctas
    if (filas % 2 == 0) filas++;       // Si es par, le suma 1 para hacerlo impar
    if (columnas % 2 == 0) columnas++; // Si es par, le suma 1 para hacerlo impar

    // Crea la matriz bidimensional llena de caracteres '#' (Paredes)
    vector<vector<char>> laberinto(filas, vector<char>(columnas, '#'));

    // Inicia la semilla y el generador aleatorio 'Mersenne Twister'
    random_device rd;
    mt19937 gen(rd());

    // Empieza a cavar siempre desde la coordenada interior (1, 1)
    cavar_caminos(1, 1, laberinto, gen);

    // Abre las puertas colocando la 'E' de Entrada y 'S' de Salida
    laberinto[0][1] = 'E';                   // Borde superior
    laberinto[filas - 1][columnas - 2] = 'S'; // Borde inferior (ajustado a la última columna impar)

    // Llama a la función auxiliar para imprimir el laberinto recién creado
    imprimir_laberinto(laberinto, "----- Laberinto Sin Resolver -----");

    return laberinto; // Devuelve la matriz terminada
}

int main() {
    int filas = 20, columnas = 20;

    // Bucle infinito para validar los datos que ingresa el usuario
    while (true) {
        cout << "Ingrese la cantidad de filas: ";
        cin >> filas;
        cout << "Ingrese la cantidad de columnas: ";
        cin >> columnas;

        // Condiciones de tamaño
        if (filas < 10 || columnas < 10) {
            cout << "Error. El tamanho de las filas y columnas debe ser mayor o igual a 10\n";
        } else if (filas > 50 || columnas > 50) {
            cout << "Error. El tamanho de las filas o columnas debe ser menor o igual a 50\n";
        } else {
            break; // Si los datos son correctos, rompe el bucle infinito y continúa el programa
        }
    }

    // 1. Genera el laberinto, lo imprime vacío y guarda la matriz
    auto laberinto = generar_laberinto(filas, columnas);

    // 2. Le pasa la matriz generada al BFS para que la resuelva, pinte y calcule el tiempo
    resolver_laberinto(laberinto);

    return 0; // Termina el programa exitosamente
}