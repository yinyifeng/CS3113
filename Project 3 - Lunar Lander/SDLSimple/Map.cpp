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

Map::Map(int width, int height, unsigned int *level_data, GLuint texture_id, float tile_size, int tile_count_x, int tile_count_y) : m_width(width), m_height(height),
    m_level_data(level_data), m_texture_id(texture_id), m_tile_size(tile_size), m_tile_count_x(tile_count_x), m_tile_count_y(tile_count_y)
{
    build();
}

void Map::build()
{
    for(int y_coord = 0; y_coord < m_height; y_coord++)
    {
        for(int x_coord = 0; x_coord < m_width; x_coord++)
        {
            int tile = m_level_data[y_coord * m_width + x_coord];
            if (tile == 0) continue;
            if (tile == 1){
                glBindTexture(GL_TEXTURE_2D, tile1_texture_id);
            }

            // Calculate UV-coordinates with padding
            float padding = 0.006f; // Adjust based on your sprite sheet
            float tile_width = (1.0f / m_tile_count_x) - padding;
            float tile_height = (1.0f / m_tile_count_y);

            float u_coord = (tile % m_tile_count_x) * (1.0f / m_tile_count_x) + (padding / 2);
            float v_coord = (tile / m_tile_count_x) * (1.0f / m_tile_count_y) + (padding / 2);

            float x_offset = -(m_tile_size / 2);
            float y_offset = (m_tile_size / 2);

            // Store vertex positions
            m_vertices.insert(m_vertices.end(), {
                x_offset + (m_tile_size * x_coord),  y_offset - (m_tile_size * y_coord),
                x_offset + (m_tile_size * x_coord),  y_offset - (m_tile_size * y_coord) - m_tile_size,
                x_offset + (m_tile_size * x_coord) + m_tile_size, y_offset - (m_tile_size * y_coord) - m_tile_size,
                x_offset + (m_tile_size * x_coord), y_offset - (m_tile_size * y_coord),
                x_offset + (m_tile_size * x_coord) + m_tile_size, y_offset - (m_tile_size * y_coord) - m_tile_size,
                x_offset + (m_tile_size * x_coord) + m_tile_size, y_offset - (m_tile_size * y_coord)
            });

            // Store texture coordinates
            m_texture_coordinates.insert(m_texture_coordinates.end(), {
                u_coord, v_coord,
                u_coord, v_coord + tile_height,
                u_coord + tile_width, v_coord + tile_height,
                u_coord, v_coord,
                u_coord + tile_width, v_coord + tile_height,
                u_coord + tile_width, v_coord
            });
        }
    }

    m_left_bound   = 0 - (m_tile_size / 2);
    m_right_bound  = (m_tile_size * m_width) - (m_tile_size / 2);
    m_top_bound    = 0 + (m_tile_size / 2);
    m_bottom_bound = -(m_tile_size * m_height) + (m_tile_size / 2);
}


void Map::render(ShaderProgram *program)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    program->set_model_matrix(model_matrix);
    
    glUseProgram(program->get_program_id());
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, m_vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, m_texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    
    // Draw all the vertices
    glDrawArrays(GL_TRIANGLES, 0, (int) m_vertices.size() / 2);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


bool Map::is_solid(glm::vec3 position, float *penetration_x, float *penetration_y)
{
    // The penetration between the map and the object
    // The reason why these are pointers is because we want to reassign values
    // to them in case that we are colliding. That way the object that originally
    // passed them as values will keep track of these distances
    // tldr: we're passing by reference
    *penetration_x = 0;
    *penetration_y = 0;
    
    // If we are out of bounds, it is not solid
    if (position.x < m_left_bound || position.x > m_right_bound)  return false;
    if (position.y > m_top_bound  || position.y < m_bottom_bound) return false;
    
    int tile_x = floor((position.x + (m_tile_size / 2))  / m_tile_size);
    int tile_y = -(ceil(position.y - (m_tile_size / 2))) / m_tile_size; // Our array counts up as Y goes down.
    
    // If the tile index is negative or greater than the dimensions, it is not solid
    if (tile_x < 0 || tile_x >= m_width)  return false;
    if (tile_y < 0 || tile_y >= m_height) return false;
    
    // If the tile index is 0 i.e. an open space, it is not solid
    int tile = m_level_data[tile_y * m_width + tile_x];
    if (tile == 0) return false;
    
    // And we likely have some overlap
    float tile_center_x = (tile_x  * m_tile_size);
    float tile_center_y = -(tile_y * m_tile_size);
    
    // And because we likely have some overlap, we adjust for that
    *penetration_x = (m_tile_size / 2) - fabs(position.x - tile_center_x);
    *penetration_y = (m_tile_size / 2) - fabs(position.y - tile_center_y);
    
    return true;
}

int Map::get_tile_type(int x, int y) {
    // Ensure the coordinates are within bounds
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return -1; // Return -1 or another designated value for out of bounds
    }

    // Retrieve the tile ID from the tile data array
    return m_level_data[y * m_width + x]; // Assuming row-major order
}

