/**
* Author: Yinyi Feng
* Assignment: Pong Clone
* Date due: 2024-10-12, 11:59pm
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

constexpr char TENNISBALL_FILEPATH[]    = "tennisball.png",
               RACKET_FILEPATH[]        = "racket.png",
               BACKGROUND_FILEPATH[]    = "tenniscourt.jpg",
               PLAYER1_WIN_FILEPATH[]   = "player1win.png",
               PLAYER2_WIN_FILEPATH[]   = "player2win.png";

constexpr glm::vec3 INIT_TENNISBALL_SCALE      = glm::vec3(0.4f, 0.4f, 0.0f),
                    INIT_RACKET_SCALE          = glm::vec3(1.0f, 2.0f, 0.0f),
                    INIT_BACKGROUND_SCALE      = glm::vec3(7.6f * 1.33, 7.6f, 0.0f),
                    INIT_POS_TENNISBALL        = glm::vec3(3.5f, 0.0f, 0.0f),
                    INIT_POS_RACKET1           = glm::vec3(-3.8f, 0.0f, 0.0f),
                    INIT_POS_RACKET2           = glm::vec3(3.8f, 0.0f, 0.0f),
                    INIT_FINAL_SCREEN_SCALE    = glm::vec3(4.0f, 4.0f, 1.0f);


float g_pulse_time = 0.0f;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,
          g_tennisball_matrix,
          g_racket1_matrix,
          g_racket2_matrix,
          g_background_matrix,
          g_projection_matrix,
          g_player1_win_matrix,
          g_player2_win_matrix,
          g_game_over_matrix;

float g_previous_ticks = 0.0f;

constexpr float RACKET_SPEED = 3.0f,
                TENNISBALL_SPEED = 2.0f;

bool g_single_player     = false, // track to see if we are in single/multi player
     g_player1_win       = false, // tracks if player 1 wins
     g_player2_win       = false, // tracks if player 2 wins
     g_show_game_over    = false; // tracks if game over screen is shown

int g_number_of_balls = 1;        // ball counter; default = 1


// texture ids
GLuint g_tennisball_texture_id,
       g_racket_texture_id,
       g_background_texture_id,
       g_player1_win_texture_id,
       g_player2_win_texture_id,
       g_final_texture_id;


// position and movement vectors for racket1
glm::vec3 g_racket1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_racket1_movement = glm::vec3(0.0f, 0.0f, 0.0f);

// position and movement vectors for racket2
glm::vec3 g_racket2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_racket2_movement = glm::vec3(0.0f, 0.0f, 0.0f);


// array to store positions of tennis balls
glm::vec3 g_tennisball_positions[3] = {
    glm::vec3(0.0f, 0.0f, 0.0f), // Tennis ball 1
    glm::vec3(0.0f, 0.0f, 0.0f), // Tennis ball 2
    glm::vec3(0.0f, 0.0f, 0.0f)  // Tennis ball 3
};

// array to store velocities of tennis balls
glm::vec3 g_tennisball_velocities[3] = {
    glm::vec3(-1.0f, 0.0f, 0.0f), // Initial velocity for tennis ball 1
    glm::vec3(1.0f, 0.0f, 0.0f),  // Initial velocity for tennis ball 2
    glm::vec3(-1.0f, 0.0f, 0.0f)  // Initial velocity for tennis ball 3
};


void initialise();
void process_input();
void update();
void render();
void shutdown();

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

    g_display_window = SDL_CreateWindow("The Tennis Court",
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
    
    // initialize background transformation matrix
    g_background_matrix   = glm::mat4(1.0f);
    
    // initialize racket 1 transformation matrix
    g_racket1_matrix     = glm::mat4(1.0f);
    g_racket1_matrix     = glm::translate(g_racket1_matrix, glm::vec3(1.0f, 1.0f, 0.0f)); // translate racket 1 to initial position
    g_racket1_position   += g_racket1_movement;                                           // update its position according to movement vector

    // initialize racket 2 transformation matrix
    g_racket2_matrix     = glm::mat4(1.0f);
    g_racket2_matrix     = glm::translate(g_racket2_matrix, glm::vec3(1.0f, 1.0f, 0.0f)); // translate racket w to initial position
    g_racket2_position   += g_racket2_movement;                                           // update its position according to movement vector
    
    // initialize win screen transformation matrix
    g_player1_win_matrix = glm::mat4(1.0f);
    g_player1_win_matrix = glm::scale(g_player1_win_matrix, INIT_FINAL_SCREEN_SCALE);
    
    g_player2_win_matrix = glm::mat4(1.0f);
    g_player2_win_matrix = glm::scale(g_player2_win_matrix, INIT_FINAL_SCREEN_SCALE);
    
    g_view_matrix         = glm::mat4(1.0f);
    g_projection_matrix   = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // load textures
    g_tennisball_texture_id   = load_texture(TENNISBALL_FILEPATH);
    g_racket_texture_id       = load_texture(RACKET_FILEPATH);
    g_background_texture_id   = load_texture(BACKGROUND_FILEPATH);
    g_player1_win_texture_id  = load_texture(PLAYER1_WIN_FILEPATH);
    g_player2_win_texture_id  = load_texture(PLAYER2_WIN_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    // reset movement vectors to 0 when theres no input
    g_racket1_movement = glm::vec3(0.0f);
    g_racket2_movement = glm::vec3(0.0f);
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
        

        if (event.type == SDL_KEYDOWN) {
            // toggle single/multi player mode when 't' is pressed
            if (event.key.keysym.sym == SDLK_t) {
                g_single_player = !g_single_player;
            }
            
            // terminate game when 'q' is pressed
            if (event.key.keysym.sym == SDLK_q) {
                g_app_status = TERMINATED;
            }
        }
        
        // update the number of balls based on key press
        if (event.key.keysym.sym == SDLK_1) {
            g_number_of_balls = 1;
        } else if (event.key.keysym.sym == SDLK_2) {
            g_number_of_balls = 2;
        } else if (event.key.keysym.sym == SDLK_3) {
            g_number_of_balls = 3;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    
    // move racket1 upwards when 'w' is pressed
    if (key_state[SDL_SCANCODE_W]){
        g_racket1_movement.y = 1.0f;
    }
    // move racket1 downwards when 's' is pressed
    else if (key_state[SDL_SCANCODE_S]){
        g_racket1_movement.y = -1.0f;
    }
    
    
    // checks if we're in multiplayer mode
    if (not g_single_player){
        // move racket2 upwards when up arrow is pressed
        if (key_state[SDL_SCANCODE_UP]){
            g_racket2_movement.y = 1.0f;
        }
        // move racket 2 downwards when down arrow is pressed
        else if (key_state[SDL_SCANCODE_DOWN]){
            g_racket2_movement.y = -1.0f;
        }
    }
    
}


void update()
{
    /* Delta time calculations */
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    // update racket1 position
    g_racket1_position += g_racket1_movement * RACKET_SPEED * delta_time;

    
    // checks if we are in single player mode
    if (g_single_player) {
        static float automatic_movement = 0.0f;                 // track automatic movement over time
        float automatic_speed = 2.0f;                           // racket2 automatic speed
        automatic_movement += automatic_speed * delta_time;     // update racket2 automatic movement
        g_racket2_position.y = 2.0f * sin(automatic_movement);  // update racket2 position in up and down motion based on sin function
    } else {
        // multiplayer mode
        g_racket2_position += g_racket2_movement * RACKET_SPEED * delta_time;   // update racket2 position
    }
    
    
    // makes sure rackets dont go off the top or the bottom of the screen
    float racket1_half_height = (INIT_RACKET_SCALE.y / 2.0f);                   // calculate half of racket's height
        if (g_racket1_position.y + racket1_half_height > 3.75f) {               // checks if racket exceeds top of the screen (3.75 is half of the background height)
            g_racket1_position.y = 3.75f - racket1_half_height;                 // set racket position to the upper boundary minus half of the racket height
        } else if (g_racket1_position.y - racket1_half_height < -3.75f) {       // checks if racket exceeds bottom of the screen
            g_racket1_position.y = -3.75f + racket1_half_height;                // set racket position to the lower boundary plus half of the racket height
        }
    
    float racket2_half_height = (INIT_RACKET_SCALE.y / 2.0f);
        if (g_racket2_position.y + racket2_half_height > 3.75f) {
            g_racket2_position.y = 3.75f - racket2_half_height;
        } else if (g_racket2_position.y - racket2_half_height < -3.75f) {
            g_racket2_position.y = -3.75f + racket2_half_height;
        }

    
    // update positions of each tennis ball
    for (int i = 0; i < g_number_of_balls; ++i) {
        g_tennisball_positions[i].x += g_tennisball_velocities[i].x * TENNISBALL_SPEED * delta_time;  // update tennis ball's x position
        g_tennisball_positions[i].y += g_tennisball_velocities[i].y * TENNISBALL_SPEED * delta_time;  // update tennis ball's y position
    }
    
    // for loop to check collisions between tennis balls and rackets
    for (int i = 0; i < g_number_of_balls; ++i) {
        // racket1 collision check
        float racket1_x_distance = fabs(g_racket1_position.x + INIT_POS_RACKET1.x - g_tennisball_positions[i].x) -
            ((INIT_TENNISBALL_SCALE.x + INIT_RACKET_SCALE.x) / 2.0f);   // calculate horizontal distance between tennis ball and racket1
        float racket1_y_distance = fabs(g_racket1_position.y + INIT_POS_RACKET1.y - g_tennisball_positions[i].y) -
            ((INIT_TENNISBALL_SCALE.y + INIT_RACKET_SCALE.y) / 2.0f);   // calculate vertical distance between tennis ball and racket1
        
        // checks if there is a collision (distance <= 0)
        if (racket1_x_distance < 0.0f && racket1_y_distance < 0.0f) {
            float impact_position = (g_tennisball_positions[i].y - (g_racket1_position.y + INIT_POS_RACKET1.y));    // calculate impact position of ball on the racket
            float racket_half_height = INIT_RACKET_SCALE.y / 2.0f;                                                  // calculate half of racket's height
            float bounce = impact_position / racket_half_height;                                                    // calculate ball's bounce based on where on the racket                                                                                                        it was hit
            g_tennisball_velocities[i].x = 1.5f;                                                                    // bounce to the right when hit with racket1
            g_tennisball_velocities[i].y = 1.0f * bounce;                                                           // adjust y velocity based on bounce
        }

        // racket2 collision check
        float racket2_x_distance = fabs(g_racket2_position.x + INIT_POS_RACKET2.x - g_tennisball_positions[i].x) -
            ((INIT_TENNISBALL_SCALE.x + INIT_RACKET_SCALE.x) / 2.0f);
        float racket2_y_distance = fabs(g_racket2_position.y + INIT_POS_RACKET2.y - g_tennisball_positions[i].y) -
            ((INIT_TENNISBALL_SCALE.y + INIT_RACKET_SCALE.y) / 2.0f);

        if (racket2_x_distance < 0.0f && racket2_y_distance < 0.0f) {
            float impact_position = (g_tennisball_positions[i].y - (g_racket2_position.y + INIT_POS_RACKET2.y));
            float racket_half_height = INIT_RACKET_SCALE.y / 2.0f;
            float bounce = impact_position / racket_half_height;
            g_tennisball_velocities[i].x = -1.5f;
            g_tennisball_velocities[i].y = 1.0f * bounce;
        }

        
        // checks for wall collisions (left and right walls)
        if (!g_show_game_over){                                  // checks if game over screen is being shown
            if (g_tennisball_positions[i].x <= -5.0f) {          // checks if ball has hit the wall on the left
                LOG("Player 2 Wins");                            // prints that player/racket 2 wins (ball hits left wall)
                g_player2_win = true;                            // sets player 2 win flag
                g_show_game_over = true;                         // sets final screen flag, final screen should be shown
            }else if (g_tennisball_positions[i].x >= 5.0f) {     // checks if ball has hit the wall on the right
                LOG("Player 1 Wins");                            // prints that player/racket 1 wins (ball hits right wall)
                g_player1_win = true;                            // sets player 1 win flag
                g_show_game_over = true;                         // sets final screen flag, final screen should be shown
            }
        }
        

        // check for wall collitions (top and bottom walls)
        if (g_tennisball_positions[i].y + INIT_TENNISBALL_SCALE.y / 2.0f > 3.75f) {     // checks if ball hits the top of the screen
            g_tennisball_velocities[i].y = -fabs(g_tennisball_velocities[i].y);         // bounce off the top (bounce down)
        }
        if (g_tennisball_positions[i].y - INIT_TENNISBALL_SCALE.y / 2.0f < -3.75f) {    // checks if the ball hits the bottom of the screen
            g_tennisball_velocities[i].y = fabs(g_tennisball_velocities[i].y);          // bounce off the bottom (bounce up)
        }
    }


    // initialize transformation matrices
    g_background_matrix = glm::mat4(1.0f);
    g_racket1_matrix   = glm::mat4(1.0f);
    g_racket2_matrix   = glm::mat4(1.0f);

    /* Transformations */
    g_background_matrix = glm::scale(g_background_matrix, INIT_BACKGROUND_SCALE);   // scale background
    
    g_racket1_matrix = glm::translate(g_racket1_matrix, INIT_POS_RACKET1);          // set initial position of racket1
    g_racket1_matrix = glm::translate(g_racket1_matrix, g_racket1_position);        // update racket1 position
    g_racket1_matrix = glm::scale(g_racket1_matrix, INIT_RACKET_SCALE);             // scale racket1
    
    g_racket2_matrix = glm::translate(g_racket2_matrix, INIT_POS_RACKET2);          // set initial position of racket2
    g_racket2_matrix = glm::translate(g_racket2_matrix, g_racket2_position);        // update racket2 position
    g_racket2_matrix = glm::scale(g_racket2_matrix, INIT_RACKET_SCALE);             // scale racket2

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

    // bind texture and draw background object
    draw_object(g_background_matrix, g_background_texture_id);
    
    // checks if game over screen is shown
    if (g_show_game_over) {
        g_game_over_matrix = glm::mat4(1.0f);                                                       // initialize game over screen matrix
        g_game_over_matrix = glm::scale(g_game_over_matrix, INIT_FINAL_SCREEN_SCALE);               // scale game over screen
        g_final_texture_id = g_player1_win ? g_player1_win_texture_id : g_player2_win_texture_id;   // set game over screen texture based on which player won
        draw_object(g_game_over_matrix, g_final_texture_id);                                        // draw game over screen with given texture
        
        // if it is not game over
        } else {
            draw_object(g_racket1_matrix, g_racket_texture_id);                                     // draw racket1
            draw_object(g_racket2_matrix, g_racket_texture_id);                                     // draw racket2
            
            // draw tennis balls based on user input of number of balls (1, 2, or 3)
            for (int i = 0; i < g_number_of_balls; ++i) {
                g_tennisball_matrix = glm::translate(glm::mat4(1.0f), g_tennisball_positions[i]);   // create transformation matrix for each ball
                g_tennisball_matrix = glm::scale(g_tennisball_matrix, INIT_TENNISBALL_SCALE);       // scale ball(s)
                draw_object(g_tennisball_matrix, g_tennisball_texture_id);                          // draw tennis ball with texture_id
            }
        }

    
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
