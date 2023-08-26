#pragma once
#include <cmath>
#include <vector>
#include <iostream>
// Multiplicar matrices
std::vector<std::vector<float>> multiplyMatrices(const std::vector<std::vector<float>>& matriz1, const std::vector<std::vector<float>>& matriz2) {
    int filas_matriz1 = matriz1.size();
    int columnas_matriz1 = matriz1[0].size();
    int filas_matriz2 = matriz2.size();
    int columnas_matriz2 = matriz2[0].size();

    // Verificar si las matrices se pueden multiplicar
    if (columnas_matriz1 != filas_matriz2) {
        std::cout << "No se pueden multiplicar las matrices." << std::endl;
        return std::vector<std::vector<float>>();
    }

    // Crear una matriz de resultado con las dimensiones adecuadas
    std::vector<std::vector<float>> matriz_resultado(filas_matriz1, std::vector<float>(columnas_matriz2, 0.0));

    // Realizar la multiplicación de matrices
    for (int i = 0; i < filas_matriz1; ++i) {
        for (int j = 0; j < columnas_matriz2; ++j) {
            for (int k = 0; k < columnas_matriz1; ++k) {
                matriz_resultado[i][j] += matriz1[i][k] * matriz2[k][j];
            }
        }
    }

    return matriz_resultado;
}

// Multiplicar matriz por vector
std::vector<float> multiplyMatrixByVector(const std::vector<std::vector<float>>& matriz, const std::vector<float>& vector) {
    int filas_matriz = matriz.size();
    int columnas_matriz = matriz[0].size();
    int filas_vector = vector.size();

    // Verificar si las matrices se pueden multiplicar
    if (columnas_matriz != filas_vector) {
        std::cout << "No se puede multiplicar la matriz por el vector." << std::endl;
        return std::vector<float>();
    }

    // Crear un vector de resultado con las dimensiones adecuadas
    std::vector<float> vector_resultado(filas_matriz, 0.0);

    // Realizar la multiplicación matriz-vector
    for (int i = 0; i < filas_matriz; ++i) {
        for (int k = 0; k < columnas_matriz; ++k) {
            vector_resultado[i] += matriz[i][k] * vector[k];
        }
    }

    return vector_resultado;
}

//coordenadas baricentricas para un punto
std::vector<float> barycentricCoords(const std::vector<float>& A, const std::vector<float>& B, const std::vector<float>& C, const std::vector<float>& P) {
    float areaABC = abs((A[0] * B[1] + B[0] * C[1] + C[0] * A[1]) - (A[1] * B[0] + B[1] * C[0] + C[1] * A[0]));
    if (areaABC == 0) {
        return std::vector<float>{0, 0, 0};
    }

    float areaPCB = abs((P[0] * C[1] + C[0] * B[1] + B[0] * P[1]) - (P[1] * C[0] + C[1] * B[0] + B[1] * P[0]));
    float areaACP = abs((A[0] * C[1] + C[0] * P[1] + P[0] * A[1]) - (A[1] * C[0] + C[1] * P[0] + P[1] * A[0]));
    float areaABP = abs((A[0] * B[1] + B[0] * P[1] + P[0] * A[1]) - (A[1] * B[0] + B[1] * P[0] + P[1] * A[0]));

    float u = areaPCB / areaABC;
    float v = areaACP / areaABC;
    float w = 1.0 -u -v;

    if (0 <= u && u <= 1 && 0 <= v && v <= 1 && 0 <= w && w <= 1 && (std::abs(u + v + w - 1.0) == 0)) {
        return std::vector<float>{u, v, w};
    }
    else {
        return std::vector<float>{-1, -1, -1};
    }
}

std::vector<std::vector<float>> matriz_transpuesta(const std::vector<std::vector<float>>& matriz) {
    int filas = matriz.size();
    int columnas = matriz[0].size();
    std::vector<std::vector<float>> transpuesta(columnas, std::vector<float>(filas, 0.0));

    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            transpuesta[j][i] = matriz[i][j];
        }
    }

    return transpuesta;
}

