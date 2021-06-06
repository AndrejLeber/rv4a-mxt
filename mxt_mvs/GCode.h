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

using namespace std;

class Point3D{
public:
    std::optional<float> x{};
    std::optional<float> y{};
    std::optional<float> z{};
    Point3D()=default;
    friend ostream& operator<<(ostream& os, const Point3D& point);
};

class GCode {
    const string axes{"XYZ"};
public:
    std::optional<unsigned int> line_number{};
    string command_id{};
    std::optional<float> feedrate;
    std::optional<float> extrusion_len{};
    string homing_axes{};
    string text{};
    struct Point3D pose{};
    struct Point3D center{};
    GCode()=default;
    bool can_extrude() const;
    bool can_mv_pose() const;
    bool is_home() const;
    bool is_circular() const;
    friend ostream& operator<<(ostream& os, const GCode& gcode);
    friend istream& operator>>(istream& is, GCode& gcode);
};

#endif //RV4A_MXT_GCODE_H
