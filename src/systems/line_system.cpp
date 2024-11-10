#include "line_system.h"

LineSystem::LineSystem(): lineData(), reference_frame()
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

    //Init Reference Frame
    reference_frame.push_back({ {-10000.0, 0.0, 0.0}, {10000.0, 0.0, 0.0}, {1.0, 0.0, 0.0} });
    reference_frame.push_back({ {0.0, -10000.0, 0.0}, {0.0, 10000.0, 0.0}, {0.0, 1.0, 0.0} });
    reference_frame.push_back({ {0.0, 0.0, -10000.0}, {0.0, 0.0, 10000.0}, {0.0, 0.0, 1.0} });
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

int LineSystem::draw_grid_lines(short type_ref_frame, const glm::vec3 pos_camera)
{
    int power_of_2 = 1;
    while (power_of_2 <= (2 * int(std::abs(pos_camera.y) / 5) + 1)) power_of_2 *= 2;
    const int multiplier_zoom = power_of_2 / 2;

    int lower_multiple = (int)(pos_camera.x / multiplier_zoom) * multiplier_zoom;
    int upper_multiple = lower_multiple + multiplier_zoom;
    const int current_x = (std::abs(pos_camera.x - lower_multiple) < std::abs(pos_camera.x - upper_multiple)) ? lower_multiple : upper_multiple;
    lower_multiple = (int)(pos_camera.z / multiplier_zoom) * multiplier_zoom;
    upper_multiple = lower_multiple + multiplier_zoom;
    const int current_z = (std::abs(pos_camera.z - lower_multiple) < std::abs(pos_camera.z - upper_multiple)) ? lower_multiple : upper_multiple;


    const int nb_lines = 10 + (int)( ((std::abs(pos_camera.y) / 5) - (multiplier_zoom / 2)) / (multiplier_zoom - (multiplier_zoom / 2)) * 10 );
    const int limit_line = nb_lines * multiplier_zoom;

    if (!type_ref_frame)
    {
        for (int i = -nb_lines; i < nb_lines + 1; i++) {

            const int pos_x = current_x + i * multiplier_zoom;
            const int pos_z = current_z + i * multiplier_zoom;

            if (pos_x > pos_camera.x - limit_line && pos_x < pos_camera.x + limit_line) draw_line({ {pos_x, 0.0, pos_camera.z - limit_line}, {pos_x, 0.0, pos_camera.z + limit_line}, {0.3, 0.3, 0.3} });
            if (pos_z > pos_camera.z - limit_line && pos_z < pos_camera.z + limit_line) draw_line({ {pos_camera.x - limit_line, 0.0, pos_z}, {pos_camera.x + limit_line, 0.0, pos_z}, {0.3, 0.3, 0.3} });
        }
    }
    else
    {
        for (int i = -nb_lines; i < nb_lines + 1; i++) {

            const int pos_x = current_x + i * multiplier_zoom;
            const int pos_z = current_z + i * multiplier_zoom;

            if (pos_x > pos_camera.x - limit_line && pos_x < pos_camera.x + limit_line && pos_x != 0) draw_line({ {pos_x, 0.0, pos_camera.z - limit_line}, {pos_x, 0.0, pos_camera.z + limit_line}, {0.3, 0.3, 0.3} });
            if (pos_z > pos_camera.z - limit_line && pos_z < pos_camera.z + limit_line && pos_z != 0) draw_line({ {pos_camera.x - limit_line, 0.0, pos_z}, {pos_camera.x + limit_line, 0.0, pos_z}, {0.3, 0.3, 0.3} });
        }
    }

    return limit_line;
}

void LineSystem::reset_reference_frame() {
    reference_frame.at(0).start.x = -10000.0;
    reference_frame.at(0).end.x = 10000.0;
    reference_frame.at(2).start.z = -10000.0;
    reference_frame.at(2).end.z = 10000.0;
}

void LineSystem::change_reference_frame(const int limit_line, const glm::vec3 pos_camera) {
    reference_frame.at(0).start.x = std::min((double)pos_camera.x - limit_line, 0.0);
    reference_frame.at(0).end.x = std::max((double)pos_camera.x + limit_line, 0.0);
    reference_frame.at(2).start.z = std::min((double)pos_camera.z - limit_line, 0.0);
    reference_frame.at(2).end.z = std::max((double)pos_camera.z + limit_line, 0.0);
}

void LineSystem::render_lines_ref_frame_grid(short type_ref_frame, bool draw_grid, const glm::vec3 pos_camera, unsigned int shader)
{
    int limit_line = 0;

    //Draw grid
    if (draw_grid)
        limit_line = draw_grid_lines(type_ref_frame, pos_camera);

    //Draw Reference Frame
    if (type_ref_frame)
    {
        if (type_ref_frame == 1) change_reference_frame(limit_line, pos_camera);
        draw_line_from_vector(reference_frame);
    }

    //Render all lines
    render_lines(shader);
}