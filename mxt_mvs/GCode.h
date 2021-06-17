//
// Created by Patrick on 04.06.2021.
//

#ifndef RV4A_MXT_GCODE_H
#define RV4A_MXT_GCODE_H

#include <iostream>
#include <optional>

#define LINE_DESCRIPTOR                 ('N')
#define EXTRUSION_DESCRIPTOR            ('E')
#define FEEDRATE_DESCRIPTOR             ('F')
#define COMMENT_DESCRIPTOR              (';')
#define CENTER_X_DESCRIPTOR             ('I')
#define CENTER_Y_DESCRIPTOR             ('J')
#define CENTER_Z_DESCRIPTOR             ('K')

class Point3D{
public:
    std::optional<float> x{};
    std::optional<float> y{};
    std::optional<float> z{};
    Point3D()=default;
    friend std::ostream& operator<<(std::ostream& os, const Point3D& point);
};

class GCode {
    const std::string axes{"XYZ"};
public:
    std::optional<unsigned int> line_number{};
    std::string command_id{};
    std::optional<float> feedrate;
    std::optional<float> extrusion_len{};
    std::string homing_axes{};
    std::string text{};
    Point3D pose{};
    Point3D center{};
    GCode()=default;
    bool can_extrude() const;
    bool can_mv_pose() const;
    bool is_home() const;
    bool is_circular() const;
    friend std::ostream& operator<<(std::ostream& os, const GCode& gcode);
    friend std::istream& operator>>(std::istream& is, GCode& gcode);
};

#endif //RV4A_MXT_GCODE_H
