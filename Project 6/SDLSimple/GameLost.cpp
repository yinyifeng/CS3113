/**
* Author: [Yinyi Feng]
* Assignment: The Glitch
* Date due: 2024-12-11, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "GameLost.h"
#include "Utility.h"

constexpr char FONTSHEET_FILEPATH[] = "assets/font1.png";

GameLost::~GameLost()
{
    delete m_game_state.player;
    delete m_game_state.map;
}

void GameLost::initialise() {
    g_font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
    
    m_game_state.player = new Entity();
    m_game_state.map = nullptr;
}

void GameLost::update(float delta_time) {
    m_game_state.next_scene_id = -1;
    
    Mix_PauseMusic();
}

void GameLost::render(ShaderProgram* g_shader_program) {
    // Set the view matrix to identity for UI rendering
    glm::mat4 identity_matrix = glm::mat4(1.0f);
    g_shader_program->set_view_matrix(identity_matrix);

    glClearColor(0.749, 0.024, 0.082, 1); // Background color
    glClear(GL_COLOR_BUFFER_BIT);

    // Text properties
    float text_size = 0.5f;
    float text_spacing = 0.05f;
    
    // Render "Game Over!" centered
    Utility::draw_text(g_shader_program, g_font_texture_id, "Game Over!", text_size, text_spacing, glm::vec3(-2.8f, 1.5f, 0.0f));

    // Render "You Won!" slightly below
    Utility::draw_text(g_shader_program, g_font_texture_id, "You Lose!", text_size, text_spacing, glm::vec3(-2.3f, 0.0f, 0.0f));
}

