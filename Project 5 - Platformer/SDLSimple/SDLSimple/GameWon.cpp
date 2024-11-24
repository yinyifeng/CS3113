/**
* Author: [Yinyi Feng]
* Assignment: Platformer
* Date due: 2023-11-23, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "GameWon.h"
#include "Utility.h"

constexpr char FONTSHEET_FILEPATH[] = "assets/font1.png";

GameWon::~GameWon()
{
    delete m_game_state.player;
    delete m_game_state.map;
}

void GameWon::initialise() {
    g_font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
    
    m_game_state.player = new Entity();
    m_game_state.map = nullptr;
    
    Mix_PauseMusic();
}

void GameWon::update(float delta_time) {
    if (m_game_state.player->get_position().x > 23.5f) {
            m_game_state.next_scene_id = -1;
    }
}

void GameWon::render(ShaderProgram* g_shader_program) {

    glClearColor(0.204, 0.788, 0.322, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    Utility::draw_text(g_shader_program, g_font_texture_id, "Game Over!", 0.5f, 0.05f, glm::vec3(2.9f, -2.0f, 0.0f));
    Utility::draw_text(g_shader_program, g_font_texture_id, "You Won!", 0.5f, 0.05f, glm::vec3(3.0f, -4.0f, 0.0f));
}
