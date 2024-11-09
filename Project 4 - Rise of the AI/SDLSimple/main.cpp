#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 3
#define LEVEL1_WIDTH 25
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
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"

// ————— GAME STATE ————— //
struct GameState
{
    Entity *player;
    Entity *enemies;
    
    Map *map;
};

enum AppStatus { RUNNING, TERMINATED };

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

constexpr char GAME_WINDOW_NAME[] = "The Rise of AI!";

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char ASH_SPRITE_FILEPATH[]    = "ash_ketchup.png",
               PIPLUP_SPRITE_FILEPATH[] = "piplup.png",
               MAP_TILESET_FILEPATH[]   = "tilesheet.png",
               FONTSHEET_FILEPATH[]     = "font1.png";

constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL  = 0;
constexpr GLint TEXTURE_BORDER   = 0;

constexpr int FONTBANK_SIZE = 16;

unsigned int LEVEL_1_DATA[] =
{
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  9, 11,  0,  0,  0,  2,  3,  3,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  9, 11,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     5,  6,  6,  6,  6,  6,  6,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  9, 10, 10, 11,
    15, 16, 16, 16, 16, 16, 16, 17,  0,  0,  0,  2,  3,  3,  3,  3,  4,  0,  0,  0,  0,  0,  0,  0,  0
};

// ————— VARIABLES ————— //
GameState g_game_state;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f,
      g_accumulator    = 0.0f;

GLuint g_font_texture_id;

bool game_win, game_lose;

void draw_text(ShaderProgram *shader_program, GLuint font_texture_id, std::string text,
               float font_size, float spacing, glm::vec3 position);

void initialise();
void process_input();
void update();
void render();
void shutdown();

// ————— GENERAL FUNCTIONS ————— //
void draw_text(ShaderProgram *shader_program, GLuint font_texture_id, std::string text,
               float font_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for
    // each character. Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their
        //    position relative to the whole sentence)
        int spritesheet_index = (int) text[i];  // ascii value of character
        float offset = (font_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float) (spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float) (spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
        });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
        });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    shader_program->set_model_matrix(model_matrix);
    glUseProgram(shader_program->get_program_id());

    glVertexAttribPointer(shader_program->get_position_attribute(), 2, GL_FLOAT, false, 0,
                          vertices.data());
    glEnableVertexAttribArray(shader_program->get_position_attribute());

    glVertexAttribPointer(shader_program->get_tex_coordinate_attribute(), 2, GL_FLOAT,
                          false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(shader_program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));

    glDisableVertexAttribArray(shader_program->get_position_attribute());
    glDisableVertexAttribArray(shader_program->get_tex_coordinate_attribute());
}

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
    // ————— GENERAL ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow(GAME_WINDOW_NAME,
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        LOG("ERROR: Could not create OpenGL context.\n");
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— VIDEO SETUP ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    g_font_texture_id = load_texture(FONTSHEET_FILEPATH);
    
    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH);
    g_game_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 22, 1);
    
    // ————— GEORGE SET-UP ————— //

    GLuint player_texture_id = load_texture(ASH_SPRITE_FILEPATH);

    int player_walking_animation[4][3] =
    {
        { 3, 7, 11 },  // for George to move to the left,
        { 1, 5, 9  }, // for George to move to the right,
        { 0, 4, 8  }, // for George to move upwards,
        { 2, 6, 10 }   // for George to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f,-4.905f, 0.0f);

    g_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        4.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        3,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        3,                         // animation row amount
        0.7f,                      // width
        0.7f,                       // height
        PLAYER
    );

    g_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));

    // Jumping
//    g_game_state.player->set_jumping_power(4.0f);
    
    // ----------- ENEMY Piplup ----------- //
    GLuint enemy_texture_id = load_texture(PIPLUP_SPRITE_FILEPATH);
    
    int enemy_walking_animation[4][3] =
    {
        { 3, 7, 11 },  // for George to move to the left,
        { 1, 5, 9  }, // for George to move to the right,
        { 0, 4, 8  }, // for George to move upwards,
        { 2, 6, 10 }   // for George to move downwards
    };
    
    g_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        g_game_state.enemies[i] = Entity(enemy_texture_id, 0.5f, acceleration, 0.1f, enemy_walking_animation, 0.0f, 3, 0, 4, 3, 0.25f, 0.7f, ENEMY);
