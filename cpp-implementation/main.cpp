#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

void printMatrix(vector<vector<int>> matrix) {
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix[i].size(); j++) {
            cout << matrix[i][j] << " ";
        }
        cout << "\n";
    }
}


class Image {
private:
    vector<vector<int>> grayImageMatrix;

public:
    Image() {}

    Image(const vector<vector<int>> &_grayImageMatrix) {
        grayImageMatrix = _grayImageMatrix;
    }

    vector<vector<int>> getMatrix() {
        return grayImageMatrix;
    }
};

class MatrixOperation {
public:
    // kernel dimensions should be odd number
    vector<vector<int>> convolve(const vector<vector<int>> &kernel, const vector<vector<int>> &matrix) {
        // TODO: handle bad input
        if (kernel.empty() || kernel[0].empty() || matrix.empty() || matrix[0].empty()) {
            return vector<vector<int>>();
        }

        int rowsKernel = kernel.size();
        int colsKernel = kernel[0].size();
        int rowsMatrix = matrix.size();
        int colsMatrix = matrix[0].size();

        vector<vector<int>> convolution(matrix);
        for (int i = 0; i < rowsMatrix; i++) {
            for (int j = 0; j < colsMatrix; j++) {
                int sum = 0;
                for (int m = -rowsKernel / 2; m <= rowsKernel / 2; m++) {
                    for (int n = -colsKernel / 2; n <= colsKernel / 2; n++) {
                        if (i + m < 0 || rowsMatrix <= i + m || j + n < 0 || colsMatrix <= j + n) {
                            continue;
                        }
                        sum += (kernel[m + rowsKernel / 2][n + colsKernel / 2] * matrix[i + m][j + n]);
                    }
                }
                convolution[i][j] = sum;
            }
        }
        return convolution;
    }

    vector<vector<int>> sum(const vector<vector<int>> &a, const vector<vector<int>> &b) {
        // TODO: handle bad input
        vector<vector<int>> s(a);
        for (int i = 0; i < a.size(); i++) {
            for (int j = 0; j < a[i].size(); j++) {
                s[i][j] = a[i][j] + b[i][j];
            }
        }
        return s;
    }

    vector<vector<int>> rotateVertically(const vector<vector<int>> &matrix) {
        vector<vector<int>> rotatedMatrix(matrix);
        int rows = matrix.size();
        for (int i = 0; i < matrix.size(); i++) {
            for (int j = 0; j < matrix[i].size(); j++) {
                rotatedMatrix[rows - i - 1][j] = matrix[i][j];
            }
        }
        return rotatedMatrix;
    }

    vector<vector<int>> square(const vector<vector<int>> &matrix) {
        vector<vector<int>> matrixSquare(matrix);
        for (int i = 0; i < matrix.size(); i++) {
            for (int j = 0; j < matrix[i].size(); j++) {
                matrixSquare[i][j] = (matrix[i][j] * matrix[i][j]);
//                matrixSquare[i][j] = abs(matrix[i][j]);
            }
        }
        return matrixSquare;
    }

    vector<vector<int>> sqrt(const vector<vector<int>> &matrix) {
        vector<vector<int>> matrixSqrt(matrix);
        for (int i = 0; i < matrix.size(); i++) {
            for (int j = 0; j < matrix[i].size(); j++) {
                matrixSqrt[i][j] = (int) std::sqrt(matrix[i][j]);
            }
        }
        return matrixSqrt;
    }
};

class Filter {
public:
    virtual vector<vector<int>> apply(const vector<vector<int>> &matrix) = 0;
};

class SobelFilter : public Filter {
private:
    MatrixOperation matrixOperation;
    vector<vector<int>> kx{
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
    };
    vector<vector<int>> ky{
            {-1, -2, -1},
            {0,  0,  0},
            {1,  2,  1}
    };
public:
    vector<vector<int>> apply(const vector<vector<int>> &matrix) override {
        vector<vector<int>> gx = matrixOperation.convolve(kx, matrix);
        vector<vector<int>> gy = matrixOperation.convolve(ky, matrix);
        vector<vector<int>> gxSquare = matrixOperation.square(gx);
        vector<vector<int>> gySquare = matrixOperation.square(gy);
        vector<vector<int>> squareSum = matrixOperation.sum(gxSquare, gySquare);
        return matrixOperation.sqrt(squareSum);
    }
};


class HoughFilter : public Filter {
private:
    vector<vector<int>> kernel = vector<vector<int>>{{}};

public:
    vector<vector<int>> apply(const vector<vector<int>> &matrix) override {
        // TODO: implement apply() Hough Filter
    }
};


class ImageService {
private:
    MatrixOperation matrixOperation;
    SobelFilter sobelFilter;

public:
    Image read(string imageName) {
        ifstream in(imageName);
        int rows, cols;
        in >> rows >> cols;
        vector<vector<int>> matrix(rows, vector<int>(cols));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                in >> matrix[i][j];
            }
        }
        in.close();
        return Image(matrix);
    }

    Image readBmp(string imageName) {
        ifstream in(imageName);

        // file data
        char *bytes;
        int length;

        // image data
        int width, height;
        vector<vector<int>> matrix;

        // read bytes
        in.seekg(0, std::ifstream::end);
        length = in.tellg();
        bytes = new char[length];
        in.seekg(0, std::ifstream::beg);
        in.read(bytes, length);

        // image width and height
        width = *(int *) (bytes + 0x12);
        height = *(int *) (bytes + 0x16);

        // extract image pixels
        matrix = vector<vector<int>>(height, vector<int>(width));
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int pixel = bytes[i * height + j + 0x36];
                if (pixel < 0) {
                    pixel = 256 + (int) pixel;
                }
                matrix[i][j] = pixel;
            }
        }
        matrix = matrixOperation.rotateVertically(matrix);
        return Image(matrix);
    }

    void write(string imageName, Image image) {
        ofstream out(imageName);
        vector<vector<int>> matrix = image.getMatrix();
        int rows = matrix.size();
        int cols = matrix[0].size();
        out << rows << " " << cols << endl;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                out << matrix[i][j] << " ";
            }
            out << endl;
        }
        out.close();
    }

    Image edge(Image image) {
        vector<vector<int>> filteredMatrix = sobelFilter.apply(image.getMatrix());
        return Image(filteredMatrix);
    }

    void display(Image image) {
        // TODO: implement display image
    }
};


int main() {

    ImageService imageService;
    Image lenaImg = imageService.readBmp("lena512.bmp");
    imageService.write("lena512.txt", lenaImg);

    return 0;
}
