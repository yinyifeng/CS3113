/**
* Author: [Yinyi Feng]
* Assignment: Platformer
* Date due: 2023-11-23, 11:59pm
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

    glClearColor(0.749, 0.024, 0.082, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    Utility::draw_text(g_shader_program, g_font_texture_id, "Game Over!", 0.5f, 0.05f, glm::vec3(2.9f, -2.0f, 0.0f));
    Utility::draw_text(g_shader_program, g_font_texture_id, "You Lost!", 0.5f, 0.05f, glm::vec3(3.0f, -4.0f, 0.0f));
}
