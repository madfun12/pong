#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
int aiSpeed = 2;

typedef enum {
    PLAYER,
    AI
} PaddleType;

typedef struct {
    int width;
    int height;
    int px;
    int py;
} Paddle;

typedef struct {
    int width;
    int height;
    int px;
    int py;
    int vx;
    int vy;
} Ball;


Paddle createPaddle(PaddleType type){
    Paddle p;
    p.height = 5;
    p.width = 80;
    if(type == PLAYER){
        p.py = WINDOW_HEIGHT - 20;
        p.px = WINDOW_WIDTH - (WINDOW_WIDTH / 2) - (p.width / 2);
    } else{
        p.py = 10;
        p.px = 20;
    }

    return p;
};

Ball createBall(){
    Ball b;
    b.height = 20;
    b.width = 20;
    b.vx = 0;
    b.vy = 5;
    b.px = WINDOW_WIDTH / 2 + (b.width / 2);
    b.py = WINDOW_HEIGHT / 2 - (b.height / 2);

    return b;
}

void moveBall(Ball* ball){
    // Check for x bounce
    if(ball->px <= 0 || ball->px + ball->width >= WINDOW_WIDTH){
        ball->vx *= -1;
    }
    if(ball->py <= 0 || ball->py + ball->height >= WINDOW_HEIGHT){
        ball->vy *= -1;
    }

    ball->px += ball->vx;
    ball->py += ball->vy;
}

bool checkCollision(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh) {
    return ax < bx + bw &&
           ax + aw > bx &&
           ay < by + bh &&
           ay + ah > by;
}

void moveAIPaddle(Paddle* ai, Ball* ball){
    if(ball->vy < 0){ // Ball is moving toward the AI
        float timeToAIPaddle = ball->py / -ball->vy;
        int predictedX = ball->px + ball->vx * timeToAIPaddle - (ball->width / 2);

        // Reflect off walls if needed
        while(predictedX < 0 || predictedX > WINDOW_WIDTH){
            if(predictedX < 0){
                // Ball will go off to the left so invert prediction to account for bounce
                predictedX = -predictedX;
            } 
            else if(predictedX > WINDOW_WIDTH){
                // Ball will go outside of the window to the right, so
                predictedX = (2 * WINDOW_WIDTH) - predictedX;
            }        
        }

        // Move AI paddle toward predictedX
        if(ai->px + ai->width / 2 < predictedX){
            ai->px += aiSpeed;
        } else if(ai->px + ai->width / 2 > predictedX){
            ai->px -= aiSpeed;
        }
    }
}

void drawPixels(Uint32* framebuffer, Paddle* player, Paddle* ai, Ball* ball){
    // Draw player paddle
    for(int y = player->py; y < player->py + player->height; y++){
        for(int x = player->px; x < player->px + player->width; x++){
            if(x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT){
                framebuffer[y * WINDOW_WIDTH + x] = 0xffffffff;
            }
        }
    }
    
    // Draw AI paddle
    for(int y = ai->py; y < ai->py + ai->height; y++){
        for(int x = ai->px; x < ai->px + ai->width; x++){
            if(x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT){
                framebuffer[y * WINDOW_WIDTH + x] = 0xffffffff;
            }
        }
    }

    // Draw ball
    for(int y = ball->py; y < ball->py + ball->height; y++){
        for(int x = ball->px; x < ball->px + ball->width; x++){
            if(x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT){
                framebuffer[y * WINDOW_WIDTH + x] = 0xffffffff;
            }
        }
    }
}

void updateTexture(SDL_Texture* texture, Uint32* framebuffer){
    void* pixels;
    int pitch;

    if(!SDL_LockTexture(texture, NULL, &pixels, &pitch)){
        fprintf(stderr, "Failed to lock texture: %s\n", SDL_GetError());
        return;
    }

    memcpy(pixels, framebuffer, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(Uint32));

    SDL_UnlockTexture(texture);
}

int main() {
    // INITIALIZE SDL
    if(!SDL_Init(SDL_INIT_VIDEO)){
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    // WINDOW
    SDL_Window* window = SDL_CreateWindow("Pong", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if(!window){
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if(!renderer){
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // TEXTURE = FRAMBUFFER
    Uint32* framebuffer = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(Uint32));
    if(!framebuffer){
        fprintf(stderr, "Failed to allocate framebuffer\n");
        return 1;
    } 
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Create players
    Paddle player = createPaddle(PLAYER);
    Paddle ai = createPaddle(AI);

    // Create ball
    Ball ball = createBall();

    // LOOP
    bool running = true;
    SDL_Event event;
    bool leftPressed = false;
    bool rightPressed = false;
    while(running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
            if(event.type == SDL_EVENT_KEY_DOWN){
                if(event.key.key == SDLK_A || event.key.key == SDLK_LEFT){
                    leftPressed = true;
                }
                if(event.key.key == SDLK_D || event.key.key == SDLK_RIGHT){
                    rightPressed = true;
                }
            }
            if(event.type == SDL_EVENT_KEY_UP){
                if(event.key.key == SDLK_A || event.key.key == SDLK_LEFT){
                    leftPressed = false;
                }
                if(event.key.key == SDLK_D || event.key.key == SDLK_RIGHT){
                    rightPressed = false;
                }
            }
        }

        int playerSpeed = 10; // Adjust for desired speed
        if(leftPressed && player.px > 0){
            player.px -= playerSpeed;
        }

        if(rightPressed && player.px < WINDOW_WIDTH - player.width){
            player.px += playerSpeed;
        }

        moveAIPaddle(&ai, &ball);

        if (checkCollision(ball.px, ball.py, ball.width, ball.height, player.px, player.py, player.width, player.height)) {
            int ballCenterX = ball.px + ball.width / 2;
            int paddleCenterX = player.px + player.width / 2;
            int distanceFromCenter = ballCenterX - paddleCenterX;
            ball.vx = distanceFromCenter / 5; // arbitrary "spin" factor
            ball.vy *= -1;
        }
        if (checkCollision(ball.px, ball.py, ball.width, ball.height, ai.px, ai.py, ai.width, ai.height)) {
            ball.vy *= -1;
        }

        // Check if ball hits either side
        if(ball.py <= 0){
            ball = createBall();
        }

        if(ball.py + ball.height >= WINDOW_HEIGHT){
            ball = createBall();
        }

        moveBall(&ball);

        memset(framebuffer, 0, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(Uint32));
        drawPixels(framebuffer, &player, &ai, &ball);
        SDL_RenderClear(renderer);
        SDL_UpdateTexture(texture, NULL, framebuffer, WINDOW_WIDTH * sizeof(Uint32));
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(framebuffer);

    return 0;
}
