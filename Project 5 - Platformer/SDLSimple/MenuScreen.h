#include "Scene.h"

class Menu : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 1;
    GLuint g_font_texture_id;
    GLuint g_player_texture_id;
    
    // ————— DESTRUCTOR ————— //
    ~Menu();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
