/**
* Author: [Yinyi Feng]
* Assignment: Platformer
* Date due: 2023-11-23, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Scene.h"

class GameLost : public Scene {
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
