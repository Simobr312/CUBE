#include <cstdio>
#include <cstdlib>
#include <cmath>

const double pi = 355./113.;

const int point_lenght = 3; 
struct Point3D { float x, y, z; };

void Point3DToVector(Point3D point, float converted[]) {
    converted[0] = point.x;
    converted[1] = point.y;
    converted[2] = point.z;
}

Point3D VectorToPoint3D(float vec[]) {
    return {vec[0], vec[1], vec[2]};
}

void MultiplyMatrixVec(float vec[], float mat[][point_lenght], float result[]) {
    for(int i = 0 ; i < point_lenght ; ++i) {   
        for(int k = 0 ; k < point_lenght ; ++k) {
            result[i] += vec[k] * mat[k][i];
        }
    }
}

int main() {
    Point3D point = { 2.f, 2.f, 4.f};
    float alpha = pi/2.;

    float point_vec[point_lenght];
    Point3DToVector(point, point_vec);

    printf("Initial coordinate: ");
    for(int i = 0 ; i < point_lenght ; ++i) { 
        printf("%f ", point_vec[i]);
    }

    printf("\n");

    float RotationMatrixZ[][point_lenght] = {
        {(float)cos(alpha), -(float)sin(alpha), 0},
        {(float)sin(alpha), (float)cos(alpha),  0},
        {   0             ,      0           ,  1}
    };

    float distance = 1.;
    float d = 1. / (distance - point.z);
    float ProjectionMatrix[][point_lenght] = {
        {d, 0, 0},
        {0, d, 0},
        {0, 0, 0},
    };

    float* rotation;
    MultiplyMatrixVec(point_vec, ProjectionMatrix, rotation);

    printf("Projected point: ");
    for(int i = 0 ; i < point_lenght ; ++i) {   
            printf("%f ", rotation[i]);
    }

    return 0;
}