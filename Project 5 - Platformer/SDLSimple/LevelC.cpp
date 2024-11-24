/**
* Author: [Yinyi Feng]
* Assignment: Platformer
* Date due: 2023-11-23, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelC.h"
#include "Utility.h"

#define LEVELC_WIDTH 25
#define LEVELC_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[]   = "assets/ash_ketchup.png",
               TILESET_FILEPATH[]       = "assets/extended_tilesheet.png",
               ENEMY_FILEPATH[]         = "assets/piplup.png";

unsigned int LEVELC_DATA[] =
{
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 29,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  9, 10, 11,  0,
     0,  0,  0,  0,  0,  0, 33, 34, 35,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0, 29,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 33, 34, 35,  0,  0,  0,  0,  0,  0,
    12, 13, 13, 13, 14,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    16, 16, 16, 16, 17,  0,  0,  0,  0,  0,  0, 33, 34, 35,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};


LevelC::~LevelC()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelC::initialise()
{
    GLuint map_texture_id = Utility::load_texture(TILESET_FILEPATH);
    m_game_state.map = new Map(LEVELC_WIDTH, LEVELC_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 36, 1);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    int player_walking_animation[4][3] =
    {
        { 3, 7, 11 },  // for Player to move to the left,
        { 1, 5, 9  },  // for Player to move to the right,
        { 0, 4, 8  },  // for Player to move upwards,
        { 2, 6, 10 }   // for Player to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        0.0f,                      // jumping power
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
    
    m_game_state.player->set_position(glm::vec3(0.5f, -5.15f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(5.0f);
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    
    int enemy_walking_animation[4][3] =
    {
        { 3, 7, 11 },  // for Enemy to move to the left,
        { 1, 5, 9  },  // for Enemy to move to the right,
        { 0, 4, 8  },  // for Enemy to move upwards,
        { 2, 6, 10 }   // for Enemy to move downwards
    };

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] =  Entity(enemy_texture_id, 0.5f, acceleration, 0.1f, enemy_walking_animation, 0.0f, 3, 0, 4, 3, 0.25f, 0.7f, ENEMY);
        m_game_state.enemies[i].set_movement(glm::vec3(0.0f));
        m_game_state.enemies[i].face_down();
    }
    
    m_game_state.enemies[0].set_position(glm::vec3(7.08f, -3.15f, 0.0f));
    m_game_state.enemies[0].set_ai_type(JUMPER);
    m_game_state.enemies[0].set_jumping_power(3.0f);
    m_game_state.enemies[0].set_acceleration(glm::vec3(0, -9.8f, 0));
    
    m_game_state.enemies[1].set_position(glm::vec3(17.08f, -4.15f, 0.0f));
    m_game_state.enemies[1].set_ai_type(JUMPER);
    m_game_state.enemies[1].set_jumping_power(3.0f);
    m_game_state.enemies[1].set_acceleration(glm::vec3(0, -9.8f, 0));

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/bg_music.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/jump.wav");
    m_game_state.lvl_up = Mix_LoadWAV("assets/lvlup.wav");
}

void LevelC::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].ai_activate(m_game_state.player, delta_time);
        if (m_game_state.enemies[i].get_is_active())
            m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
    }
    
    if (m_game_state.player->get_position().x > 23.5f) {
        m_game_state.next_scene_id = 4; // Transition to GameWon
        Mix_PlayChannel(-1, m_game_state.lvl_up, 0); // level transition sfx
    }
}


void LevelC::render(ShaderProgram *g_shader_program)
{
    glClearColor(0.659, 0.816, 0.929, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    
    for (int i = 0; i < ENEMY_COUNT; i++){
        if (m_game_state.enemies[i].get_is_active())
            m_game_state.enemies[i].render(g_shader_program);
    }
}
