/**
* Author: [Yinyi Feng]
* Assignment: Platformer
* Date due: 2023-11-23, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "MenuScreen.h"
#include "Utility.h"

constexpr char FONTSHEET_FILEPATH[] = "assets/font1.png";

Menu::~Menu()
{
    delete m_game_state.player;
    delete m_game_state.map;
}

void Menu::initialise() {
    g_font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
    
    m_game_state.player = new Entity();
    m_game_state.map = nullptr;          // Set to null if not used
}

void Menu::update(float delta_time) {
    // Handle any menu-specific updates here if necessary (e.g., animations)
    if (m_game_state.player->get_position().x > 23.5f) {
            m_game_state.next_scene_id = 1; // Transition to LevelA
    }
}

void Menu::render(ShaderProgram* g_shader_program) {
    glClearColor(0.690f, 0.710f, 0.588f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Render "Game Title" and "Press Enter to Start"
    Utility::draw_text(g_shader_program, g_font_texture_id, "The Glitch", 0.5f, 0.05f, glm::vec3(1.6f, -2.0f, 0.0f));
    Utility::draw_text(g_shader_program, g_font_texture_id, "Press Enter to Start", 0.3f, 0.05f, glm::vec3(1.6f, -4.0f, 0.0f));
}
