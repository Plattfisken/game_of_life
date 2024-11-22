#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/raylib.h"

#define CELL_WIDTH 2
#define CELL_HEIGHT 2

// must be powers of 2 for wrapping to work properly. (I think..?)
#define ROW_SIZE 4096
#define COL_SIZE 4096
#define GRID_SIZE ((ROW_SIZE)*(COL_SIZE))

#define IDX_2D(x,y) ((x)+((y)*(ROW_SIZE)))


void addGosperGun(unsigned char *cells, size_t size, int x, int y) {
    cells[IDX_2D(x + 1,y + 5) % size] = 1;
    cells[IDX_2D(x + 1,y + 6) % size] = 1;

    cells[IDX_2D(x + 2,y + 5) % size] = 1;
    cells[IDX_2D(x + 2,y + 6) % size] = 1;

    cells[IDX_2D(x + 11,y + 5) % size] = 1;
    cells[IDX_2D(x + 11,y + 6) % size] = 1;
    cells[IDX_2D(x + 11,y + 7) % size] = 1;

    cells[IDX_2D(x + 12,y + 4) % size] = 1;
    cells[IDX_2D(x + 12,y + 8) % size] = 1;

    cells[IDX_2D(x + 13,y + 3) % size] = 1;
    cells[IDX_2D(x + 13,y + 9) % size] = 1;

    cells[IDX_2D(x + 14,y + 3) % size] = 1;
    cells[IDX_2D(x + 14,y + 9) % size] = 1;

    cells[IDX_2D(x + 15,y + 6) % size] = 1;

    cells[IDX_2D(x + 16,y + 4) % size] = 1;
    cells[IDX_2D(x + 16,y + 8) % size] = 1;

    cells[IDX_2D(x + 17,y + 5) % size] = 1;
    cells[IDX_2D(x + 17,y + 6) % size] = 1;
    cells[IDX_2D(x + 17,y + 7) % size] = 1;

    cells[IDX_2D(x + 18,y + 6) % size] = 1;

    cells[IDX_2D(x + 21,y + 3) % size] = 1;
    cells[IDX_2D(x + 21,y + 4) % size] = 1;
    cells[IDX_2D(x + 21,y + 5) % size] = 1;

    cells[IDX_2D(x + 22,y + 3) % size] = 1;
    cells[IDX_2D(x + 22,y + 4) % size] = 1;
    cells[IDX_2D(x + 22,y + 5) % size] = 1;

    cells[IDX_2D(x + 23,y + 2) % size] = 1;
    cells[IDX_2D(x + 23,y + 6) % size] = 1;

    cells[IDX_2D(x + 25,y + 1) % size] = 1;
    cells[IDX_2D(x + 25,y + 2) % size] = 1;
    cells[IDX_2D(x + 25,y + 6) % size] = 1;
    cells[IDX_2D(x + 25,y + 7) % size] = 1;

    cells[IDX_2D(x + 35,y + 3) % size] = 1;
    cells[IDX_2D(x + 35,y + 4) % size] = 1;
    cells[IDX_2D(x + 36,y + 3) % size] = 1;
    cells[IDX_2D(x + 36,y + 4) % size] = 1;
}

void addAcorn(unsigned char *cells, size_t size, int x, int y) {
    cells[IDX_2D(x,y)] = 1;
    cells[IDX_2D(x + 1,y)] = 1;
    cells[IDX_2D(x + 1,y - 2)] = 1;
    cells[IDX_2D(x + 3,y - 1)] = 1;
    cells[IDX_2D(x + 4,y)] = 1;
    cells[IDX_2D(x + 5,y)] = 1;
    cells[IDX_2D(x + 6,y)] = 1;
}

unsigned char updateCell(unsigned char *cells, size_t size, unsigned int cellIndex) {
    int aliveNeighborsCount = cells[(cellIndex-1) % size]            +
                              cells[(cellIndex-ROW_SIZE) % size]     +
                              cells[(cellIndex+1) % size]            +
                              cells[(cellIndex+ROW_SIZE) % size]     +
                              cells[(cellIndex-(ROW_SIZE+1)) % size] +
                              cells[(cellIndex-(ROW_SIZE-1)) % size] +
                              cells[(cellIndex+(ROW_SIZE+1)) % size] +
                              cells[(cellIndex+(ROW_SIZE-1)) % size];

    if(aliveNeighborsCount == 3) return 1;
    if(aliveNeighborsCount == 2) return cells[cellIndex];
    return 0;
}