float determinante(const std::vector<std::vector<float>>& matriz) {
    int n = matriz.size();
    if (n == 2) {
        return matriz[0][0] * matriz[1][1] - matriz[0][1] * matriz[1][0];
    }

    float det = 0.0;
    for (int i = 0; i < n; ++i) {
        std::vector<std::vector<float>> submatriz(n - 1, std::vector<float>(n - 1, 0.0));
        for (int row = 1; row < n; ++row) {
            for (int col = 0; col < n; ++col) {
                if (col != i) {
                    submatriz[row - 1][col < i ? col : col - 1] = matriz[row][col];
                }
            }
        }
        det += (i % 2 == 0 ? 1 : -1) * matriz[0][i] * determinante(submatriz);
    }

    return det;
}

std::vector<std::vector<float>> matriz_adjunta(const std::vector<std::vector<float>>& matriz) {
    int n = matriz.size();
    std::vector<std::vector<float>> adjunta(n, std::vector<float>(n, 0.0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::vector<std::vector<float>> submatriz(n - 1, std::vector<float>(n - 1, 0.0));
            for (int row = 0; row < n; ++row) {
                if (row != i) {
                    for (int col = 0; col < n; ++col) {
                        if (col != j) {
                            submatriz[row < i ? row : row - 1][col < j ? col : col - 1] = matriz[row][col];
                        }
                    }
                }
            }
            adjunta[i][j] = ((i + j) % 2 == 0 ? 1 : -1) * determinante(submatriz);
        }
    }

    return matriz_transpuesta(adjunta);
}

std::vector<std::vector<float>> matriz_inversa(const std::vector<std::vector<float>>& matriz) {
    float determ = determinante(matriz);
    if (determ == 0) {
        throw std::invalid_argument("No se puede calcular la inversa de la matriz con determinante nulo");
    }
    std::vector<std::vector<float>> m_adjunta = matriz_adjunta(matriz);

    int n = matriz.size();
    std::vector<std::vector<float>> inversa(n, std::vector<float>(n, 0.0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            inversa[i][j] = m_adjunta[i][j] / determ;
        }
    }

    return inversa;
}

std::vector<float> producto_cruz(const std::vector<float>& vector1, const std::vector<float>& vector2) {
    if (vector1.size() != 3 || vector2.size() != 3) {
        throw std::invalid_argument("Los vectores deben tener tres componentes.");
    }

    float a1 = vector1[0], a2 = vector1[1], a3 = vector1[2];
    float b1 = vector2[0], b2 = vector2[1], b3 = vector2[2];

    float producto_cruz_x = a2 * b3 - a3 * b2;
    float producto_cruz_y = a3 * b1 - a1 * b3;
    float producto_cruz_z = a1 * b2 - a2 * b1;

    return { producto_cruz_x, producto_cruz_y, producto_cruz_z };
}

std::vector<float> subtract_arrays(const std::vector<float>& array1, const std::vector<float>& array2) {
    if (array1.size() != array2.size()) {
        throw std::invalid_argument("Los arrays deben tener la misma longitud.");
    }

    std::vector<float> result(array1.size(), 0.0);
    for (size_t i = 0; i < array1.size(); ++i) {
        result[i] = array1[i] - array2[i];
    }

    return result;
}

float calcular_norma(const std::vector<float>& vector) {
    float suma_cuadrados = 0.0;
    for (float componente : vector) {
        suma_cuadrados += componente * componente;
    }
    return std::sqrt(suma_cuadrados);
}

std::vector<float> normalizar_vector(const std::vector<float>& vector) {
    float norma = calcular_norma(vector);
    if (norma == 0.0) {
        throw std::invalid_argument("No se puede normalizar un vector nulo.");
    }

    std::vector<float> vector_normalizado;
    vector_normalizado.resize(vector.size(), 0);
    for (size_t i = 0; i < vector.size(); ++i) {
        vector_normalizado[i] = vector[i] / norma;
    }

    return vector_normalizado;
}

float dot_product(const std::vector<float>& vector1, const std::vector<float>& vector2) {
    if (vector1.size() != vector2.size()) {
        throw std::invalid_argument("Los vectores deben tener la misma longitud.");
    }

    float result = 0.0;
    for (size_t i = 0; i < vector1.size(); ++i) {
        result += vector1[i] * vector2[i];
    }

    return result;
}