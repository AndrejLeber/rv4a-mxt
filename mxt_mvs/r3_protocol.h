#include <string>

// Program definition
#define MXT_PRG_NO          1       // Program number for the MXT program to be executed

// Command command prefix variables
#define DELIMITER           ";"
#define ROBOT_NO            "1"
#define SLOT_NO             "1"
#define _CMD_PREFIX         ROBOT_NO DELIMITER SLOT_NO DELIMITER

// Macro to append the common command prefix to a new command
#define CREATE_CMD(CMD)             (_CMD_PREFIX CMD)
#define CREATE_READ_VAR(VAR)        (_CMD_PREFIX "VAL" VAR)

// R3 protocol commands
#define CMD_SRV_ON          CREATE_CMD("SRVON")             // Command to turn the servos on
#define CMD_SRV_OFF         CREATE_CMD("SRVOFF")            // Command to turn the servos off
#define CMD_CTRL_ON         CREATE_CMD("CNTLON")            // Command to obtain control (necessary for some commands)
#define CMD_CTRL_OFF        CREATE_CMD("CNTLOFF")           // Command to release control
#define CMD_OPEN_COM        CREATE_CMD("OPEN=NARCUSER")     // Open a new connection
#define CMD_CLOSE_COM       CREATE_CMD("CLOSE")             // Close a connection
#define CMD_PRG_LOAD        CREATE_CMD("PRGLOAD=")          // Load a program into the active slot
#define CMD_PRG_RUN         CREATE_CMD("RUN")               // Run the given program number or run the one in the active slot if  omitted
#define CMD_PRG_STOP        CREATE_CMD("STOP")              // Stop the program
#define CMD_PRG_RESET       CREATE_CMD("SLOTINIT")          // Reset the program slot

// Robot variables
#define VAR_READ_SRV_STATE  CREATE_READ_VAR("M_SVO")        // State variable for the servos (1 = on, 0 = off)

// Response flags
#define STATUS_OK           "QoK"       // The command executed successfully
#define STATUS_MINOR_ISSUE  "Qok"
#define STATUS_ERROR        "QeR"       // Meaning?
#define STATUS_ERROR2       "Qer"

// Function declarations
int open_connection(const std::string& host, unsigned int port);   // Returns a TCP/IP-socket handle if successful
int send_command(long sock, std::string cmd);                        // Sends an R3-command via a given socket
int receive_response(long sock, std::string &response);              // Receives an R3-respinse via a given socket
int start_robot(long sock);
void stop_robot(long sock);