typedef struct {
    unsigned char *cells;
    unsigned char *cells2;
    size_t startIdx;
    size_t endIdx;
} updatePartOfArray_args;

void *updatePartOfArray(void *args_struct) {
    updatePartOfArray_args *args = (updatePartOfArray_args *)args_struct;
    for(int i = args->startIdx; i < args->endIdx; ++i) {
        args->cells2[i] = updateCell(args->cells, (unsigned int)GRID_SIZE, i);
    }
    return NULL;
}

int main(void) {
    // setup
    // initializing window
    InitWindow(1, 1, "Game of Life");
    if(!IsWindowReady()) {
        printf("Window failed to initialize\n");
        return 0;
    }
    ToggleBorderlessWindowed();

    // initialize camera
    Camera2D camera = {0};
    camera.zoom = 1.0;

    unsigned char *cells = calloc(GRID_SIZE, sizeof(char));
    unsigned char *cells2 = calloc(GRID_SIZE, sizeof(char));

    // initializing cell state
    for(int i = 0; i < 20; ++i) {
        addGosperGun(cells, GRID_SIZE, 30, 200 * (i+1));
        addAcorn(cells, GRID_SIZE, 200 * (i+1), 200);
    }

    int generationCount = 0;
    int shouldPlay = 1;

    // main loop
    while(shouldPlay) {
        // process input
        {
            if(IsKeyPressed(KEY_Q)) shouldPlay = false;

            if(IsKeyDown(KEY_LEFT))  camera.target.x -= 1000 * GetFrameTime() / camera.zoom;
            if(IsKeyDown(KEY_RIGHT)) camera.target.x += 1000 * GetFrameTime() / camera.zoom;

            if(IsKeyDown(KEY_UP))    camera.target.y -= 1000 * GetFrameTime() / camera.zoom;
            if(IsKeyDown(KEY_DOWN))  camera.target.y += 1000 * GetFrameTime() / camera.zoom;

            if(IsKeyDown(KEY_SPACE)) camera.zoom += 1.0f * GetFrameTime();
            if(IsKeyDown(KEY_LEFT_SHIFT)) camera.zoom -= 1.0f * GetFrameTime();

            if(camera.zoom < 0.1) camera.zoom = 0.1;
            if(camera.zoom > 5.0) camera.zoom = 5.0;
        }

        // update
        {
            int halfIdx = GRID_SIZE / 2;
            pthread_t thread;
            updatePartOfArray_args threadArgs = {cells, cells2, 0, halfIdx};
            pthread_create(&thread, NULL, updatePartOfArray, &threadArgs);

            updatePartOfArray_args args = {cells, cells2, halfIdx + 1, GRID_SIZE};
            updatePartOfArray(&args);

            pthread_join(thread, NULL);

            generationCount++;

            // swap pointers
            unsigned char *tmp = cells;
            cells = cells2;
            cells2 = tmp;
        }

        // render
        {
            BeginDrawing();
            ClearBackground(WHITE);

            BeginMode2D(camera);

            DrawRectangle(0 ,0 ,ROW_SIZE * CELL_WIDTH, COL_SIZE * CELL_HEIGHT, GREEN);

            {
                Vector2 screenMin = camera.target;
                Vector2 screenMax = (Vector2){ screenMin.x + GetScreenWidth(), screenMin.y + GetScreenHeight() };
                size_t firstCellOnScreenIdx = (screenMin.x + screenMin.y * ROW_SIZE) / CELL_WIDTH;
                // TODO: Account for zoom?
                int cellsPerRowOnScreen = GetScreenWidth() / CELL_WIDTH;

                for(int i = firstCellOnScreenIdx; i < GRID_SIZE; ++i) {
                    Vector2 cellPos = (Vector2){ i % ROW_SIZE * CELL_WIDTH, i / ROW_SIZE * CELL_HEIGHT };
                    if(cellPos.x > screenMax.x && cellPos.y > screenMax.y)
                        break;
                    else if(cellPos.x > screenMax.x) {
                        i += ROW_SIZE - cellsPerRowOnScreen;
                        continue;
                    }
                    if(cells[i])
                        DrawRectangleV(cellPos, (Vector2){ CELL_WIDTH, CELL_HEIGHT }, RED);
                }
            }

            EndMode2D();

            DrawFPS(30, 30);

            char outputBuf[64];
            sprintf(outputBuf, "generations: %d\nx: %d\ny: %d", generationCount, (int)camera.target.x, (int)camera.target.y);
            DrawText(outputBuf,30, 50, 20, BLACK);
            EndDrawing();
        }
    }
}
