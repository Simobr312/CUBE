#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>

const double pi = 355./113.;

const int dimX = 50;
const int dimY = dimX / 2;
const char* glyph = " *";
const int default_originX = dimX / 2, default_originY = dimX / 2;

const float cube_spigol = 30.f;
const int vertices_of_cube = 8;
const int point_lenght = 3; 

struct Point3D {
    union {
        struct {
            float x, y, z; 
        };
        float V[vertices_of_cube];
    };
};

float OrthogonalProjectionMatrix[][point_lenght] = {
    {1.f, 0.f, 0.f},
    {0.f, 1.f, 0.f},
    {0.f, 0.f, 0.f},
}; 

clock_t initTime, oldTime;
double dt, elapsedTime;
void CalculateTime();

void Clear(bool grid[dimX][dimY]);
void Render(bool grid[dimX][dimY]);

void MultiplyMatrixVec(float vec[], float mat[][point_lenght], float result[]);

void InitCube(Point3D cube[vertices_of_cube]);
void SetMatrixPoints(bool grid[dimX][dimY], float point[point_lenght]);
void Line(bool grid[dimX][dimY], float ox, float oy, float x1, float y1, float x2, float y2 );

int main() {
    printf("Deloped by Simone Riccio\n");
    printf("A rotating cube rendered in 3D with ascii\n");

    bool grid[dimX][dimY]; 
    float r;
    double AngleX = 0.   , AngleY = 0.   , AngleZ = 0.;
    double AngleVelX = 0., AngleVelY = 0., AngleVelZ = 0.;

    printf("Choose the angular velocity x, y, z: (degre/sec)\n");
    scanf("%lf", &AngleVelX ); 
    scanf("%lf", &AngleVelY ); 
    scanf("%lf", &AngleVelZ );

    AngleVelX = AngleVelX * pi / 180.;
    AngleVelY = AngleVelY * pi / 180.;
    AngleVelZ = AngleVelZ * pi / 180.;

    Point3D cube[vertices_of_cube];
    InitCube(cube);

    bool isRunning = true;
    while(isRunning) {
        CalculateTime();

        Clear(grid);

        Point3D projected_points[vertices_of_cube];

        float RotationMatrixX[][point_lenght] = {
            { 1.f               , 0.f               , 0.f               },
            { 0.f               , (float)cos(AngleX),-(float)sin(AngleX)},
            { 0.f               , (float)sin(AngleX), (float)cos(AngleX)}
        };

        float RotationMatrixY[][point_lenght] = {
            { (float)cos(AngleY) , 0.f              , (float)sin(AngleY)},
            { 0.f                , 1.f              , 0.f                },
            {-(float)sin(AngleY) , 0.f              , (float)cos(AngleY)}
        };

        float RotationMatrixZ[][point_lenght] = {
            { (float)cos(AngleZ),-(float)sin(AngleZ)  , 0.f              },
            { (float)sin(AngleZ), (float)cos(AngleZ)  , 0.f              },
            { 0.f               , 0.f                 , 1.f              }
        };

        for(int i = 0 ; i < vertices_of_cube ; ++i) {
            Point3D& point = cube[i];
            
            Point3D rotatedX_point;;
            MultiplyMatrixVec(point.V         , RotationMatrixX, rotatedX_point.V);

            Point3D rotatedY_point;
            MultiplyMatrixVec(rotatedX_point.V, RotationMatrixY, rotatedY_point.V);

            Point3D rotatedZ_point;
            MultiplyMatrixVec(rotatedY_point.V, RotationMatrixZ, rotatedZ_point.V);

            float distance = 200.f; //Maybe I have to change the origin of the cartesian space in order to resolve the weak prespective.
            float prospRatio = 1.f / (distance - rotatedZ_point.z);
            //printf("%f ",  prospRatio);
            float WeakProjectionMatrix[][point_lenght] = {
                { prospRatio        , 0.f                 , 0.f              },
                { 0.f               , prospRatio          , 0.f              },
                { 0.f               , 0.f                 , 0.f              }
            };

            Point3D projected_point;
            MultiplyMatrixVec(rotatedZ_point.V, OrthogonalProjectionMatrix, projected_point.V);
            SetMatrixPoints(grid, projected_point.V);  

            projected_points[i].x = projected_point.x;
            projected_points[i].y = projected_point.y;
            projected_points[i].z = projected_point.z;
        }

        for(int i = 0 ; i < vertices_of_cube / 2 ; ++i) {
            Line(grid, default_originX, default_originY, projected_points[i].x, projected_points[i].y, projected_points[(i + 1)%4].x, projected_points[(i + 1) % 4].y );
            Line(grid, default_originX, default_originY, projected_points[i].x, projected_points[i].y, projected_points[i + 4].x, projected_points[i + 4].y );
            Line(grid, default_originX, default_originY, projected_points[i + 4].x, projected_points[i + 4].y, projected_points[(i + 1) % 4 + 4].x, projected_points[(i + 1) % 4 + 4].y );
        }

        AngleX += AngleVelX * dt;
        AngleY += AngleVelY * dt;
        AngleZ += AngleVelZ * dt;

        system("cls");
        Render(grid);
    }

    return 0;
}

