#pragma once
#include "../config.h"

typedef struct Line {
    glm::vec3 start;
    glm::vec3 end;
    glm::vec3 color;
} Line;

class LineSystem {
public:

    LineSystem();

    void draw_line(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color);
    void draw_line(const Line line);
    void draw_line_from_vector(const std::vector<Line> lines_vector);

    void render_lines_ref_frame_grid(bool draw_ref_frame, bool draw_grid, float view_x, float view_z, const glm::vec3 pos_camera, unsigned int shader);
    
private:

    void draw_grid_lines(bool draw_middle, float view_x, float view_z, const glm::vec3 pos_camera);
    void render_lines(unsigned int shader);

    std::vector<float> lineData{};
    unsigned int vao, vbo;

    std::vector<Line> reference_frame;
};