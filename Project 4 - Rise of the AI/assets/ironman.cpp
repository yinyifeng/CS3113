//#define LOG(argument) std::cout << argument << '\n'
//#define STB_IMAGE_IMPLEMENTATION
//#define GL_SILENCE_DEPRECATION
//#define GL_GLEXT_PROTOTYPES 1
//
//#ifdef _WINDOWS
//    #include <GL/glew.h>
//#endif
//
//#include <SDL.h>
//#include <SDL_opengl.h>
//#include "glm/mat4x4.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//#include "ShaderProgram.h"
//#include "stb_image.h"
//#include "Entity.h"
//#include <vector>
//#include <ctime>
//#include "cmath"
//
//// ————— CONSTANTS ————— //
//constexpr int WINDOW_WIDTH  = 640 * 2,
//              WINDOW_HEIGHT = 480 * 2;
//
//constexpr float BG_RED     = 0.1765625f,
//                BG_GREEN   = 0.17265625f,
//                BG_BLUE    = 0.1609375f,
//                BG_OPACITY = 1.0f;
//
//constexpr int VIEWPORT_X = 0,
//              VIEWPORT_Y = 0,
//              VIEWPORT_WIDTH  = WINDOW_WIDTH,
//              VIEWPORT_HEIGHT = WINDOW_HEIGHT;
//
//constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
//               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
//
//constexpr float MILLISECONDS_IN_SECOND = 1000.0;
//
//constexpr glm::vec3 WITCH_IDLE_SCALE = glm::vec3(1.0f, 1.0f, 0.0f);
//constexpr glm::vec3 INIT_BACKGROUND_SCALE = glm::vec3(100.0f, 7.6f, 0.0f);
////constexpr glm::vec3 WITCH_ATTACK_SCALE = glm::vec3(1.0f, 1.0f, 0.0f);
//constexpr glm::vec3 WITCH_IDLE_LOCATION = glm::vec3(0.0f, 0.0f, 0.0f);
//
//constexpr float ACCELERATION = 2.0f;
//
//constexpr GLint NUMBER_OF_TEXTURES = 1,
//                LEVEL_OF_DETAIL    = 0,
//                TEXTURE_BORDER     = 0;
//
//// ————— STRUCTS AND ENUMS —————//
//enum AppStatus  { RUNNING, TERMINATED };
//enum FilterType { NEAREST, LINEAR     };
//
//struct GameState { Entity* witch; };
//
//// ————— VARIABLES ————— //
//GameState g_game_state;
//
//SDL_Window* g_display_window;
//AppStatus g_app_status = RUNNING;
//
//ShaderProgram g_shader_program;
//glm::mat4 g_view_matrix, g_projection_matrix, g_background_matrix;
//
//GLuint g_background_texture_id;
//
//constexpr char BACKGROUND_FILEPATH[] = "avengers_tower.png";
//
//float g_previous_ticks = 0.0f;
//
//void initialise();
//void process_input();
//void update();
//void render();
//void shutdown();
//
//GLuint load_texture(const char* filepath);
//
//// ———— GENERAL FUNCTIONS ———— //
//GLuint load_texture(const char* filepath, FilterType filterType)
//{
//    int width, height, number_of_components;
//    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components,
//                                     STBI_rgb_alpha);
//
//    if (image == NULL)
//    {
//        LOG("Unable to load image. Make sure the path is correct.");
//        assert(false);
//    }
//
//    GLuint textureID;
//    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
//                 GL_RGBA, GL_UNSIGNED_BYTE, image);
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
//                    filterType == NEAREST ? GL_NEAREST : GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
//                    filterType == NEAREST ? GL_NEAREST : GL_LINEAR);
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//    stbi_image_free(image);
//
//    return textureID;
//}
//
//void initialise()
//{
//    SDL_Init(SDL_INIT_VIDEO);
//    g_display_window = SDL_CreateWindow("Hello, Witch!",
//                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
//                                      WINDOW_WIDTH, WINDOW_HEIGHT,
//                                      SDL_WINDOW_OPENGL);
//
//    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
//    SDL_GL_MakeCurrent(g_display_window, context);
//
//    if (g_display_window == nullptr)
//    {
//        std::cerr << "Error: SDL window could not be created.\n";
//        shutdown();
//    }
//
//#ifdef _WINDOWS
//    glewInit();
//#endif
//
//    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
//
//    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
//
//    g_view_matrix       = glm::mat4(1.0f);
//    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
//    g_background_matrix = glm::mat4(1.0f);
//
//    g_shader_program.set_projection_matrix(g_projection_matrix);
//    g_shader_program.set_view_matrix(g_view_matrix);
//
//    glUseProgram(g_shader_program.get_program_id());
//
//    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
//
//    // ————— WITCH ————— //
//    std::vector<GLuint> witch_textures_ids = {
////        load_texture("assets/idle.png", NEAREST),   // IDLE spritesheet
////        load_texture("assets/attack.png", NEAREST)  // ATTACK spritesheet
//        
//        load_texture("ironman_idle.png", NEAREST),   // IDLE spritesheet
//        load_texture("ironman_flying.png", NEAREST),   // IDLE spritesheet
//    };
//    
//    g_background_texture_id   = load_texture(BACKGROUND_FILEPATH, NEAREST);
//
//    std::vector<std::vector<int>> witch_animations = {
//        {0},       // IDLE animation frames
//        {0}  // ATTACK animation frames
//    };
//
//    g_game_state.witch = new Entity(
//        witch_textures_ids,  // a list of texture IDs
//        1.0f,                // translation speed; irrelevant in this problem
//        witch_animations,    // list of animation frames for each type of animation
//        0.0f,                // animation time
//        1,                   // number of frames for idle animation
//        0,                   // current frame index
//        1,                   // current animation col amount
//        1,                   // current animation row amount
//        IDLE                 // current animation
//    );
//
//    g_game_state.witch->set_position(WITCH_IDLE_LOCATION);
//    g_game_state.witch->set_scale(WITCH_IDLE_SCALE);
//
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    
//}
//
//
//
//void process_input()
//{
//    g_game_state.witch->set_animation_state(IDLE);
//
//    SDL_Event event;
//    while (SDL_PollEvent(&event))
//    {
//        switch (event.type) {
//            case SDL_QUIT:
//            case SDL_WINDOWEVENT_CLOSE:
//                g_app_status = TERMINATED;
//                break;
//
//            case SDL_KEYDOWN:
//                switch (event.key.keysym.sym)
//                {
//                    case SDLK_q:
//                        g_app_status = TERMINATED;
//                        break;
//                   
//                    default:
//                        break;
//                }
//                break;
//
//            default:
//                break;
//        }
//    }
//
//    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
//    glm::vec3 acceleration(0.0f, 0.0f, 0.0f);
//
//    // Handle rotation
//    if (key_state[SDL_SCANCODE_D]) {
//        g_game_state.witch->set_rotation(90.0f); // Rotate to 90 degrees
////        acceleration.x = ACCELERATION;
//    }
//    else if (key_state[SDL_SCANCODE_A]) {
//        g_game_state.witch->set_rotation(-90.0f); // Rotate to -90 degrees
////        acceleration.x = -ACCELERATION;
//    }
//    else if (key_state[SDL_SCANCODE_W]) {
//        g_game_state.witch->set_rotation(0.0f); // Rotate back to 0 degrees
////        acceleration.y = ACCELERATION;
//    }
//
//    // Movement vector
//    glm::vec3 movement(0.0f, 0.0f, 0.0f); // Initialize movement vector
//    float angle = g_game_state.witch->get_rotation();
//
//    // Handle acceleration
//    if (key_state[SDL_SCANCODE_SPACE]) {
////        if (angle == 0.0f) {
////            acceleration.y = 2.0f; // Move up
////        }
//
//        // Apply movement to the witch
////        g_game_state.witch->set_movement(movement);
//        g_game_state.witch->set_animation_state(ATTACK);
//    }
////    g_game_state.witch->set_movement(acceleration);
//}
//
//
//
//void update()
//{
//    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
//    float delta_time = ticks - g_previous_ticks;
//    g_previous_ticks = ticks;
//
//    g_background_matrix = glm::mat4(1.0f);
//    g_background_matrix = glm::scale(g_background_matrix, INIT_BACKGROUND_SCALE);
//    g_game_state.witch->update(delta_time);
//}
//
//void draw_object(glm::mat4 &object_g_model_matrix, GLuint &object_texture_id)
//{
//    g_shader_program.set_model_matrix(object_g_model_matrix);
//    glBindTexture(GL_TEXTURE_2D, object_texture_id);
//    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so use 6, not 3
//}
//
//void render()
//{
//    glClear(GL_COLOR_BUFFER_BIT);
//    
//    
//    g_game_state.witch->render(&g_shader_program);
//    draw_object(g_background_matrix, g_background_texture_id);
//    SDL_GL_SwapWindow(g_display_window);
//}
//
//
//void shutdown()
//{
//    SDL_Quit();
//    delete   g_game_state.witch;
//}
//
//
//int main(int argc, char* argv[])
//{
//    initialise();
//
//    while (g_app_status == RUNNING)
//    {
//        process_input();
//        update();
//        render();
//    }
//
//    shutdown();
//    return 0;
//}
