/**
* Author: Yinyi Feng
* Assignment: Simple 2D Scene
* Date due: 2024-09-28, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

// reference: tumblr.com/be-my-peter-panx/160091184137/chase-your-dreams-like-peter-chased-his-shadow?source=share

enum AppStatus { RUNNING, TERMINATED };

constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 400 * 2;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X      = 0,
              VIEWPORT_Y      = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr GLint NUMBER_OF_TEXTURES = 1, // to be generated, that is
                LEVEL_OF_DETAIL    = 0, // mipmap reduction image level
                TEXTURE_BORDER     = 0; // this value MUST be zero

constexpr char TINKERBELL_FILEPATH[]    = "tinkerbell.png",
               PETERPAN_FILEPATH[]      = "peterpan.png",
               WENDY_FILEPATH[]         = "wendy.png",
               BACKGROUND_FILEPATH[]    = "background.png";

constexpr glm::vec3 INIT_TINKEBELL_SCALE       = glm::vec3(1.0f, 1.0f, 0.0f),
                    INIT_PETERPAN_SCALE        = glm::vec3(7.0f, 7.0f, 0.0f),
                    INIT_WENDY_SCALE           = glm::vec3(2.0f, 2.0f, 0.0f),
                    INIT_BACKGROUND_SCALE      = glm::vec3(7.6f * 1.33, 7.6f, 0.0f),
                    INIT_POS_TINKERBELL        = glm::vec3(3.5f, 0.0f, 0.0f),
                    INIT_POS_PETERPAN          = glm::vec3(0.0f, -1.0f, 0.0f),
                    INIT_POS_WENDY             = glm::vec3(0.0f, 2.5f, 0.0f);

constexpr float ROT_INCREMENT = 1.0f;

constexpr float BASE_SCALE     = 1.0f,
                MAX_AMPLITUDE  = 0.1f,
                PULSE_SPEED    = 3.0f;

float g_pulse_time = 0.0f;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,
          g_tinkerbell_matrix,
          g_peterpan_matrix,
          g_wendy_matrix,
          g_background_matrix,
          g_projection_matrix;

float g_previous_ticks = 0.0f;

glm::vec3 g_rotation_peterpan = glm::vec3(0.0f, 0.0f, 0.0f);

GLuint g_tinkerbell_texture_id,
       g_peterpan_texture_id,
       g_wendy_texture_id,
       g_background_texture_id;


GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}


void initialise()
{
    // Initialise video and joystick subsystems
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("The Shadow Chase",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        SDL_Quit();
        exit(1);
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_tinkerbell_matrix   = glm::mat4(1.0f);
    g_peterpan_matrix     = glm::mat4(1.0f);
    g_wendy_matrix        = glm::mat4(1.0f);
    g_background_matrix   = glm::mat4(1.0f);
    g_view_matrix         = glm::mat4(1.0f);
    g_projection_matrix   = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_tinkerbell_texture_id   = load_texture(TINKERBELL_FILEPATH);
    g_peterpan_texture_id = load_texture(PETERPAN_FILEPATH);
    g_wendy_texture_id = load_texture(WENDY_FILEPATH);
    g_background_texture_id = load_texture(BACKGROUND_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
    }
}


void update()
{
    /* Delta time calculations */
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    // Peter Pan Rotation
    g_rotation_peterpan.z += -5 * ROT_INCREMENT * delta_time;

    // Circular motion for Tinkerbell
    static float angle = 0.0f; // angle for circular motion
    float radius = 0.8f;       // radius of the circular path

    // Update the angle
    angle += ROT_INCREMENT * delta_time * 1.5; // * 1.5 increases circulation speed
    
    // Pulse
    g_pulse_time += PULSE_SPEED * delta_time;
    float scale_factor = BASE_SCALE + MAX_AMPLITUDE * glm::sin(g_pulse_time);

    // Calculate Tinker Bells position using sin / cos and the angle
    float tinkerbell_x = radius * cos(angle);
    float tinkerbell_y = radius * sin(angle);
    
    // Variables for Wendy's oscillation
    static float wendy_angle = 0.0f;   // angle of which wendy oscillates
    float wendy_speed = 1.5f;          // speed of the horizontal movement
    float wendy_amplitude = 0.5f;      // amplitude of the vertical oscillation
    static float wendy_x = -5.0f;      // start position

    // Update Wendy's horizontal position
    wendy_x += wendy_speed * delta_time;

    // Reset position if Wendy moves off the screen
    if (wendy_x > 6.0f) {
        wendy_x = -6.0f;
    }

    // Calculate Wendy's vertical position based on sine wave
    float wendy_y = wendy_amplitude * sin(wendy_angle);
    wendy_angle += delta_time * 3.0f;

    /* Model matrix reset */
    g_tinkerbell_matrix = glm::mat4(1.0f);
    g_peterpan_matrix   = glm::mat4(1.0f);
    g_wendy_matrix      = glm::mat4(1.0f);
    g_background_matrix = glm::mat4(1.0f);

    /* Transformations */
    g_background_matrix = glm::scale(g_background_matrix, INIT_BACKGROUND_SCALE);
    
    g_tinkerbell_matrix = glm::translate(g_tinkerbell_matrix, INIT_POS_TINKERBELL +                                                  glm::vec3(tinkerbell_x, tinkerbell_y, 0.0f));
    g_tinkerbell_matrix = glm::scale(g_tinkerbell_matrix,
                                     glm::vec3(scale_factor, scale_factor, 1.0f));

    g_peterpan_matrix = glm::translate(g_peterpan_matrix, INIT_POS_PETERPAN);
    g_peterpan_matrix = glm::rotate(g_peterpan_matrix, g_rotation_peterpan.z,
                                    glm::vec3(0.0f, 0.0f, 1.0f));
    g_peterpan_matrix = glm::scale(g_peterpan_matrix, INIT_PETERPAN_SCALE);


    g_wendy_matrix = glm::translate(g_wendy_matrix, INIT_POS_WENDY +
                                    glm::vec3(wendy_x, wendy_y, 0.0f));
    g_wendy_matrix = glm::scale(g_wendy_matrix, INIT_WENDY_SCALE);
}


void draw_object(glm::mat4 &object_g_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_g_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so use 6, not 3
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
                          0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
                          false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_background_matrix, g_background_texture_id);
    draw_object(g_tinkerbell_matrix, g_tinkerbell_texture_id);
    draw_object(g_wendy_matrix, g_wendy_texture_id);
    draw_object(g_peterpan_matrix, g_peterpan_texture_id);

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


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
