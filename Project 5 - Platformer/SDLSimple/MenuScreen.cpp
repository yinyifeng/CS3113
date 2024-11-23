#include "MenuScreen.h"
#include "Utility.h"

constexpr char FONTSHEET_FILEPATH[] = "assets/font1.png";
//               PLAYER_FILEPATH[]    = "assets/ash_ketchup.png";

constexpr float BG_RED = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

Menu::~Menu()
{
    delete m_game_state.player;
    delete m_game_state.map;
}

void Menu::initialise() {
    g_font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
//    g_player_texture_id = Utility::load_texture(PLAYER_FILEPATH);
    
    m_game_state.player = new Entity();
    
//    m_game_state.player = new Entity(g_player_texture_id, 0.0f, 1.0f, 1.0f, PLAYER);  // Create a player entity
//    int player_walking_animation[4][3] =
//    {
//        { 3, 7, 11 },  // for Player to move to the left,
//        { 1, 5, 9  },  // for Player to move to the right,
//        { 0, 4, 8  },  // for Player to move upwards,
//        { 2, 6, 10 }   // for Player to move downwards
//    };
//
//    m_game_state.player = new Entity(
//        g_player_texture_id,         // texture id
//        0.0f,                      // speed
//        glm::vec3(0.0f),              // acceleration
//        0.0f,                      // jumping power
//        player_walking_animation,  // animation index sets
//        0.0f,                      // animation time
//        4,                         // animation frame amount
//        0,                         // current animation index
//        4,                         // animation column amount
//        3,                         // animation row amount
//        0.7f,                      // width
//        0.7f,                      // height
//        PLAYER
//    );
//
//    m_game_state.player->set_position(glm::vec3(1.6f, -4.0f, 0.0f));

    m_game_state.map = nullptr;          // Set to null if not used
}

void Menu::update(float delta_time) {
    // Handle any menu-specific updates here if necessary (e.g., animations)
    if (m_game_state.player->get_position().x > 23.5f) {
            m_game_state.next_scene_id = 1; // Transition to LevelA
    }
}

void Menu::render(ShaderProgram* g_shader_program) {
    // Set up a solid background
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);// Black background
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Render "Game Title" and "Press Enter to Start"
    Utility::draw_text(g_shader_program, g_font_texture_id, "The Platformer", 0.5f, 0.05f, glm::vec3(1.6f, -2.0f, 0.0f));
    Utility::draw_text(g_shader_program, g_font_texture_id, "Press Enter to Start", 0.3f, 0.05f, glm::vec3(1.6f, -4.0f, 0.0f));
    
//    m_game_state.player->render(g_shader_program);
}
