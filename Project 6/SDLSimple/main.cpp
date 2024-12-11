/**
* Author: [Yinyi Feng]
* Assignment: Platformer
* Date due: 2023-11-23, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f
#define LEVEL1_RIGHT_EDGE 8.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "MenuScreen.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "GameLost.h"
#include "GameWon.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640 * 1.5,
          WINDOW_HEIGHT = 480 * 1.5;

constexpr float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

//constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
//               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr char V_SHADER_PATH[] = "shaders/vertex_lit.glsl",
           F_SHADER_PATH[] = "shaders/fragment_lit.glsl";

constexpr char DAMAGE_FILEPATH[] = "assets/damage.mp3";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene;
LevelA *g_level_a;
LevelB *g_level_b;
LevelC *g_level_c;
Menu *g_level_menu;
GameLost *g_level_lost;
GameWon *g_level_won;

Mix_Chunk *g_damage_sfx;

// BGM
constexpr int CD_QUAL_FREQ    = 44100,  // CD quality
          AUDIO_CHAN_AMT  = 2,      // Stereo
          AUDIO_BUFF_SIZE = 4096,
          LOOP_FOREVER   = -1;  // -1 means loop forever in Mix_PlayMusic; 0 means play once and loop zero times

// SFX
//constexpr int PLAY_ONCE   =  0,
//          NEXT_CHNL   = -1,  // next available channel
//          MUTE_VOL    =  0,
//          MILS_IN_SEC =  1000,
//          ALL_SFX_CHN = -1;

int lives = 3;
bool gameover = false;
bool flashlight_enabled = false;

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();

void print_vec3(const glm::vec3& vec)
{
    std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
}

void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("The Glitch",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // Start Audio
    Mix_OpenAudio(
        CD_QUAL_FREQ,        // the frequency to playback audio at (in Hz)
        MIX_DEFAULT_FORMAT,  // audio format
        AUDIO_CHAN_AMT,      // number of channels (1 is mono, 2 is stereo, etc).
        AUDIO_BUFF_SIZE      // audio buffer size in sample FRAMES (total samples divided by channel count)
    );

    g_damage_sfx = Mix_LoadWAV(DAMAGE_FILEPATH);

    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    Mix_VolumeChunk(g_damage_sfx, MIX_MAX_VOLUME / 2);
    
    // ————— MENU SETUP ————— //
    g_level_menu = new Menu();
    switch_to_scene(g_level_menu);
    
//    g_level_a = new LevelA();
//    switch_to_scene(g_level_a);
    
//    g_level_b = new LevelB();
//    switch_to_scene(g_level_b);
    
//    g_level_c = new LevelC();
//    switch_to_scene(g_level_c);

//    g_level_won = new GameWon();
//    switch_to_scene(g_level_won);
    
//    g_level_lost = new GameLost();
//    switch_to_scene(g_level_lost);
//
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_app_status = TERMINATED;
                        break;
                        
                    case SDLK_SPACE:
                        // ————— JUMPING ————— //
                        if (g_current_scene->get_state().player->get_collided_bottom())
                        {
                            g_current_scene->get_state().player->jump();
                            Mix_PlayChannel(-1,  g_current_scene->get_state().jump_sfx, 0);
                        }
                         break;
                        
                    case SDLK_RETURN:
                        // MENU - ENTER GAME
                        if (g_current_scene == g_level_menu){
                            g_current_scene->get_state().next_scene_id = 1;
                        }
                        break;
                    case SDLK_c:
                        // Toggle spotlight when pressing 'C'
                        flashlight_enabled = !flashlight_enabled;
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    // ————— KEY HOLD ————— //
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_A])        g_current_scene->get_state().player->move_left();
    else if (key_state[SDL_SCANCODE_D])  g_current_scene->get_state().player->move_right();
    
    if (key_state[SDL_SCANCODE_W])        g_current_scene->get_state().player->move_up();
    else if (key_state[SDL_SCANCODE_S])  g_current_scene->get_state().player->move_down();
     
    if (glm::length( g_current_scene->get_state().player->get_movement()) > 1.0f)
        g_current_scene->get_state().player->normalise_movement();
 
}

void update() {
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    // Determine the light radius based on the current level
        float lightRadius = 1.0f; // Default radius
        if (typeid(*g_current_scene) == typeid(LevelA)) {
            lightRadius = 5.0f; // Increase radius for Level A
        } else if (typeid(*g_current_scene) == typeid(LevelB)) {
            lightRadius = 3.0f; // Smaller radius for Level B
        } else if (typeid(*g_current_scene) == typeid(LevelC)) {
            lightRadius = 1.0f; // Large radius for Level C
        }

        // Update the uniform in the shader
        GLuint lightRadiusLoc = glGetUniformLocation(g_shader_program.get_program_id(), "lightRadius");
        glUniform1f(lightRadiusLoc, lightRadius);

//    if (lightRadiusLoc == -1) {
//        std::cerr << "Error: 'lightRadius' uniform not found in the shader!" << std::endl;
//    } else {
//        glUniform1f(lightRadiusLoc, lightRadius);
//        std::cout << "lightRadius set to: " << lightRadius << std::endl;
//    }


    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP) {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        if (g_current_scene) g_current_scene->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    if (g_current_scene->get_state().player) {
        glm::vec3 player_pos = g_current_scene->get_state().player->get_position();

        // Update the view matrix to center on the player
        float camera_x = -player_pos.x;
        float camera_y = -player_pos.y;
        g_view_matrix = glm::mat4(1.0f);
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(camera_x, camera_y, 0));

        // Spotlight logic: Only update the light position if the flashlight is enabled
        if (flashlight_enabled) {
            g_shader_program.set_light_position_matrix(player_pos); // Spotlight follows the player
        } else {
            g_shader_program.set_light_position_matrix(glm::vec3(-100.0f, -100.0f, 0.0f)); // Place the light offscreen
        }
    } else {
        g_view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 3.75f, 0.0f));
        g_shader_program.set_light_position_matrix(glm::vec3(-100.0f, -100.0f, 0.0f)); // Default light offscreen
    }

    if (g_current_scene->get_state().next_scene_id != -1) {
        switch (g_current_scene->get_state().next_scene_id) {
            case 1:
                g_level_a = new LevelA();
                switch_to_scene(g_level_a);
                break;
            case 2:
                g_level_b = new LevelB();
                switch_to_scene(g_level_b);
                break;
            case 3:
                g_level_c = new LevelC();
                switch_to_scene(g_level_c);
                break;
            case 4:
                g_level_won = new GameWon();
                switch_to_scene(g_level_won);
                break;
            case 5:
                g_level_lost = new GameLost();
                switch_to_scene(g_level_lost);
                break;
            default:
                break;
        }
    }

    // ————— COLLISION LOGIC ————— //
    if (g_current_scene && g_current_scene->get_state().player) {
        // checks if we are in a level
        if (typeid(*g_current_scene) == typeid(LevelA) ||
            typeid(*g_current_scene) == typeid(LevelB) ||
            typeid(*g_current_scene) == typeid(LevelC)) {
            
            // iterates through 3 enemies
            for (int i = 0; i < 3; i++) {
                Entity& enemy = g_current_scene->get_state().enemies[i];
                
                // checks collisions with active enemies
                if (enemy.get_is_active() && g_current_scene->get_state().player->check_collision(&enemy)) {
                    std::cout << "Collision detected! Lives before decrement: " << lives << std::endl;
                    enemy.deactivate();
                    Mix_PlayChannel(-1, g_damage_sfx, 0);   // Play damage sfx when hit
                    lives--;
                    std::cout << "Lives after decrement: " << lives << std::endl;

                    if (lives <= 0) {
                        gameover = true;
                        g_level_lost = new GameLost();
                        switch_to_scene(g_level_lost); // Render game lost screen when all lives = 0
                        return;
                    }
                }

            }
        }
    }

//    print_vec3(g_current_scene->get_state().player->get_position());
}


void render()
{
    // Set the view matrix for the main game rendering
    g_shader_program.set_view_matrix(g_view_matrix);
    
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Render the current scene (e.g., player, enemies, map)
    g_current_scene->render(&g_shader_program);
    
    // Reset view matrix for lives text rendering
    glm::mat4 temp_view_matrix = glm::mat4(1.0f);
    g_shader_program.set_view_matrix(temp_view_matrix);

    // Render the Lives text at the top-left corner
    Utility::draw_text(
        &g_shader_program,
        Utility::load_texture("assets/font1.png"),
        "Lives: " + std::to_string(lives),
        0.3f,                        // Text size
        0.0f,                        // Spacing between characters
        glm::vec3(2.5f, 3.5f, 0.0f) // coordinates for top-left corner
    );

    // Restore the main view matrix for game rendering
    g_shader_program.set_view_matrix(g_view_matrix);

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
    SDL_Quit();
    
    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_level_a;
    delete g_level_b;
    delete g_level_c;
    delete g_level_menu;
    delete g_level_lost;
    delete g_level_won;
 
}

// ————— GAME LOOP ————— //
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
