#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>

#ifdef __linux__ 
    #define CLEAR "clear"
#elif _WIN32
    #define CLEAR "cls"
#else
#endif

const double pi = 3.1415926535;

const int dimX = 100;
const int dimY = dimX / 2;
const char* glyph = " *";
const int default_originX = dimX / 2, default_originY = dimX / 2;

const float cube_spigol = 60.f;
const int vertices_of_cube = 8;
const int point_lenght = 3; 

struct Point3D {
    union {
        struct {
            float x, y, z; 
        };
        float V[point_lenght];
    };
};

float orthogonalProjectionMatrix[][point_lenght] = {
    {1.f, 0.f, 0.f},
    {0.f, 1.f, 0.f},
    {0.f, 0.f, 0.f},
}; 

clock_t initTime, oldTime;
double dt, elapsedTime;
void calculateTime();

void clear(bool grid[dimX][dimY]);
void render(bool grid[dimX][dimY]);

void multiplyMatrixVec(float vec[], float mat[][point_lenght], float result[]);

void initCube(Point3D cube[vertices_of_cube]);
void setMatrixPoints(bool grid[dimX][dimY], float point[point_lenght]);
void line(bool grid[dimX][dimY], float ox, float oy, float x1, float y1, float x2, float y2 );

int main() {
    printf("Deloped by Simone Riccio\n");
    printf("A rotating cube rendered in 3D with ascii\n");
    char buffer[dimX * dimY];

    bool grid[dimX][dimY]; 
    float r;
    double angleX = 0.   , angleY = 0.   , angleZ = 0.;
    double angleVelX = 0., angleVelY = 0., angleVelZ = 0.;

    printf("Choose the angular velocity x, y, z: (degre/sec)\n");
    scanf("%lf", &angleVelX ); 
    scanf("%lf", &angleVelY ); 
    scanf("%lf", &angleVelZ );

    angleVelX = angleVelX * pi / 180.;
    angleVelY = angleVelY * pi / 180.;
    angleVelZ = angleVelZ * pi / 180.;

    Point3D cube[vertices_of_cube];
    initCube(cube);

    setbuf(stdout, buffer);

    bool isRunning = true;
    while(isRunning) {
        calculateTime();

        clear(grid);

        Point3D projected_points[vertices_of_cube];

        float rotationMatrixX[][point_lenght] = {
            { 1.f               , 0.f               , 0.f               },
            { 0.f               , (float)cos(angleX),-(float)sin(angleX)},
            { 0.f               , (float)sin(angleX), (float)cos(angleX)}
        };

        float rotationMatrixY[][point_lenght] = {
            { (float)cos(angleY) , 0.f              , (float)sin(angleY)},
            { 0.f                , 1.f              , 0.f               },
            {-(float)sin(angleY) , 0.f              , (float)cos(angleY)}
        };

        float rotationMatrixZ[][point_lenght] = {
            { (float)cos(angleZ),-(float)sin(angleZ)  , 0.f              },
            { (float)sin(angleZ), (float)cos(angleZ)  , 0.f              },
            { 0.f               , 0.f                 , 1.f              }
        };

        for(int i = 0 ; i < vertices_of_cube ; ++i) {
            Point3D& point = cube[i];
            
            Point3D rotatedX_point;;
            multiplyMatrixVec(point.V         , rotationMatrixX, rotatedX_point.V);

            Point3D rotatedY_point;
            multiplyMatrixVec(rotatedX_point.V, rotationMatrixY, rotatedY_point.V);

            Point3D rotatedZ_point;
            multiplyMatrixVec(rotatedY_point.V, rotationMatrixZ, rotatedZ_point.V);

            float distance = 200.f; //Maybe I have to change the origin of the cartesian space in order to resolve the weak prespective.
            float prospRatio = 1.f / (distance - rotatedZ_point.z);
            //printf("%f ",  prospRatio);
            float WeakProjectionMatrix[][point_lenght] = {
                { prospRatio        , 0.f                 , 0.f              },
                { 0.f               , prospRatio          , 0.f              },
                { 0.f               , 0.f                 , 0.f              }
            };

            Point3D projected_point;
            multiplyMatrixVec(rotatedZ_point.V, orthogonalProjectionMatrix, projected_point.V);
            setMatrixPoints(grid, projected_point.V);  

            projected_points[i].x = projected_point.x;
            projected_points[i].y = projected_point.y;
            projected_points[i].z = projected_point.z;
        }

        for(int i = 0 ; i < vertices_of_cube / 2 ; ++i) {
            line(grid, default_originX, default_originY, projected_points[i].x, projected_points[i].y, projected_points[(i + 1)%4].x, projected_points[(i + 1) % 4].y );
            line(grid, default_originX, default_originY, projected_points[i].x, projected_points[i].y, projected_points[i + 4].x, projected_points[i + 4].y );
            line(grid, default_originX, default_originY, projected_points[i + 4].x, projected_points[i + 4].y, projected_points[(i + 1) % 4 + 4].x, projected_points[(i + 1) % 4 + 4].y );
        }

        angleX += angleVelX * dt;
        angleY += angleVelY * dt;
        angleZ += angleVelZ * dt;

        render(grid);
    }

    return 0;
}

