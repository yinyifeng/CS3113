/**
* Author: [Yinyi Feng]
* Assignment: The Glitch
* Date due: 2024-12-11, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelC.h"
#include "Utility.h"

#define LEVELC_WIDTH 36
#define LEVELC_HEIGHT 28

constexpr char SPRITESHEET_FILEPATH[]   = "assets/knight_2.png",
               TILESET_FILEPATH[]       = "assets/dungeon.png",
               ENEMY_FILEPATH[]         = "assets/skeleton.png",
               COIN_FILEPATH[]          = "assets/coin.png";

unsigned int LEVELC_DATA[] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,2,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    9,46,57,51,57,46,11,0,0,0,0,0,9,46,46,46,68,69,46,46,46,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    9,46,66,87,66,46,11,0,0,0,0,0,9,46,0,0,0,0,0,0,46,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    9,46,0,87,0,46,11,0,0,0,0,0,9,46,0,0,0,0,0,0,46,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    9,46,0,87,0,46,31,1,1,1,1,1,53,29,72,74,0,0,74,75,27,23,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    9,46,0,87,0,46,46,57,46,67,46,57,46,46,0,0,89,0,0,0,0,31,1,1,1,2,0,0,0,0,0,0,0,0,0,0,
    9,46,0,95,77,80,0,66,79,77,80,66,0,0,0,0,87,0,89,0,0,0,0,0,0,31,1,1,1,1,1,1,1,1,1,2,
    9,46,0,87,0,0,0,0,0,0,0,0,0,0,0,0,98,0,87,0,0,0,0,0,0,0,0,0,46,46,57,46,46,57,46,11,
    9,46,0,87,0,0,0,0,0,0,0,0,0,0,0,0,0,0,98,0,10,0,0,79,77,77,80,0,0,0,66,0,0,66,0,11,
    9,46,0,85,77,80,0,0,79,77,77,77,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,
    9,46,46,46,46,46,46,54,46,65,46,54,46,46,11,0,0,0,0,0,0,0,0,0,0,9,0,0,40,19,19,19,19,19,19,20,
    18,19,19,19,19,19,19,19,19,19,19,19,19,19,39,0,0,0,0,11,76,77,77,77,78,9,0,0,11,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,75,0,0,72,11,87,46,67,46,87,9,0,0,11,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,11,85,77,77,77,86,9,0,0,11,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,31,1,1,1,1,1,30,0,0,11,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,11,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,11,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,19,19,19,19,19,19,19,19,19,19,19,19,19,20,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

};

LevelC::~LevelC()
{
    delete [] m_game_state.enemies;
    delete [] m_game_state.coins;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.coin_sfx);
    Mix_FreeChunk(m_game_state.lvl_up);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelC::initialise()
{
    GLuint map_texture_id = Utility::load_texture(TILESET_FILEPATH);
    m_game_state.map = new Map(LEVELC_WIDTH, LEVELC_HEIGHT, LEVELC_DATA, map_texture_id, 0.5f, 9, 11);
    
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
        1.2f,                      // speed
        acceleration,              // acceleration
        0.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.5f,                      // width
        0.5f,                      // height
        PLAYER
    );
    
    m_game_state.player->set_position(glm::vec3(1.5f, -1.5f, 0.0f));
    
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
    m_game_state.enemies[i] =  Entity(enemy_texture_id, 0.8f, acceleration, 0.0f, enemy_walking_animation, 0.0f, 4, 0, 4, 4, 0.1f, 0.1f, ENEMY);
    m_game_state.enemies[i].set_movement(glm::vec3(0.0f));
    }

    m_game_state.enemies[0].set_position(glm::vec3(1.0f, -5.0f, 0.0f));
    m_game_state.enemies[0].set_ai_type(GUARD);
    m_game_state.enemies[0].set_ai_state(IDLE);
    
    m_game_state.enemies[1].set_position(glm::vec3(7.0f, -3.0f, 0.0f));
    m_game_state.enemies[1].set_ai_type(GUARD);
    m_game_state.enemies[1].set_ai_state(IDLE);
    
    m_game_state.enemies[2].set_position(glm::vec3(10.0f, -8.5f, 0.0f));
    m_game_state.enemies[2].set_ai_type(GUARD);
    m_game_state.enemies[2].set_ai_state(IDLE);

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
        m_game_state.coins[i] = Entity(coin_texture_id, 0.0f, 0.5f, 0.5f, COIN);
        m_game_state.coins[i].set_scale(glm::vec3(0.3f));
    }
    
    m_game_state.coins[0].set_position(glm::vec3(9.5f, -1.5f, 0.0f));
    m_game_state.coins[1].set_position(glm::vec3(11.0f, -8.5f, 0.0f));
    m_game_state.coins[2].set_position(glm::vec3(6.5f, -3.5f, 0.0f));
    m_game_state.coins[3].set_position(glm::vec3(11.0f, -5.4f, 0.0f));
    m_game_state.coins[4].set_position(glm::vec3(15.5f, -5.0f, 0.0f));
    
}

void LevelC::update(float delta_time) {
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
        
    glm::vec3 player_position = m_game_state.player->get_position();

    // Clamp player position within level bounds
    if (player_position.x < 0.5f) player_position.x = 0.5f;
    if (player_position.x > LEVELC_WIDTH * 0.5f) player_position.x = LEVELC_WIDTH * 0.5f;
    if (player_position.y > -0.5f) player_position.y = -0.5f;
    if (player_position.y < -LEVELC_HEIGHT * 0.5f) player_position.y = -LEVELC_HEIGHT * 0.5f;

    m_game_state.player->set_position(player_position);

    // Calculate camera position based on player position
    float camera_x = player_position.x;
    float camera_y = player_position.y;

    // Clamp camera position within bounds
    if (camera_x < 4.0f) camera_x = 4.0f;
    if (camera_x > (LEVELC_WIDTH * 0.5f) - 5.0f) camera_x = (LEVELC_WIDTH * 0.5f) - 5.0f;
    if (camera_y < -(LEVELC_HEIGHT * 0.5f) + 3.5f) camera_y = -(LEVELC_HEIGHT * 0.5f) + 3.5f;
    if (camera_y > 0.0f) camera_y = 0.0f;

    // Update view matrix (camera transformation)
    m_view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-camera_x, -camera_y, 0));
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        // Activate enemy AI
        m_game_state.enemies[i].ai_activate(m_game_state.player, delta_time);
        
        // Update active enemies
        if (m_game_state.enemies[i].get_is_active()) {
            m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
        }
    }

    
    if (m_game_state.player->get_position().x > 23.5f) {
        m_game_state.next_scene_id = 2; // Transition to LevelB
    }
    
    
    for (int i = 0; i < COINS_COUNT; i++) {
        if (m_game_state.coins[i].get_is_active() &&
            m_game_state.player->check_collision(&m_game_state.coins[i])) {
            
            m_game_state.coins[i].deactivate();  // Deactivate the coin
            m_coins_collected++;                // Increment coin counter
            Mix_PlayChannel(-1, m_game_state.coin_sfx, 0);
        }
    }
    
//    // Log the tile the player is walking on
//        int tile_x = static_cast<int>(floor(player_position.x / 0.5f));  // Convert position to tile index
//        int tile_y = static_cast<int>(-ceil(player_position.y / 0.5f));
//        int current_tile = LEVELC_DATA[tile_y * LEVELC_WIDTH + tile_x];
//
//        std::cout << "Player Position: X = " << player_position.x << ", Y = " << player_position.y << std::endl;
//        std::cout << "Player Tile: [" << tile_x << ", " << tile_y << "] with value: " << current_tile << std::endl;

    // Check for all coins collected
    if (m_coins_collected == COINS_COUNT) {
        LEVELC_DATA[10 * LEVELC_WIDTH + 32] = 50;
        m_game_state.map->build();
        if (!played){
            Mix_PlayChannel(-1, m_game_state.lvl_up, 0);
            played = true;
        }
    }

    // Check for collision with tile 50
    glm::vec3 tile_50_position = glm::vec3(32 * 0.5f, -10 * 0.5f, 0.0f);
    if (m_coins_collected == COINS_COUNT &&
        glm::distance(m_game_state.player->get_position(), tile_50_position) < 0.25f) {
        m_game_state.next_scene_id = 4;
    }
}


void LevelC::render(ShaderProgram *g_shader_program) {
    // Pass the view matrix to the shader
//    g_shader_program->set_view_matrix(m_view_matrix);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render the map and player
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.enemies[i].get_is_active()) {
            m_game_state.enemies[i].render(g_shader_program);
        }
    }
    
    // Render all active coins
    for (int i = 0; i < COINS_COUNT; i++) {
        if (m_game_state.coins[i].get_is_active()) {
            m_game_state.coins[i].render(g_shader_program);
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
        0.3f, 0.0f, // Text size and spacing
        glm::vec3(-4.5f, 3.5f, 0.0f) // Adjust position to top-left corner
    );

    // Restore the main view matrix for game rendering
//    g_shader_program->set_view_matrix(m_view_matrix);
}

