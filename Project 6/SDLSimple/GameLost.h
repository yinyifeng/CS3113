/**
* Author: [Yinyi Feng]
* Assignment: The Glitch
* Date due: 2024-12-11, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Scene.h"

class GameLost : public Scene {
    glm::mat4 m_view_matrix;
public:
    // ————— STATIC ATTRIBUTES ————— //
    GLuint g_font_texture_id;
    
    // ————— DESTRUCTOR ————— //
    ~GameLost();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
