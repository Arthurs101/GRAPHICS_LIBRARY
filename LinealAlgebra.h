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
    float w = areaABP / areaABC;

    if (0 <= u && u <= 1 && 0 <= v && v <= 1 && 0 <= w && w <= 1 && std::abs(u + v + w - 1.0) < 1e-6) {
        return std::vector<float>{u, v, w};
    }
    else {
        return std::vector<float>{-1, -1, -1};
    }
}
