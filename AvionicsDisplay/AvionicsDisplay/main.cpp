#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

SDL_Window* window = nullptr;
SDL_GLContext context;

// Define your variables for position adjustments here
float circleYPositions = 0.0f;
float YPositions = 0.0f;
float circleRotations = 0.0f;
float slipSkidAmount = 0.0f;
float speedYPositions = 0.0f;

void     Draw();


void handleKeys(const Uint8* keys)
{
    // Check SDL2 keyboard events here
    if (keys[SDL_SCANCODE_W]) {
        circleYPositions -= 0.005f;
    }
    if (keys[SDL_SCANCODE_S]) {
        circleYPositions += 0.005f;
    }
    if (keys[SDL_SCANCODE_UP]) {
        YPositions -= 0.0005f;
    }
    if (keys[SDL_SCANCODE_DOWN]) {
        YPositions += 0.0005f;
    }
    if (keys[SDL_SCANCODE_Q]) {
        circleRotations -= 0.5f;
    }
    if (keys[SDL_SCANCODE_E]) {
        circleRotations += 0.5f;
    }
    if (keys[SDL_SCANCODE_Z]) {
        slipSkidAmount -= 0.001f;
    }
    if (keys[SDL_SCANCODE_X]) {
        slipSkidAmount += 0.001f;
    }
    if (keys[SDL_SCANCODE_Y]) {
        speedYPositions -= 0.0005f;
    }
    if (keys[SDL_SCANCODE_H]) {
        speedYPositions += 0.0005f;
    }
}

bool initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    return true;
}

SDL_Window* createWindow()
{
    SDL_Window* window = SDL_CreateWindow("EICAS Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
    {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    return window;
}

bool initGLAD()
{
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "GLAD initialization failed!" << std::endl;
        return false;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    return true;
}

/*
 void renderEICAS()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Placeholder values for the information
    float outsideAirTemp = 0.0f;
    float n1RPM = 0.0f;
    float exhaustGasTemp = 0.0f;
    float n2RPM = 0.0f;
    float fuelFlow = 0.0f;
    float fuelUsed = 0.0f;
    float fuelInTanks = 0.0f;
    float oilPressure = 0.0f;
    float oilTemp = 0.0f;
    float oilQuantity = 0.0f;
    float engineVibration = 0.0f;
    float hydraulicPressure = 0.0f;
    float hydraulicQuantity = 0.0f;

    // TODO: Render the EICAS components using the above data

    SDL_GL_SwapWindow(window);
}
 
 */
void renderText(const char* text, float x, float y, float scale = 1.0f, glm::vec3 color = glm::vec3(1.0f))
{
    // Placeholder rendering for text
}

void renderGaul(float x, float y, float value)
{
    // Placeholder rendering for Gaul
}

void renderEICAS()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Render column titles
    renderText("LOW OIL PRESSURE", 100.0f, WINDOW_HEIGHT - 20.0f);
    renderText("LOW OIL PRESSURE", 400.0f, WINDOW_HEIGHT - 20.0f);

    // Render row titles
    renderText("OIL P", 20.0f, WINDOW_HEIGHT - 80.0f, 1.5f, glm::vec3(0.0f, 0.0f, 1.0f));
    renderText("OIL T", 20.0f, WINDOW_HEIGHT - 240.0f, 1.5f, glm::vec3(0.0f, 0.0f, 1.0f));
    renderText("VIB", 20.0f, WINDOW_HEIGHT - 400.0f, 1.5f, glm::vec3(0.0f, 0.0f, 1.0f));

    // Render Gaul values
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            float x = 150.0f + j * 300.0f;
            float y = WINDOW_HEIGHT - (80.0f + i * 160.0f);

            renderGaul(x, y, 0.0f);
            renderGaul(x + 50.0f, y, 50.0f);
            renderGaul(x + 100.0f, y, 100.0f);
        }
    }
    
    Draw();


    SDL_GL_SwapWindow(window);
}

int height = 600;
int width = 1000;

SDL_Renderer* renderer = nullptr;

bool gQuit = false;

int main(int argc, char* argv[])
{
    if (!initSDL())
        return -1;

    window = createWindow();
    if (!window)
    {
        SDL_Quit();
        return -1;
    }

    if (!initGLAD())
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
        
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
       if (renderer == nullptr) {
           std::cout << "SDL_renderer could not be created" << std::endl;
           exit(1);
       }

    bool quit = false;
    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                quit = true;
        }
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        handleKeys(keys);

        renderEICAS();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


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


