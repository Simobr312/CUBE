#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#ifdef __linux__ 
    #define CLEAR "clear"
#elif _WIN32
    #define CLEAR "cls"
#else
#endif

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define pi 3.1415926535

#define character_ratio 1/2
#define dimX 100
#define dimY dimX * character_ratio

#define glyph " *" //Change this as you want

#define cube_spigol 60.f
#define vertices_of_cube 8
#define point_lenght 3 

const int default_originX = dimX / 2, default_originY = dimX / 2;

typedef struct Point3D {
    union {
        struct {
            float x, y, z; 
        };
        float vec[point_lenght];
    };
} point_t;

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

void multiplyMatrixMatrix(point_t first[vertices_of_cube], float second[point_lenght][point_lenght], point_t result[vertices_of_cube]);

void initCube(point_t cube[vertices_of_cube]);
void setMatrixPoints(bool grid[dimX][dimY], float point[point_lenght]);
void line(bool grid[dimX][dimY], float ox, float oy, float x1, float y1, float x2, float y2 );

int main() {
    printf("Developed by Simone Riccio\n");
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

    point_t cube[vertices_of_cube];
    initCube(cube);

    setbuf(stdout, buffer);

    bool isRunning = true;
    while(isRunning) {
        calculateTime();

        clear(grid);

        point_t projected_points[vertices_of_cube];

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

        point_t rotatedX_points[vertices_of_cube], rotatedY_points[vertices_of_cube], rotatedZ_points[vertices_of_cube];
        point_t trasformated_points[vertices_of_cube];

        multiplyMatrixMatrix(cube, rotationMatrixX, rotatedX_points);
        multiplyMatrixMatrix(rotatedX_points, rotationMatrixY, rotatedY_points);
        multiplyMatrixMatrix(rotatedY_points, rotationMatrixZ, rotatedZ_points);
        multiplyMatrixMatrix(rotatedZ_points, orthogonalProjectionMatrix, trasformated_points);

        for(int i = 0 ; i < vertices_of_cube / 2 ; ++i) {
            line(grid, default_originX, default_originY, trasformated_points[i].x, trasformated_points[i].y, trasformated_points[(i + 1)%4].x, trasformated_points[(i + 1) % 4].y );
            line(grid, default_originX, default_originY, trasformated_points[i].x, trasformated_points[i].y, trasformated_points[i + 4].x, trasformated_points[i + 4].y );
            line(grid, default_originX, default_originY, trasformated_points[i + 4].x,  trasformated_points[i + 4].y, trasformated_points[(i + 1) % 4 + 4].x, trasformated_points[(i + 1) % 4 + 4].y );
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

void multiplyMatrixMatrix(point_t first[vertices_of_cube], float second[point_lenght][point_lenght], point_t result[vertices_of_cube]) {
    for(int i = 0; i < vertices_of_cube; ++i ) {
        for(int j = 0; j < point_lenght; ++j) {
            result[i].vec[j] = 0;
            for(int k = 0; k < point_lenght; ++k) {
                result[i].vec[j] += first[i].vec[k] * second[k][j]; 
            }
        }
    }
}

void initCube(point_t cube[vertices_of_cube]) {
    float coord = cube_spigol / 2.f;
    cube[0] = (point_t){ .x =  coord, .y = -coord, .z =  coord};
    cube[1] = (point_t){ .x =  coord, .y =  coord, .z =  coord};
    cube[2] = (point_t){ .x = -coord, .y =  coord, .z =  coord};
    cube[3] = (point_t){ .x = -coord, .y = -coord, .z =  coord};
    cube[4] = (point_t){ .x =  coord, .y = -coord, .z = -coord};
    cube[5] = (point_t){ .x =  coord, .y =  coord, .z = -coord};
    cube[6] = (point_t){ .x = -coord, .y =  coord, .z = -coord};
    cube[7] = (point_t){ .x = -coord, .y = -coord, .z = -coord};
}

bool isInRange(int x, int y) { return x >= 0 && x < dimX && y >= 0 && y < dimY;}

void setMatrixPoints(bool grid[dimX][dimY], float point[point_lenght]) {
    for(int i = 0 ; i < vertices_of_cube ; ++i) {
        int x = default_originX + round(point[0]), y = (default_originY + round(point[1])) * character_ratio;
        if(isInRange(x, y)) grid[x][y] = true;
    }
}

void line(bool grid[dimX][dimY], float ox, float oy, float x1, float y1, float x2, float y2 ) {

    float dX = x2 - x1;
    float dY = y2 - y1;
    
    if(abs(dX) > abs(dY)) {
        float m = dY / dX;
        float xi = min(x1, x2); float xf = max(x1, x2);

        for(float x = xi ; x <= xf ; x += 0.1f) {
            float y = m * (x - x1) + y1;

            int xp = ox + round(x), yp = (oy + round(y)) * character_ratio;
            
            if(isInRange(xp, yp)) grid[xp][yp] = true;
        }
    } else {      
        float m = dX / dY;
        float yi = min(y1, y2); float yf = max(y1, y2);
        
        for(float y = yi; y <= yf ; y += 0.1) {
            float x = m * (y - y1) + x1;

            int xp = ox + round(x), yp = (oy + round(y)) * character_ratio;

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
/*
                             **************************************************
                            ****                                              ******
                          **   *                                               **  *
                        ***    **                                            ***   **
                      ***       *                                          ***      *
                    ***          *                                       ***        **
                   **            **                                     **           *
                 ***              *                                   ***            **
               ***                **                                ***               *
             ***                   *                              ***                 **
            **                     **                            **                    **
          ***                       *                          ***                      *
        ***                         **                       ***                        **
      ***                            *                     ***                           *
     **                              **                   **                             **
    ******************************************************                                *
    **                                **                *                                 **
     *                                 *                **                                 *
     **                                 *                **                                **
      *                                 **                *                                 *
      **                                 *                **                                **
       *                                 **                *                                 *
       **                                 *                **                                **
        *                                 **                *                                 **
        **                                 *                **                                 *
         *                                 **                *                                 **
          *                                **********************************************       *
          **                              **                  *                         *********
           *                            ***                   **                             ***
           **                         ***                      *                           ***
            *                       ***                        **                        ***
            **                     **                           **                      **
             *                   **                              *                    **
             **                ***                               **                 ***
              *              ***                                  *               ***
              **            **                                    **            ***
               *          **                                       *           **
               **       ***                                        **        ***
                *     ***                                           *      ***
                 *  ***                                             **   ***
                 ****                                                *  **
                  *******************************************************
*/