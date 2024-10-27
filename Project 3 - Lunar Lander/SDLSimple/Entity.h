/**
* Author: Yinyi Feng
* Assignment: Lunar Lander
* Date due: 2024-10-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"

enum EntityType { PLATFORM, PLAYER, ENEMY  };
enum Animation { IDLE, ATTACK };

class Entity
{
private:
    EntityType m_entity_type;
    // ————— TEXTURES ————— //
    std::vector<GLuint> m_texture_ids;  // Vector of texture IDs for different animations

    // ————— ANIMATIONS ————— //
    std::vector<std::vector<int>> m_animations;  // Indices for each animation type

    glm::vec3 m_movement;
    glm::vec3 m_position = glm::vec3(0.0f, -100.0f, 0.0f);
    glm::vec3 m_scale;

    glm::mat4 m_model_matrix;
    float m_speed;

    int m_animation_cols;
    int m_animation_frames, m_animation_index, m_animation_rows;

    Animation m_current_animation;  // Current animation state
    int* m_animation_indices = nullptr;
    float m_animation_time = 0.0f;
    
    // New rotation member variable
    float m_rotation; // Rotation angle in degrees
    
    float m_vertical_velocity = 0.0f; // Current vertical velocity
    const float GRAVITY = -0.05f; // Gravity acceleration
    const float JUMP_VELOCITY = 2.0f; // Initial jump velocity
    
    float horizontal_velocity = 0.0f;
    float vertical_velocity = 0.0f;
    
    glm::vec3 m_velocity; // To hold the current velocity
    glm::vec3 m_acceleration; // To hold the current acceleration
    float m_gravity; // Gravity acceleration
    
    float ACCELERATION = 2.0f;
    
    float m_width = 1.0f,
          m_height = 1.0f;
    // ————— COLLISIONS ————— //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;
    
public:
    static constexpr int SECONDS_PER_FRAME = 6;

    // ————— CONSTRUCTORS ————— //
    Entity();
    Entity(std::vector<GLuint> texture_ids, float speed,
           std::vector<std::vector<int>> animations, float animation_time,
           int animation_frames, int animation_index, int animation_cols,
           int animation_rows, Animation animation);
    ~Entity();

    // ————— METHODS ————— //
    void draw_sprite_from_texture_atlas(ShaderProgram* program);
//    void update(float delta_time);
    void render(ShaderProgram* program);

    // Animation control
    void set_animation_state(Animation new_animation);
    void normalise_movement() { m_movement = glm::normalize(m_movement); };

    // Getters and Setters
    glm::vec3 const get_position() const { return m_position; }
    glm::vec3 const get_movement() const { return m_movement; }
    glm::vec3 const get_scale() const { return m_scale; }
    float const get_speed() const { return m_speed; }

    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    
    void set_rotation(float rotation) { m_rotation = rotation; } // Setter for rotation
    float get_rotation() const { return m_rotation; } // Getter for rotation
    
    // New method to set vertical velocity (if needed)
//    void set_vertical_velocity(float velocity) {m_vertical_velocity = velocity;}
    float get_vertical_velocity() const {return m_vertical_velocity;}
    
    glm::vec3 get_direction() const {
        float radians = glm::radians(m_rotation); // Convert degrees to radians
        return glm::vec3(cos(radians), sin(radians), 0.0f); // X and Y components based on rotation
    }
    
    void set_horizontal_velocity(float velocity) {
            horizontal_velocity = velocity;
    }

    void set_vertical_velocity(float velocity) {
        vertical_velocity = velocity;
    }



//    void set_acceleration(const glm::vec3& acceleration) { m_acceleration = acceleration; }
    void set_velocity(const glm::vec3& velocity) { m_velocity = velocity; }

    glm::vec3 get_velocity() const { return m_velocity; }
    glm::vec3 get_acceleration() const { return m_acceleration; }
    
    void update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map);
    void set_acceleration(const glm::vec3& acceleration) {
        m_acceleration = acceleration;
    }
    
    bool check_collision(Entity* other) const;
    
    void check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
    
    // Overloading our methods to check for only the map
    void const check_collision_y(Map *map);
    void const check_collision_x(Map *map);
    

};
