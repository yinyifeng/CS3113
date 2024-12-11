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

class LevelA : public Scene {
    glm::mat4 m_view_matrix;
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 3;
    int COINS_COUNT = 3;
    int m_coins_collected = 0;
    bool played = false;
    
    // ————— DESTRUCTOR ————— //
    ~LevelA();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
