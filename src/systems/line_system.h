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

    void reset_reference_frame();

    void render_lines_ref_frame_grid(short type_, bool draw_grid, const glm::vec3 pos_camera, unsigned int shader);
    
private:

    int draw_grid_lines(short type_ref_frame, const glm::vec3 pos_camera);
    void render_lines(unsigned int shader);
    void change_reference_frame(const int limit_line, const glm::vec3 pos_camera);

    std::vector<float> lineData{};
    unsigned int vao, vbo;

    std::vector<Line> reference_frame;
};