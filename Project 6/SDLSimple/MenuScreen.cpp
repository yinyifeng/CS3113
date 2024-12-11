/**
* Author: [Yinyi Feng]
* Assignment: The Glitch
* Date due: 2024-12-11, 2:00pm
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
    // Set the view matrix to identity for UI rendering
    glm::mat4 identity_matrix = glm::mat4(1.0f);
    g_shader_program->set_view_matrix(identity_matrix);

    glClearColor(0.1f, 0.5f, 0.4f, 1.0f); // Cool blue-green


    glClear(GL_COLOR_BUFFER_BIT);

    // Text properties
    float text_size = 0.7f;
    float text_spacing = 0.05f;
    
    // Render "Game Over!" centered
    Utility::draw_text(g_shader_program, g_font_texture_id, "The Glitch", text_size, text_spacing, glm::vec3(-3.3f, 2.5f, 0.0f));

    // Render "You Won!" slightly below
    Utility::draw_text(g_shader_program, g_font_texture_id, "'Enter' to Play", 0.3, text_spacing, glm::vec3(-2.8f, 0.0f, 0.0f));
    
    Utility::draw_text(g_shader_program, g_font_texture_id, "'C' for Light", 0.3, text_spacing, glm::vec3(-2.6f, -1.5f, 0.0f));
}