void clear(bool grid[dimX][dimY]) {
    for(int y = 0 ; y < dimY ; ++y) {
        for(int x = 0 ; x < dimX ; ++x) {
            grid[x][y] = false;
        }
    }   
}

void render(bool output[dimX][dimY]) {
    system(CLEAR);
    for(int y = 0 ; y < dimY ; ++y) {
        for(int x = 0 ; x < dimX ; ++x) {
            putchar(glyph[output[x][y]]);
        }
        putchar('\n');
    }
    fflush(stdout);
}


void multiplyMatrixVec(float vec[], float mat[][point_lenght], float result[]) {
    for(int i = 0 ; i < point_lenght ; ++i) {
        result[i] = 0;
    }

    for(int i = 0 ; i < point_lenght ; ++i) {   
        for(int k = 0 ; k < point_lenght ; ++k) {
            result[i] += vec[k] * mat[k][i];
        }
    }
}

void initCube(Point3D cube[vertices_of_cube]) {
    float coord = cube_spigol / 2.f;
    cube[0] = { coord , -coord,  coord};
    cube[1] = { coord ,  coord,  coord};
    cube[2] = {-coord ,  coord,  coord};
    cube[3] = {-coord , -coord,  coord};
    cube[4] = { coord , -coord, -coord};
    cube[5] = { coord ,  coord, -coord};
    cube[6] = {-coord ,  coord, -coord};
    cube[7] = {-coord , -coord, -coord};
}

bool isInRange(int x, int y) { return x >= 0 && x < dimX && y >= 0 && y < dimY;}

void setMatrixPoints(bool grid[dimX][dimY], float point[point_lenght]) {
    for(int i = 0 ; i < vertices_of_cube ; ++i) {
        int x = default_originX + round(point[0]), y = (default_originY + round(point[1])) / 2;
        if(isInRange(x, y)) grid[x][y] = true;
    }
}

float max(float a, float b) { return a > b ? a : b; }
float min(float a, float b) { return a < b ? a : b; }

void line(bool grid[dimX][dimY], float ox, float oy, float x1, float y1, float x2, float y2 ) {

    float dX = x2 - x1;
    float dY = y2 - y1;
    
    if(abs(dX) > abs(dY)) {
        float m = dY / dX;
        float xi = min(x1, x2); float xf = max(x1, x2);

        for(float x = xi ; x <= xf ; x += 0.1f) {
            float y = m * (x - x1) + y1;

            int xp = ox + round(x), yp = (oy + round(y)) / 2;
            
            if(isInRange(xp, yp)) grid[xp][yp] = true;
        }
    } else {      
        float m = dX / dY;
        float yi = min(y1, y2); float yf = max(y1, y2);
        
        for(float y = yi; y <= yf ; y += 0.1) {
            float x = m * (y - y1) + x1;

            int xp = ox + round(x), yp = (oy + round(y)) / 2;

            if(isInRange(xp, yp))
                grid[xp][yp] = true;
        }
    }
}

void calculateTime() {
    elapsedTime = (clock() - initTime) / (double)CLOCKS_PER_SEC;
    dt = (clock() - oldTime) / (double)CLOCKS_PER_SEC;
    oldTime = clock();
}