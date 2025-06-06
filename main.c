#include <stdio.h>
#include <SDL3/SDL.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

#define RGBA(c) \
    ((c >> 24) & 0xFF), \
    ((c >> 16) & 0xFF), \
    ((c >> 8) & 0xFF), \
    (c & 0xFF)


#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_GRAY 0xB0B0B0FF
#define COLOR_BLACK 0x000000FF

#define CELL_SIZE 18
#define ROWS SCREEN_HEIGHT / CELL_SIZE
#define COLS SCREEN_WIDTH / CELL_SIZE
#define GRIDLINE_WIDTH 1

#define GENERATION_SPEED 10 // once each x game loop iteration

struct point {
    int row, col;
    int state; // 0 is dead, 1 is alive
};


struct point points[ROWS][COLS];


void init_points(){

    // store points
    for (int row = 0; row < ROWS; row++) {
        for(int col = 0; col < COLS; col++) {
            points[row][col].row = row;
            points[row][col].col = col;
            points[row][col]. state = 0;
        }

    }
}


// update points
void update_points() {
    int next_state[ROWS][COLS];

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int total_alive_neighbors = 0;

            // for neighbors
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    if (x == 0 && y == 0) continue; // skip itself

                    int neigh_r = row + x;
                    int neigh_c = col + y;

                    if (neigh_r >= 0 && neigh_r < ROWS && neigh_c >= 0 && neigh_c < COLS) {
                        if (points[neigh_r][neigh_c].state == 1) {
                            total_alive_neighbors++;
                        }
                    }
                }
            }

            // rules
            if (points[row][col].state == 1) { // if already alive
                if (total_alive_neighbors  == 2 || total_alive_neighbors == 3)
                    next_state[row][col] = 1;
                else
                    next_state[row][col] = 0;
            } else {
                if (total_alive_neighbors == 3)
                    next_state[row][col] = 1; // rule of birth
                else
                    next_state[row][col] = 0; // stays dead
            }
        }
    }

    // Copy back to original grid
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            points[row][col].state = next_state[row][col];
        }
    }
}


void reset_all_points() {

    for (int row = 0; row < ROWS; row++) {
        for(int col = 0; col < COLS; col++) {
            points[row][col].state = 0;
        }
    }
}

void draw_grid(SDL_Renderer *renderer) {

    SDL_SetRenderDrawColor(renderer, RGBA(COLOR_GRAY));

    // rows
    for (int i = 0; i < SCREEN_HEIGHT; i += CELL_SIZE) {
        SDL_FRect line = {0, i, SCREEN_WIDTH, GRIDLINE_WIDTH};
        SDL_RenderFillRect(renderer, &line);
    }

    // for cols
    for (int i = 0; i < SCREEN_WIDTH; i += CELL_SIZE) {
        SDL_FRect line = {i, 0, GRIDLINE_WIDTH, SCREEN_WIDTH};
        SDL_RenderFillRect(renderer, &line);
    }

}


void toggle_point(SDL_Renderer *renderer, float x, float y, int btnIndex) {

   
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }

    int cellRow = y / CELL_SIZE;
    int cellCol = x / CELL_SIZE;

    if (cellRow < 0 || cellRow >= ROWS || cellCol < 0 || cellCol >= COLS) {
        return;
    }

    int next_state;

    if(1 == btnIndex) {
        next_state = 1;
    } else if ((3 == btnIndex) || (4 == btnIndex)) {
        next_state = 0;
    }

    // draw
    SDL_FRect point = {(cellCol * CELL_SIZE + GRIDLINE_WIDTH), (cellRow * CELL_SIZE+ 1), CELL_SIZE - GRIDLINE_WIDTH, CELL_SIZE - GRIDLINE_WIDTH};

    if (1 == next_state) {
        SDL_SetRenderDrawColor(renderer, RGBA(COLOR_BLACK));
    } else {
        SDL_SetRenderDrawColor(renderer, RGBA(COLOR_WHITE));  
    }

    SDL_RenderFillRect(renderer, &point);

    points[cellRow][cellCol].state = next_state;
}



void draw_points(SDL_Renderer *renderer) {
    
    SDL_SetRenderDrawColor(renderer, RGBA(COLOR_BLACK));

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col <COLS; col++){
            if(1 == points[row][col].state) {
                SDL_FRect point = {(col * CELL_SIZE + GRIDLINE_WIDTH), (row * CELL_SIZE+ 1), CELL_SIZE - GRIDLINE_WIDTH, CELL_SIZE - GRIDLINE_WIDTH};

                SDL_RenderFillRect(renderer, &point);

            }
        }
    }

}

void handleMouseClick(SDL_Renderer *renderer, SDL_MouseButtonEvent *button) {

    int btnIndex = button->button;
    float mouseX = button->x;
    float mouseY = button->y;

    if(btnIndex) {
        toggle_point(renderer, mouseX, mouseY, btnIndex);
    }

}


int main() {
    // initializing SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL couldn't be initialized! SDL_Errow: %s\n", SDL_GetError());
        return 1;
    }

    // printing controls
    printf("\n          Controls\n");
    printf("\n    Right Click : Populate Cell\n");
    printf("    Left Click  : Depopulate Cell\n");
    printf("    Space       : Reset\n");
    printf("    Enter       : Start Game\n");
    printf("    ESC         : Pause Game\n");
    printf("    E           : End Game\n");

    SDL_Window *window = SDL_CreateWindow("Conway's Game of Life", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    init_points();

    int running = 1;

    int gameStarted = 0;
    int gamePaused = 0;

    int isMouseDown;

    int generation_counter = 0;

    while(running) {
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            switch(event.type) {
                case SDL_EVENT_QUIT:
                    running = 0; break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    isMouseDown = 1; break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    isMouseDown = 0; break;

                case SDL_EVENT_KEY_DOWN:
                    if(event.key.key == SDLK_SPACE && !gameStarted) {
                        reset_all_points();
                    }

                    if(event.key.key == SDLK_RETURN) {
                        gameStarted = 1;
                    }

                    if (event.key.key == SDLK_E) {
                        gameStarted = 0;
                    }

                    if (event.key.key == SDLK_ESCAPE) {
                        gamePaused = (gamePaused == 0)? 1 : 0; 
                    }

                default:
                    break;
            }

            if(isMouseDown && !gameStarted) {
                handleMouseClick(renderer, &event.button);
            }

        }

        if (!gamePaused) {

            
            // re-draw white bg on each update
            SDL_SetRenderDrawColor(renderer, RGBA(COLOR_WHITE));
            SDL_RenderClear(renderer);
            
            

            // draw grid
            draw_grid(renderer);

            if (gameStarted) {
                if (generation_counter > GENERATION_SPEED){
                    update_points();
                    draw_points(renderer);
                    generation_counter = 0;
                } else {
                    generation_counter++;
                }
            }

            draw_points(renderer);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
        
    }
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
    

    return 0;
}


