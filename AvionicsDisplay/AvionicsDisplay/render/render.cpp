//
//  render.cpp
//  AvionicsDisplay
//
//  Created by Bernardo Lorenzini on 05/08/23.
//



#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <iostream>
#include <cmath>


#include "../render.hpp"


int height = 600;
int width = 1000;

SDL_Window* window = nullptr;
SDL_GLContext context = nullptr;

SDL_Renderer* renderer = nullptr;

bool gQuit = false;




void InitProgram(){
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "SDL2 could not initialize"<< std::endl;
        exit(1);
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    
     window = SDL_CreateWindow("EFIS", 0, 0, width, height, SDL_WINDOW_OPENGL);
    
    if(window == nullptr){
        std::cout << "SDL_window could not open"<< std::endl;
        exit(1);
    }
    
    context = SDL_GL_CreateContext(window);
    
    if(context == nullptr){
        std::cout << "CONTEXT openGL not available" << std::endl;
        exit(1);
    }
    
    //INITIALIZE GLAD
    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
        std::cout << "glad not initialize" << std::endl;
        exit(1);
    }
    
    GetGLVersionInfo();
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
       if (renderer == nullptr) {
           std::cout << "SDL_renderer could not be created" << std::endl;
           exit(1);
       }
    
};

void MainLoop(){
    
    while(!gQuit){
        
        Input();
        
        PreDraw();
        
        Draw();
        
        
        //update the screen
        SDL_GL_SwapWindow(window);
        
        
    }
    
};

void CleanUp(){
    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);
    SDL_Quit();
    
};


void Input(){
    
    SDL_Event e;
    
    while(SDL_PollEvent(&e) != 0){
        if(e.type == SDL_QUIT){
            std::cout << "SYSTEM SHUT DOWN" << std::endl;
            gQuit = true;
        }
        
    }
    
};

void PreDraw(){};

void DrawRedCircle() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
    int radius = height * 0.2; // Adjust the radius as needed
    int center_x = width * 3 / 4;
    int center_y = height / 2;

    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPoint(renderer, center_x + x, center_y + y);
            }
        }
    }
}

void DrawBlueTriangle() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue color

    SDL_Point points[3] = {
        {width / 4, height / 2 + height / 4},
        {width / 4 + width / 4, height / 2 + height / 4},
        {width / 4 + width / 8, height / 2 - height / 4}
    };

    SDL_RenderDrawLines(renderer, points, 3);
}

void Draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the left column with a red background
    SDL_Rect leftRect = {0, 0, width / 2, height};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &leftRect);

    // Draw the right column with a blue background
    SDL_Rect rightRect = {width / 2, 0, width / 2, height};
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &rightRect);

    // Draw the red circle in the center of the right column
    DrawRedCircle();

    // Draw the blue triangle in the center of the left column
    DrawBlueTriangle();

    // Update the screen
    SDL_RenderPresent(renderer);
}



void GetGLVersionInfo(){
    
    std::cout << "Vendor: " << glad_glGetString(GL_VENDOR)<< std::endl;
    std::cout << "Renderer: " << glad_glGetString(GL_RENDERER)<< std::endl;
    std::cout << "Version: " << glad_glGetString(GL_VERSION)<< std::endl;
    std::cout << "Shading Language: " << glad_glGetString(GL_SHADING_LANGUAGE_VERSION)<< std::endl;

};
