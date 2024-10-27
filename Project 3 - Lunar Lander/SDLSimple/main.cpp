/**
* Author: Yinyi Feng
* Assignment: Lunar Lander
* Date due: 2024-10-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 5

#ifdef _WINDOWS
    #include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "Entity.h"
#include <vector>
#include <ctime>
#include "cmath"
#include "Map.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED     = 0.1765625f,
                BG_GREEN   = 0.17265625f,
                BG_BLUE    = 0.1609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr glm::vec3 PLAYER_IDLE_SCALE = glm::vec3(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 INIT_BACKGROUND_SCALE = glm::vec3(100.0f, 7.6f, 0.0f);
constexpr glm::vec3 PLAYER_IDLE_LOCATION = glm::vec3(0.0f, 0.0f, 0.0f);

constexpr float ACCELERATION = 2.0f;

// ————— STRUCTS AND ENUMS —————//
enum AppStatus  { RUNNING, TERMINATED };
enum FilterType { NEAREST, LINEAR     };

struct GameState { Entity* player; Map* map; };

// ————— VARIABLES ————— //
GameState g_game_state;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix, g_background_matrix;

GLuint g_background_texture_id;

constexpr char MAP_TILESET_FILEPATH[] = "tileset2.png";

constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL  = 0;
constexpr GLint TEXTURE_BORDER   = 0;

unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 1, 2, 2, 2, 0, 0,
    1, 2, 0, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1,
    2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

float g_previous_ticks = 0.0f;

void initialise();
void process_input();
void update();
void render();
void shutdown();

// ———— GENERAL FUNCTIONS ———— //
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    GLuint texture_id;
    glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_image_free(image);
    
    return texture_id;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Hello, Ironman!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix       = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    g_background_matrix = glm::mat4(1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // MAP SETUP //
    GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH);
    g_game_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 5, 1);
    
    // ————— IRONMAN ————— //
    std::vector<GLuint> ironman_textures_ids = {
//        load_texture("assets/idle.png", NEAREST),   // IDLE spritesheet
//        load_texture("assets/attack.png", NEAREST)  // ATTACK spritesheet

        load_texture("ironman_idle.png"),   // IDLE spritesheet
        load_texture("ironman_flying.png")  // IDLE spritesheet
    };

    std::vector<std::vector<int>> ironman_animations = {
        {0},       // IDLE animation frames
        {0}  // ATTACK animation frames
    };

    g_game_state.player = new Entity(
        ironman_textures_ids,  // a list of texture IDs
        1.0f,                // translation speed; irrelevant in this problem
        ironman_animations,    // list of animation frames for each type of animation
        0.0f,                // animation time
        1,                   // number of frames for idle animation
        0,                   // current frame index
        1,                   // current animation col amount
        1,                   // current animation row amount
        IDLE                 // current animation
    );

    g_game_state.player->set_position(PLAYER_IDLE_LOCATION);
    g_game_state.player->set_scale(PLAYER_IDLE_SCALE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}



void process_input()
{
    g_game_state.player->set_animation_state(IDLE);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        g_app_status = TERMINATED;
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    // Handle rotation
    if (key_state[SDL_SCANCODE_D]) {
        g_game_state.player->set_rotation(90.0f); // Rotate to 90 degrees
    }
    else if (key_state[SDL_SCANCODE_A]) {
        g_game_state.player->set_rotation(-90.0f); // Rotate to -90 degrees
    }
    else if (key_state[SDL_SCANCODE_W]) {
        g_game_state.player->set_rotation(0.0f); // Rotate back to 0 degrees
    }

    // Movement vector
    float angle = g_game_state.player->get_rotation();

    // Handle acceleration
    if (key_state[SDL_SCANCODE_SPACE]) {
        g_game_state.player->set_animation_state(ATTACK);
    }

}



void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    g_background_matrix = glm::mat4(1.0f);
    g_background_matrix = glm::scale(g_background_matrix, INIT_BACKGROUND_SCALE);
    g_game_state.player->update(delta_time, g_game_state.player, NULL, 0,
                               g_game_state.map);
    
    g_view_matrix = glm::mat4(1.0f);
    
    // Camera Follows the player
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_game_state.player->get_position().x, 0.0f, 0.0f));
}

void draw_object(glm::mat4 &object_g_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_g_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so use 6, not 3
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);


    g_game_state.player->render(&g_shader_program);
    g_game_state.map->render(&g_shader_program);
    SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
    SDL_Quit();
    delete   g_game_state.player;
}


int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
