//
// Created by Patrick on 04.06.2021.
//

#include <sstream>
#include <vector>
#include <algorithm>
#include "GCode.h"

/*
 * Overload the output operator for poses
 */
ostream& operator<<(ostream& os, const Point3D& point){
    //os << "X" << point.x << " Y" << point.y << " Z" << point.z;
    return os;
}

/*
 * Overload the output operator to define how the command is printed
 */
ostream& operator<<(ostream& os, const GCode& gcode){
    return os;
}

bool GCode::can_extrude() const{
    std::vector<std::string> extr_enabled{"G1", "G2", "G3"};
    return std::find(extr_enabled.begin(), extr_enabled.end(), GCode::command_id) != extr_enabled.end();
}

bool GCode::can_mv_pose() const{
    std::vector<std::string> mv_enabled{"G0", "G1", "G2", "G3"};
    return std::find(mv_enabled.begin(), mv_enabled.end(), GCode::command_id) != mv_enabled.end();
}

bool GCode::is_circular() const{
    std::vector<std::string> is_circular{"G2", "G3"};
    return std::find(is_circular.begin(), is_circular.end(), GCode::command_id) != is_circular.end();
}

bool GCode::is_home() const{
    const std::string home_cmd = "G28";
    return GCode::command_id == home_cmd;
}

/*
 * Overload the input operator to define how the command is read
 */
istream& operator>>(istream& is, GCode& gcode){
    std::getline(is, gcode.text);

    // Remove excess whitespace
    std::size_t start, end;
    // Remove leading whitespace
    start = gcode.text.find_first_not_of(' ');
    // Remove trailing whitespace and checksum asterisk
    end = gcode.text.find_last_not_of(" *");
    gcode.text = gcode.text.substr(start, end - start + 1);

    // Convert all characters to uppercase
    for (auto & c: gcode.text) c = (char)toupper(c);

    // Evaluate only non-empty strings and ignore comments
    if(!gcode.text.empty() && (gcode.text.at(0) != COMMENT_DESCRIPTOR)){
        string word;
        string command_number, line_number;
        stringstream sstream(gcode.text);

        // Get first argument
        std::getline(sstream, word, ' ');

        // Check if G-Code starts with a line number and extract it if so
        if(word.at(0) == LINE_DESCRIPTOR){
            // First argument is line number, second is command ID
            line_number = word.substr(1, word.size() - 1);
            line_number.erase(0, line_number.find_first_not_of('0'));
            gcode.line_number = std::stoi(line_number);

            // Get second argument
            std::getline(sstream, word, ' ');
        }

        // Command ID is not valid, needs at least one type and one digit
        if(word.size() < 2){
            return is;
        }

        // Remove leading zeros from command number
        command_number = word.substr(1, word.size() - 1);
        command_number.erase(0, command_number.find_first_not_of('0'));
        gcode.command_id = word.at(0) + command_number;

        // Classify into groups of commands
        bool can_extr = gcode.can_extrude();
        // Extrusion implies pose movement
        bool can_mv_pose = can_extr | gcode.can_mv_pose();
        // Pose movement is necessary for circular pose movement
        bool is_circular = can_mv_pose & gcode.is_circular();
        bool is_home = gcode.is_home();

        // Iterate over remaining arguments
        while (std::getline(sstream, word, ' ')){
            // Split argument into descriptor and value
            char descriptor = word.at(0);
            std::string val = word.substr(1, word.size());

            // Descriptors for commands that can move to a given pose
            if(can_mv_pose){
                // Check for subset that allows extrusion while moving
                if(can_extr){
                    if(descriptor == EXTRUSION_DESCRIPTOR){
                        gcode.extrusion_len = std::stof(val);
                    }
                }

                // Check for subset that moves circularly
                if(is_circular){
                    if(descriptor == CENTER_X_DESCRIPTOR){
                        gcode.center.x = std::stof(val);
                    }
                    else if(descriptor == CENTER_Y_DESCRIPTOR){
                        gcode.center.y = std::stof(val);
                    }
                    else if(descriptor == CENTER_Z_DESCRIPTOR){
                        gcode.center.z = std::stof(val);
                    }
                }

                if(descriptor == FEEDRATE_DESCRIPTOR){
                    gcode.feedrate = std::stof(val);
                }
                else if(descriptor == 'X'){
                    gcode.pose.x = std::stof(val);
                }
                else if(descriptor == 'Y'){
                    gcode.pose.y = std::stof(val);
                }
                else if(descriptor == 'Z'){
                    gcode.pose.z = std::stof(val);
                }
            }

            if(is_home){
                // Search for descriptor in axes and add to axes to home
                std::size_t pos = gcode.axes.find(descriptor);
                if(pos != string::npos){
                    gcode.homing_axes += descriptor;
                }
            }
        }

        if(is_home){
            if(gcode.homing_axes.empty()){
                // Home all axes if none is specified
                gcode.homing_axes = gcode.axes;
            }
        }
    }
    return is;
}