void Clear(bool grid[dimX][dimY]) {
    for(int y = 0 ; y < dimY ; ++y) {
        for(int x = 0 ; x < dimX ; ++x) {
            grid[x][y] = false;
        }
    }   
}

void Render(bool output[dimX][dimY]) {
    for(int y = 0 ; y < dimY ; ++y) {
        for(int x = 0 ; x < dimX ; ++x) {
            putchar(glyph[output[x][y]]);
        }
        putchar('\n');
    }
}


void MultiplyMatrixVec(float vec[], float mat[][point_lenght], float result[]) {
    for(int i = 0 ; i < point_lenght ; ++i) {
        result[i] = 0;
    }

    for(int i = 0 ; i < point_lenght ; ++i) {   
        for(int k = 0 ; k < point_lenght ; ++k) {
            result[i] += vec[k] * mat[k][i];
        }
    }
}

void InitCube(Point3D cube[vertices_of_cube]) {
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

void SetMatrixPoints(bool grid[dimX][dimY], float point[point_lenght]) {
    for(int i = 0 ; i < vertices_of_cube ; ++i) {
        int x = default_originX + round(point[0]), y = (default_originY + round(point[1])) / 2;
        if(isInRange(x, y)) grid[x][y] = true;
    }
}

float Max(float a, float b) { return a > b ? a : b; }
float Min(float a, float b) { return a < b ? a : b; }

void Line(bool grid[dimX][dimY], float ox, float oy, float x1, float y1, float x2, float y2 ) {

    float dX = x2 - x1;
    float dY = y2 - y1;
    
    if(abs(dX) > abs(dY)) {
        float m = dY / dX;
        float xi = Min(x1, x2); float xf = Max(x1, x2);

        for(float x = xi ; x <= xf ; x += 0.1f) {
            float y = m * (x - x1) + y1;

            int xp = ox + round(x), yp = (oy + round(y)) / 2;
            
            if(isInRange(xp, yp)) grid[xp][yp] = true;
        }
    } else {      
        float m = dX / dY;
        float yi = Min(y1, y2); float yf = Max(y1, y2);
        
        for(float y = yi; y <= yf ; y += 0.1) {
            float x = m * (y - y1) + x1;

            int xp = ox + round(x), yp = (oy + round(y)) / 2;

            if(isInRange(xp, yp))
                grid[xp][yp] = true;
        }
    }
}

void CalculateTime() {
    elapsedTime = (clock() - initTime) / (double)CLOCKS_PER_SEC;
    dt = (clock() - oldTime) / (double)CLOCKS_PER_SEC;
    oldTime = clock();
}