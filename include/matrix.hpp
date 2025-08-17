#ifndef MATRIX_H
#define MATRIX_H

#include <initializer_list>
#include <stdexcept>
#include <iostream>
#include <algorithm>

class Matrix {
private:
    float* matrix_elements;
    int rows;
    int cols;

public:
    // Constructor with just dimensions (zero-initialized)
    Matrix(int row, int col) : rows{row}, cols{col} {
        matrix_elements = new float[rows * cols]();
    }

    // Constructor with dimensions and initializer list
    Matrix(int row, int col, std::initializer_list<float> values)
        : rows{row}, cols{col} {
        if ((int)values.size() != row * col) {
            throw std::invalid_argument("Initializer list size does not match matrix dimensions");
        }
        matrix_elements = new float[rows * cols];
        int i = 0;
        for (auto v : values) {
            matrix_elements[i++] = v;
        }
    }

    // Copy constructor
    Matrix(const Matrix& other) : rows{other.rows}, cols{other.cols} {
        matrix_elements = new float[rows * cols];
        for (int i = 0; i < rows * cols; i++) {
            matrix_elements[i] = other.matrix_elements[i];
        }
    }

    // COPY assignment
    Matrix& operator=(const Matrix& other) {
        if (this == &other) return *this;
        this->cols = other.cols;
        this->rows = other.rows;
        this->matrix_elements = new float[rows * cols];
        std::copy(other.matrix_elements, other.matrix_elements + rows * cols, this->matrix_elements);
        return *this;
    }

    // MOVE ctor
    Matrix(Matrix&& other) noexcept : rows(other.rows), cols(other.cols),
                                      matrix_elements(other.matrix_elements) {
        other.rows = other.cols = 0;
        other.matrix_elements = nullptr;
    }

    // MOVE assignment
    Matrix& operator=(Matrix&& other) noexcept {
        if (this == &other) return *this;
        delete[] matrix_elements;
        rows = other.rows; cols = other.cols;
        matrix_elements = other.matrix_elements;
        other.rows = other.cols = 0;
        other.matrix_elements = nullptr;
        return *this;
    }

    // Destructor
    ~Matrix() {
        delete[] matrix_elements;
    }

    // Access operator
    float& operator()(int r, int c) {
        return matrix_elements[r * cols + c];
    }

    const float& operator()(int r, int c) const {
        return matrix_elements[r * cols + c];
    }

    // Matrix multiplication
    Matrix operator*(const Matrix& other) const {
        if (cols != other.rows) {
            throw std::invalid_argument("Matrix dimensions do not match for multiplication");
        }
        Matrix result(rows, other.cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < other.cols; j++) {
                float sum = 0.0f;
                for (int k = 0; k < cols; k++) {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

    // Matrix addition
    Matrix operator+(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrix dimensions do not match for addition");
        }
        Matrix result(rows, cols);
        for (int i = 0; i < rows * cols; i++) {
            result.matrix_elements[i] = this->matrix_elements[i] + other.matrix_elements[i];
        }
        return result;
    }

    // Scalar multiplication (matrix * scalar)
    Matrix operator*(float scalar) const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows * cols; i++) {
            result.matrix_elements[i] = this->matrix_elements[i] * scalar;
        }
        return result;
    }

    // Scalar multiplication (scalar * matrix) – friend function
    friend Matrix operator*(float scalar, const Matrix& mat) {
        return mat * scalar; // reuse the member version
    }

    // Transpose: returns a new matrix with rows/cols swapped
    Matrix transpose() const {
        Matrix result(cols, rows);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }

    // Conversion to float (only if 1x1)
    operator float() const {
        if (rows != 1 || cols != 1) {
            throw std::logic_error("Matrix is not 1x1, cannot convert to float");
        }
        return matrix_elements[0];
    }

    // Debug print
    void print() const {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                std::cout << (*this)(i, j) << " ";
            }
            std::cout << "\n";
        }
    }

    int get_cols() const {
        return cols;
    }

    int get_rows() const {
        return rows;
    }
};

#endif // MATRIX_H