//        g_game_state.enemies[i].set_position(glm::vec3(i + 2.0f, -2.1f, 0.0f));
        g_game_state.enemies[i].set_movement(glm::vec3(0.0f));
        g_game_state.enemies[i].face_down();
    }
    
    g_game_state.enemies[0].set_position(glm::vec3(11.0f, -2.15f, 0.0f));
    g_game_state.enemies[1].set_position(glm::vec3(17.5f, 0.85f, 0.0f));
//    g_game_state.enemies[2].set_position(glm::vec3(22.6f, -2.1f, 0.0f));
    
    g_game_state.enemies[2].set_position(glm::vec3(2.0f, 5.0f, 0.0f));
    
    g_game_state.enemies[1].set_ai_type(ROTATOR);
    
    g_game_state.enemies[0].set_ai_type(GUARD);
    g_game_state.enemies[0].set_ai_state(IDLE);
    
    g_game_state.enemies[2].set_ai_type(JUMPER);
    g_game_state.enemies[2].set_ai_state(JUMPING);
    g_game_state.enemies[2].set_jumping_power(2.0f);
    
    
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void print_vec3(const glm::vec3& vec)
{
    std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
}


void process_input()
{
    g_game_state.player->set_movement(glm::vec3(0.0f));
//    g_game_state.enemies->set_movement(glm::vec3(0.0f));
    
    g_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    g_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    g_game_state.enemies[2].set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
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
                        // Jump
                        if (g_game_state.player->get_collided_bottom())
                        {
                            g_game_state.player->jump();
//                            g_game_state.enemies[2].jump();
                        }
                        if (g_game_state.enemies[2].get_collided_bottom())
                        {
                            g_game_state.enemies[2].jump();
                        }
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_A])       g_game_state.player->move_left();
    else if (key_state[SDL_SCANCODE_D]) g_game_state.player->move_right();
         
    if (glm::length(g_game_state.player->get_movement()) > 1.0f)
        g_game_state.player->normalise_movement();
}

void update()
{
    if (game_win || g_game_state.player->get_is_gameover()) return; // stop updating when game end has been reached
    
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP) {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        // Update player
        g_game_state.player->update(FIXED_TIMESTEP, g_game_state.player, NULL, 0, g_game_state.map);

        // Check for collisions between player and enemies
        g_game_state.player->check_collision_y(g_game_state.enemies, ENEMY_COUNT);

        // Update active enemies
        int active_enemies = 0;
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (g_game_state.enemies[i].get_is_active()) {
                  g_game_state.enemies[i].update(FIXED_TIMESTEP, g_game_state.player, NULL, 0, g_game_state.map);
//                g_game_state.enemies[i].update(FIXED_TIMESTEP, g_game_state.player, g_game_state.player, 1, g_game_state.map);
                active_enemies++;
            }
        }

        // Check if all enemies are defeated
        if (active_enemies == 0) {
            LOG("All enemies defeated! You win!");
            game_win = true;
//            g_app_status = TERMINATED;  // End the game
        }

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_game_state.player->get_position().x, 0.0f, 0.0f));
    
    print_vec3(g_game_state.player->get_position());
}



void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    g_game_state.map->render(&g_shader_program);
    
    g_game_state.player->render(&g_shader_program);
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (g_game_state.enemies[i].get_is_active()) {
            g_game_state.enemies[i].render(&g_shader_program);
        }
    }
    
    glm::vec3 player_position = g_game_state.player->get_position();
    glm::vec3 text_position = player_position + glm::vec3(-1.8f, 2.0f, 0.0f);

    
    if (game_win){
        draw_text(&g_shader_program, g_font_texture_id, "You Win!", 0.5f, 0.05f, text_position);
    }else if (g_game_state.player->get_is_gameover()){
        draw_text(&g_shader_program, g_font_texture_id, "You Lose!", 0.5f, 0.05f, text_position);
    }
    

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    delete [] g_game_state.enemies;
    delete    g_game_state.player;
    delete    g_game_state.map;
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
    
    if (g_app_status == TERMINATED) shutdown();
    return 0;
}
