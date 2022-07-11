#pragma once

#include <vector>
#include <tuple>
#include <stdexcept>

namespace parsiss
{

template <unsigned int n, unsigned int m>
class Matrix
{
public:
    Matrix();
    Matrix(const Matrix& other);
    Matrix(const std::vector<double>& data);

    ~Matrix();

    double &operator()(unsigned int i, unsigned int j);
    double &operator()(unsigned int i);
    
    double operator()(unsigned int i, unsigned int j) const;
    double operator()(unsigned int i) const;


    template <unsigned int p> Matrix<n, p> operator*(const Matrix<m, p>& other) const;

    std::vector<double> operator*(const std::vector<double>& other) const;


    static std::tuple<Matrix, Matrix<m, m>, std::vector<int>> lu(const Matrix& A);

    static std::vector<double> solve(
        const Matrix& L,
        const Matrix<m, m>& U,
        const std::vector<int>& pivots,
        const std::vector<double>& b
    );

    static Matrix identity();

    void swap_rows(int i, int j);
    void swap_columns(int i, int j);

    std::vector<double> to_vector() const;


private:
    double* data;
};

template <unsigned int n, unsigned int m>
Matrix<n, m>::Matrix()
{
    data = new double[n * m];
    for (unsigned int i = 0; i < n * m; i++) {
        data[i] = 0;
    }
}


template <unsigned int n, unsigned int m>
Matrix<n, m>::Matrix(const Matrix& other)
{
    data = new double[n * m];
    for (unsigned int i = 0; i < n * m; i++) {
        data[i] = other.data[i];
    }
}


template <unsigned int n, unsigned int m>
Matrix<n, m>::Matrix(const std::vector<double>& data)
{
    this->data = new double[n * m];
    for (unsigned int i = 0; i < n * m; i++) {
        this->data[i] = data[i];
    }
}


template <unsigned int n, unsigned int m>
Matrix<n, m>::~Matrix()
{
    delete[] data;
}


template <unsigned int n, unsigned int m>
double& Matrix<n, m>::operator()(unsigned int i, unsigned int j)
{
    return data[i * m + j];
}


template <unsigned int n, unsigned int m>
double& Matrix<n, m>::operator()(unsigned int i)
{
    if constexpr(n == 1 || m == 1) {
        return data[i];
    } else {
        throw std::runtime_error("Matrix::operator()(unsigned int i) can only be used for matrices with one row or one column");
    }
}


template <unsigned int n, unsigned int m>
double Matrix<n, m>::operator()(unsigned int i, unsigned int j) const
{
    return data[i * m + j];
}


template <unsigned int n, unsigned int m>
double Matrix<n, m>::operator()(unsigned int i) const
{
    if constexpr(n == 1 || m == 1) {
        return data[i];
    } else {
        throw std::runtime_error("Matrix::operator()(unsigned int i) can only be used for matrices with one row or one column");
    }
}


template <unsigned int n, unsigned int m>
template <unsigned int p> Matrix<n, p> Matrix<n, m>::operator*(const Matrix<m, p>& other) const
{
    Matrix<n, p> result;
    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < p; j++) {
            for (unsigned int k = 0; k < m; k++) {
                result(i, j) += (*this)(i, k) * other(k, j);
            }
        }
    }
    return result;
}


template <unsigned int n, unsigned int m>
std::vector<double> Matrix<n, m>::operator*(const std::vector<double>& other) const
{
    std::vector<double> result(n);
    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < m; j++) {
            result[i] += (*this)(i, j) * other[j];
        }
    }
    return result;
}


template <unsigned int n, unsigned int m>
std::tuple<Matrix<n, m>, Matrix<m, m>, std::vector<int>> Matrix<n, m>::lu(const Matrix<n, m>& A)
{
    auto L = identity();
    Matrix<m, m> U = A;
    std::vector<int> pivots(n);
    for (unsigned int i = 0; i < n; i++) {
        pivots[i] = i;
    }
    for (unsigned int i = 0; i < n; i++) {
        double max = std::abs(U(i, i));
        unsigned int max_index = i;
        for (unsigned int j = i + 1; j < n; j++) {
            if (std::abs(U(j, i)) > max) {
                max = std::abs(U(j, i));
                max_index = j;
            }
        }
        if (max_index != i) {
            std::swap(pivots[i], pivots[max_index]);
            for (unsigned int j = 0; j < n; j++) {
                std::swap(U(i, j), U(max_index, j));
            }
        }
        for (unsigned int j = i + 1; j < n; j++) {
            L(j, i) = U(j, i) / U(i, i);
            for (unsigned int k = 0; k < n; k++) {
                U(j, k) -= L(j, i) * U(i, k);
            }
        }
    }

    return std::make_tuple(L, U, pivots);
}


template <unsigned int n, unsigned int m>
std::vector<double> Matrix<n, m>::solve(
    const Matrix& L,
    const Matrix<m, m>& U,
    const std::vector<int>& pivots,
    const std::vector<double>& b
) {
    std::vector<double> y(m);
    for (unsigned int i = 0; i < m; i++) {
        y[i] = b[pivots[i]];
    }
    for (unsigned int i = 0; i < m; i++) {
        for (unsigned int j = 0; j < i; j++) {
            y[i] -= L(i, j) * y[j];
        }
    }
    std::vector<double> x(m);
    for (unsigned int i = m - 1; i < m; i--) {
        for (unsigned int j = i + 1; j < m; j++) {
            y[i] -= U(i, j) * x[j];
        }
        x[i] = y[i] / U(i, i);
    }
    return x;
}


template <unsigned int n, unsigned int m>
Matrix<n, m> Matrix<n, m>::identity()
{
    Matrix<n, m> result;
    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < m; j++) {
            result(i, j) = (i == j) ? 1 : 0;
        }
    }
    return result;
}


template <unsigned int n, unsigned int m>
void Matrix<n, m>::swap_rows(int i, int j)
{
    for (unsigned int k = 0; k < m; k++) {
        double temp = (*this)(i, k);
        (*this)(i, k) = (*this)(j, k);
        (*this)(j, k) = temp;
    }
}


template <unsigned int n, unsigned int m>
void Matrix<n, m>::swap_columns(int i, int j)
{
    for (unsigned int k = 0; k < n; k++) {
        double temp = (*this)(k, i);
        (*this)(k, i) = (*this)(k, j);
        (*this)(k, j) = temp;
    }
}


template <unsigned int n, unsigned int m>
std::vector<double> Matrix<n, m>::to_vector() const
{
    if constexpr(n == 1 || m == 1) {
        std::vector<double> result(n * m);
        for (unsigned int i = 0; i < n * m; i++) {
            result[i] = data[i];
        }
        return result;
    } else {
        throw std::runtime_error("Matrix::to_vector() can only be called on an 1xn or nx1 matrix");
    }
}

}
