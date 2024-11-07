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

void LineSystem::draw_grid_lines(bool draw_middle, float view_x, float view_z, const glm::vec3 pos_camera)
{
    float current_y = pos_camera.y;


    int power_of_2 = 1;
    while (power_of_2 <= (2 * int(std::abs(current_y) / 5) + 1)) power_of_2 *= 2;
    int multiplier_zoom = power_of_2 / 2;

    int max_lines = -100 * std::log(multiplier_zoom + 10) + 550;


    int lower_multiple = (int)(pos_camera.x / multiplier_zoom) * multiplier_zoom;
    int upper_multiple = lower_multiple + multiplier_zoom;
    int current_x = (std::abs(pos_camera.x - lower_multiple) < std::abs(pos_camera.x - upper_multiple)) ? lower_multiple : upper_multiple;
    lower_multiple = (int)(pos_camera.z / multiplier_zoom) * multiplier_zoom;
    upper_multiple = lower_multiple + multiplier_zoom;
    int current_z = (std::abs(pos_camera.z - lower_multiple) < std::abs(pos_camera.z - upper_multiple)) ? lower_multiple : upper_multiple;

    int threshold_lines_rendered_behind_camera = multiplier_zoom * 50; // multiplier * nb lines you want behind the camera

    for (int i = -max_lines; i < max_lines; i++) {

        int pos_line = i * multiplier_zoom;
        if (view_x < 0) {
            if (view_z < 0) {
                if (current_x + pos_line < current_x + threshold_lines_rendered_behind_camera && current_x + pos_line != 0) draw_line({ {current_x + pos_line, 0.0, -10000}, {current_x + pos_line, 0.0, current_z + threshold_lines_rendered_behind_camera}, {0.3, 0.3, 0.3} });
                if (current_z + pos_line < current_z + threshold_lines_rendered_behind_camera && current_z + pos_line != 0) draw_line({ {-10000, 0.0, current_z + pos_line}, {current_x + threshold_lines_rendered_behind_camera, 0.0, current_z + pos_line}, {0.3, 0.3, 0.3} });
                //draw_middle
                if (current_x + pos_line == 0 && draw_middle) draw_line({ {current_x + pos_line, 0.0, -10000}, {current_x + pos_line, 0.0, current_z + threshold_lines_rendered_behind_camera}, {0.3, 0.3, 0.3} });
                if (current_z + pos_line == 0 && draw_middle) draw_line({ {-10000, 0.0, current_z + pos_line}, {current_x + threshold_lines_rendered_behind_camera, 0.0, current_z + pos_line}, {0.3, 0.3, 0.3} });
            }
            else
            {
                if (current_x + pos_line < current_x + threshold_lines_rendered_behind_camera && current_x + pos_line != 0) draw_line({ {current_x + pos_line, 0.0, current_z - threshold_lines_rendered_behind_camera}, {current_x + pos_line, 0.0, 10000}, {0.3, 0.3, 0.3} });
                if (current_z + pos_line > current_z - threshold_lines_rendered_behind_camera && current_z + pos_line != 0) draw_line({ {-10000, 0.0, current_z + pos_line}, {current_x + threshold_lines_rendered_behind_camera, 0.0, current_z + pos_line}, {0.3, 0.3, 0.3} });
                //draw_middle
                if (current_x + pos_line == 0 && draw_middle) draw_line({ {current_x + pos_line, 0.0, current_z - threshold_lines_rendered_behind_camera}, {current_x + pos_line, 0.0, 10000}, {0.3, 0.3, 0.3} });
                if (current_z + pos_line == 0 && draw_middle) draw_line({ {-10000, 0.0, current_z + pos_line}, {current_x + threshold_lines_rendered_behind_camera, 0.0, current_z + pos_line}, {0.3, 0.3, 0.3} });
            }
        }
        else
        {
            if (view_z < 0) {
                if (current_x + pos_line > current_x - threshold_lines_rendered_behind_camera && current_x + pos_line != 0) draw_line({ {current_x + pos_line, 0.0, -10000}, {current_x + pos_line, 0.0, current_z + threshold_lines_rendered_behind_camera}, {0.3, 0.3, 0.3} });
                if (current_z + pos_line < current_z + threshold_lines_rendered_behind_camera && current_z + pos_line != 0) draw_line({ {current_x - threshold_lines_rendered_behind_camera, 0.0, current_z + pos_line}, {10000, 0.0, current_z + pos_line}, {0.3, 0.3, 0.3} });
                //draw_middle
                if (current_x + pos_line == 0 && draw_middle) draw_line({ {current_x + pos_line, 0.0, -10000}, {current_x + pos_line, 0.0, current_z + threshold_lines_rendered_behind_camera}, {0.3, 0.3, 0.3} });
                if (current_z + pos_line == 0 && draw_middle) draw_line({ {current_x - threshold_lines_rendered_behind_camera, 0.0, current_z + pos_line}, {10000, 0.0, current_z + pos_line}, {0.3, 0.3, 0.3} });
            }
            else
            {
                if (current_x + pos_line > current_x - threshold_lines_rendered_behind_camera && current_x + pos_line != 0) draw_line({ {current_x + pos_line, 0.0, current_z - threshold_lines_rendered_behind_camera}, {current_x + pos_line, 0.0, 10000}, {0.3, 0.3, 0.3} });
                if (current_z + pos_line > current_z - threshold_lines_rendered_behind_camera && current_z + pos_line != 0) draw_line({ {current_x - threshold_lines_rendered_behind_camera, 0.0, current_z + pos_line}, {10000, 0.0, current_z + pos_line}, {0.3, 0.3, 0.3} });
                //draw_middle
                if (current_x + pos_line == 0 && draw_middle) draw_line({ {current_x + pos_line, 0.0, current_z - threshold_lines_rendered_behind_camera}, {current_x + pos_line, 0.0, 10000}, {0.3, 0.3, 0.3} });
                if (current_z + pos_line == 0 && draw_middle) draw_line({ {current_x - threshold_lines_rendered_behind_camera, 0.0, current_z + pos_line}, {10000, 0.0, current_z + pos_line}, {0.3, 0.3, 0.3} });
            }
        }
    }
}

void LineSystem::render_lines_ref_frame_grid(bool draw_ref_frame, bool draw_grid, float view_x, float view_z, const glm::vec3 pos_camera, unsigned int shader)
{
    //Draw Reference Frame
    if (draw_ref_frame)
        draw_line_from_vector(reference_frame);

    //Draw grid
    if (draw_grid)
        draw_grid_lines(!draw_ref_frame, view_x, view_z, pos_camera);

    //Render all lines
    render_lines(shader);
}