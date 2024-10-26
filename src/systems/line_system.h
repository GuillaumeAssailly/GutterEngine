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
    void render_lines(unsigned int shader);

private:

    std::vector<float> lineData{};
    unsigned int vao, vbo;
};