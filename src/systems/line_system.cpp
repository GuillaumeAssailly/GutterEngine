#include "line_system.h"

LineSystem::LineSystem(): lineData()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, lineData.size() * sizeof(float), lineData.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
}

void LineSystem::draw_line(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color)
{
    // position point 1
    lineData.push_back(p1.x);
    lineData.push_back(p1.y);
    lineData.push_back(p1.z);

    // color point 1
    lineData.push_back(color.r);
    lineData.push_back(color.g);
    lineData.push_back(color.b);

    // position point 2
    lineData.push_back(p2.x);
    lineData.push_back(p2.y);
    lineData.push_back(p2.z);

    // color point 2
    lineData.push_back(color.r);
    lineData.push_back(color.g);
    lineData.push_back(color.b);
}

void LineSystem::draw_line(const Line line)
{
    // position point 1
    lineData.push_back(line.start.x);
    lineData.push_back(line.start.y);
    lineData.push_back(line.start.z);

    // color point 1
    lineData.push_back(line.color.r);
    lineData.push_back(line.color.g);
    lineData.push_back(line.color.b);

    // position point 2
    lineData.push_back(line.end.x);
    lineData.push_back(line.end.y);
    lineData.push_back(line.end.z);

    // color point 2
    lineData.push_back(line.color.r);
    lineData.push_back(line.color.g);
    lineData.push_back(line.color.b);
}

void LineSystem::draw_line_from_vector(const std::vector<Line> lines_vector)
{
    for (auto& line : lines_vector) draw_line(line);
}

void LineSystem::render_lines(unsigned int shader)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE,glm::value_ptr(glm::mat4(1.0f)));

    glUniform1i(glGetUniformLocation(shader, "isLine"), true);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, lineData.size() * sizeof(float), lineData.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, lineData.size() / 6);
    glUniform1i(glGetUniformLocation(shader, "isLine"), false);

    lineData.clear();
}