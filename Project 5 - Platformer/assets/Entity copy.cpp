#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include <vector>


// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
      m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
      m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
      m_current_animation(IDLE)
{
}

// Parameterized constructor
Entity::Entity(std::vector<GLuint> texture_ids, float speed,
               std::vector<std::vector<int>> animations, float animation_time,
               int animation_frames, int animation_index, int animation_cols,
               int animation_rows, Animation animation)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
      m_speed(speed), m_texture_ids(texture_ids), m_animations(animations),
      m_animation_cols(animation_cols), m_animation_frames(animation_frames),
      m_animation_index(animation_index), m_animation_rows(animation_rows),
      m_animation_time(animation_time), m_current_animation(animation)
{
    set_animation_state(m_current_animation);  // Initialize animation state
}

Entity::~Entity() { }

void Entity::set_animation_state(Animation new_animation)
{
    m_current_animation = new_animation;

    // Update the texture and animation indices based on the current animation
    m_animation_indices = m_animations[m_current_animation].data();
    m_animation_rows = m_animations[m_current_animation].size();
}

// Render the appropriate texture and animation frame
void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program)
{
    GLuint current_texture = m_texture_ids[m_current_animation];  // Get the right texture

    float u_coord = (float) (m_animation_index % m_animation_cols) / (float) m_animation_cols;
    float v_coord = (float) (m_animation_index / m_animation_cols) / (float) m_animation_rows;

    float width = 1.0f / (float) m_animation_cols;
    float height = 1.0f / (float) m_animation_rows;

    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width,
        v_coord, u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    glBindTexture(GL_TEXTURE_2D, current_texture);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0,
                          vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
                          tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::update(float delta_time)
{
    m_animation_time += delta_time;
    float frames_per_second = 1.0f / SECONDS_PER_FRAME;

    // Update animation frame based on time
    if (m_animation_time >= frames_per_second)
    {
        m_animation_time = 0.0f;
        m_animation_index++;

        if (m_animation_index >= m_animation_frames)
        {
            m_animation_index = 0;
        }
    }

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    // Apply gravity
    m_vertical_velocity += GRAVITY * delta_time;

    // Update vertical position
    m_position.y += m_vertical_velocity * delta_time;

    // Reset vertical position if falling below ground level
    if (m_position.y < 0.0f) {
        m_position.y = 0.0f; // Ground level
        m_vertical_velocity = 0.0f; // Reset vertical velocity
    }

    // Initialize horizontal acceleration
    glm::vec3 acceleration(0.0f, 0.0f, 0.0f);

    // Apply movement acceleration only when space is pressed
    if (key_state[SDL_SCANCODE_SPACE]) {
        if (m_rotation == 0.0f){
            acceleration.y = ACCELERATION; // Accelerate right
        } else if (m_rotation == 90.0f){
            acceleration.x = ACCELERATION; // Accelerate right
        } else if (m_rotation == -90.0f){
            acceleration.x = -ACCELERATION; // Accelerate right
        }

    }

    // Update position based on acceleration
    m_position.x += acceleration.x * delta_time;
    m_position.y += acceleration.y * delta_time;

//    const float drift_factor = 0.1f; // Adjust this value as needed
//        if (acceleration.x == 0.0f) {
//            // Apply friction to the velocity
//            if (m_velocity.x > 0.0f) {
//                m_velocity.x -= drift_factor; // Slow down to the left
////                if (m_velocity.x < 0.0f) m_velocity.x = 0.0f; // Prevent negative speed
//            } else if (m_velocity.x < 0.0f) {
//                m_velocity.x += drift_factor; // Slow down to the right
////                if (m_velocity.x > 0.0f) m_velocity.x = 0.0f; // Prevent negative speed
//            }
//        }

    // Update model matrix for rendering
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, -1.0f)); // Apply rotation
}




void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != nullptr) draw_sprite_from_texture_atlas(program);
}
