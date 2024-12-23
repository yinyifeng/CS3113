/**
* Author: [Yinyi Feng]
* Assignment: The Glitch
* Date due: 2024-12-11, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelB.h"
#include "Utility.h"

#define LEVELB_WIDTH 25
#define LEVELB_HEIGHT 15

constexpr char SPRITESHEET_FILEPATH[]   = "assets/knight.png",
               TILESET_FILEPATH[]       = "assets/dungeon.png",
               ENEMY_FILEPATH[]         = "assets/skeleton.png",
               COIN_FILEPATH[]          = "assets/coin.png";

unsigned int LEVELB_DATA[] =
{
     0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,    0,   0,   0,    0,   0,    0,    0,    0,    0,   0,   0,   0,   0,  0,
     5,   1,   1,   1,   2,   0,   0,   5,   1,    1,   1,    1,   1,   1,    1,   1,    1,    1,    2,    0,   0,   0,   0,   0,  0,
     9,  46,  51,  46,  11,   0,   0,   9,   10,  72,   73,  74,  74,  73,   73,   75,   0,    0,   11,    0,   0,   0,   0,   0,  0,
     9,   0,  87,   0,  11,   0,   0,   9,   0,   95,   77,  77,  77,  77,    77,  77,  78,    0,   31,    1,   1,   1,   1,   2,  0,
     9,   0,  87,   0,  31,   1,   1,  30,   0,   87,   0,    0,   0,   0,    0,   0,   87,    0,    0,    0,   0,   0,   0,  11,  0,
     9,   0,  87,   0,   0,   0,   0,   0,   0,   87,   0,    0,   0,   0,    0,   0,   95,   77,   77,   77,  80,   0,   0,  11,  0,
     9,  79,  88,  77,  77,  77,  97,  77,  77,   94,   0,    0,   0,   0,    0,   0,   87,    0,    0,    0,   0,   0,   0,  11,  0,
     9,   0,  98,   0,   0,   0,  87,   0,   0,   87,   0,    0,   0,   0,    0,   0,   87,    0,    0,    0,   0,   0,   0,  11,  0,
    18,  19,  19,  19,  39,   0,  87,   0,   0,   85,  77,   77,  77,   77,  77,  77,   94,    0,    0,    0,  40,  19,  19,  20,  0,
     0,   0,   0,   0,   9,   0,  87,   0,   0,    0,  46,   47,  67,  47,   46,   0,   98,    0,    0,    0,  11,   0,   0,   0,  0,
     0,   0,   0,   0,   9,   0,  87,   0,   0,   40,  19,   19,  19,  19,   39,   0,    0,    0,    0,    0,  11,   0,   0,   0,  0,
     0,   0,   0,   0,   9,   0,  87,   0,   0,   11,   0,    0,   0,   0,    9,   0,    0,    0,    0,    0,  11,   0,   0,   0,  0,
     0,   0,   0,   0,   9,   0,  98,   0,   0,   11,   0,    0,   0,   0,   18,  19,   19,   19,   19,   19,  20,   0,   0,   0,  0,
     0,   0,   0,   0,   9,   0,   0,   0,   0,   11,   0,    0,   0,   0,    0,   0,    0,    0,    0,    0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,   18, 19,  19,  19,   19,  20,   0,    0,   0,   0,    0,   0,    0,    0,    0,    0,   0,   0,   0,   0,  0
};

LevelB::~LevelB()
{
    delete [] m_game_state.enemies;
    delete [] m_game_state.coins;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.coin_sfx);
    Mix_FreeChunk(m_game_state.lvl_up);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelB::initialise()
{
    GLuint map_texture_id = Utility::load_texture(TILESET_FILEPATH);
    m_game_state.map = new Map(LEVELB_WIDTH, LEVELB_HEIGHT, LEVELB_DATA, map_texture_id, 0.5f, 9, 11);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    int player_walking_animation[4][4] =
    {
        { 3, 7, 11, 15},  // for Player to move to the left,
        { 2, 6, 10, 14},  // for Player to move to the right,
        { 0, 4,  8, 12},  // for Player to move upwards,
        { 1, 5,  9, 13}   // for Player to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        1.0f,                      // speed
        acceleration,              // acceleration
        0.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.1f,                      // width
        0.1f,                      // height
        PLAYER
    );
    
    m_game_state.player->set_position(glm::vec3(1.0f, -1.0f, 0.0f));
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    
    int enemy_walking_animation[4][4] =
    {
        { 3, 7, 11, 15},  // for Player to move to the left,
        { 2, 6, 10, 14},  // for Player to move to the right,
        { 0, 4,  8, 12},  // for Player to move upwards,
        { 1, 5,  9, 13}   // for Player to move downwards
    };

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
    m_game_state.enemies[i] =  Entity(enemy_texture_id, 0.5f, acceleration, 0.0f, enemy_walking_animation, 0.0f, 4, 0, 4, 4, 0.1f, 0.1f, ENEMY);
    m_game_state.enemies[i].set_movement(glm::vec3(0.0f));
    }

    m_game_state.enemies[0].set_position(glm::vec3(5.0f, -2.5f, 0.0f));
    m_game_state.enemies[0].set_ai_type(GUARD);
    m_game_state.enemies[0].set_ai_state(IDLE);
    
//    m_game_state.enemies[1].set_position(glm::vec3(7.5f, -3.5f, 0.0f));
//    m_game_state.enemies[1].set_ai_type(GUARD);
//    m_game_state.enemies[1].set_ai_state(IDLE);
    m_game_state.enemies[1].set_ai_type(ROTATOR);
    //    m_game_state.enemies[1].set_ai_state(IDLE);
    // Orbit parameters
    m_game_state.enemies[1].m_orbit_center = glm::vec3(6.5f, -2.5f, 0.0f);
    m_game_state.enemies[1].m_orbit_radius = 1.0f;
    m_game_state.enemies[1].m_orbit_speed = 45.0f;
    
    m_game_state.enemies[2].set_position(glm::vec3(7.45f, -5.5f, 0.0f));
    m_game_state.enemies[2].set_ai_type(WALKER);
    m_game_state.enemies[2].set_ai_state(WALKING);


    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/bg_music.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);

    m_game_state.coin_sfx = Mix_LoadWAV("assets/coin.mp3");
    m_game_state.lvl_up = Mix_LoadWAV("assets/unlocked.mp3");
    
    m_game_state.coins = new Entity[COINS_COUNT];
    GLuint coin_texture_id = Utility::load_texture(COIN_FILEPATH);
    
    for (int i = 0; i < COINS_COUNT; i++)
    {
        m_game_state.coins[i] =  Entity(coin_texture_id, 0.0f, 0.5f, 0.5f, COIN);
        m_game_state.coins[i].set_scale(glm::vec3(0.3f));

    }
    
    m_game_state.coins[0].set_position(glm::vec3(2.5f, -6.5f, 0.0f));
    m_game_state.coins[1].set_position(glm::vec3(3.5f, -2.5f, 0.0f));
    m_game_state.coins[2].set_position(glm::vec3(8.5f, -1.5f, 0.0f));
    m_game_state.coins[3].set_position(glm::vec3(8.05f, -5.4f, 0.0f));
    m_game_state.coins[4].set_position(glm::vec3(10.0f, -3.5f, 0.0f));
    
}

void LevelB::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
        
    glm::vec3 player_position = m_game_state.player->get_position();

    // Clamp player position
    if (player_position.x < 0.5f) player_position.x = 0.5f;
    if (player_position.x > LEVELB_WIDTH * 0.5f) player_position.x = LEVELB_WIDTH * 0.5f;
    if (player_position.y > -0.5f) player_position.y = -0.5f;
    if (player_position.y < -LEVELB_HEIGHT * 0.5f) player_position.y = -LEVELB_HEIGHT * 0.5f;

    m_game_state.player->set_position(player_position);

    // Calculate camera position based on player position
    float camera_x = player_position.x;
    float camera_y = player_position.y;

    // Clamp camera position
    if (camera_x < 4.0f) camera_x = 4.0f;
    if (camera_x > (LEVELB_WIDTH * 0.5f) - 5.0f) camera_x = (LEVELB_WIDTH * 0.5f) - 5.0f;
    if (camera_y < -(LEVELB_HEIGHT * 0.5f) + 3.5f) camera_y = -(LEVELB_HEIGHT * 0.5f) + 3.5f;
    if (camera_y > 0.0f) camera_y = 0.0f;

    // Update view matrix
    m_view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-camera_x, -camera_y, 0));
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].ai_activate(m_game_state.player, delta_time);
        if (m_game_state.enemies[i].get_is_active()) {
            m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
             
        }
    }

    
    if (m_game_state.player->get_position().x > 23.5f) {
        m_game_state.next_scene_id = 2; // Transition to LevelB
        Mix_PlayChannel(-1, m_game_state.lvl_up, 0); // level transition sfx
    }
    
    
    for (int i = 0; i < COINS_COUNT; i++) {
        if (m_game_state.coins[i].get_is_active() &&
            m_game_state.player->check_collision(&m_game_state.coins[i])) {
            
            m_game_state.coins[i].deactivate();
            m_coins_collected++;
            Mix_PlayChannel(-1, m_game_state.coin_sfx, 0);
        }
    }

        // Check if all coins are collected
        if (m_coins_collected == COINS_COUNT) {
            // Change tile at [7][22] to 50
            LEVELB_DATA[7 * LEVELB_WIDTH + 22] = 50;

            // Rebuild the map to update tile
            m_game_state.map->build();
            if (!played){
                Mix_PlayChannel(-1, m_game_state.lvl_up, 0);
                played = true;
            }
                
        }
    
    // Check collision with tile 50 (at [7][22])
        glm::vec3 tile_50_position = glm::vec3(22 * 0.5f, -7 * 0.5f, 0.0f); // Calculate tile 50 position
        if (m_coins_collected == COINS_COUNT &&
            glm::distance(m_game_state.player->get_position(), tile_50_position) < 0.25f) {
            m_game_state.next_scene_id = 3;  // Transition to LevelC
        }
}


void LevelB::render(ShaderProgram *g_shader_program)
{
    // Apply the view matrix before rendering the game
    g_shader_program->set_view_matrix(m_view_matrix);

    // Clear the screen
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render the map and all entities
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    
    for (int i = 0; i < COINS_COUNT; i++) {
        if (m_game_state.coins[i].get_is_active()) {
            m_game_state.coins[i].render(g_shader_program);
        }
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.enemies[i].get_is_active()) {
            m_game_state.enemies[i].render(g_shader_program);
        }
    }
    
    // Render UI with an identity view matrix
    glm::mat4 temp_view_matrix = glm::mat4(1.0f);
    g_shader_program->set_view_matrix(temp_view_matrix);

    // Display coin counter in the top-left corner
    Utility::draw_text(
        g_shader_program,
        Utility::load_texture("assets/font1.png"), // Font texture
        "Coins: " + std::to_string(m_coins_collected) + "/" + std::to_string(COINS_COUNT),
        0.3f, 0.0f,
        glm::vec3(-4.5f, 3.5f, 0.0f)
    );
}

