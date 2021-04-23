#include <string>
#include <sys/socket>

// Program definition
#define MXT_PRG_NO          1       // Program number for the MXT program to be executed

// Command command prefix variables
#define DELIMITER           ";"
#define ROBOT_NO            "1"
#define SLOT_NO             "1"
#define _CMD_PREFIX         ROBOT_NO DELIMITER SLOT_NO DELIMITER

// Macro to append the common command prefix to a new command
#define CREATE_CMD(CMD)     (_CMD_PREFIX CMD)

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
#define CMD_READ_VAR        CREATE_CMD("VAL")

// Robot variables
#define VAR_SRV_STATE       "M_SVO"     // State variable for the servos (1 = on, 0 = off)

// Response flags
#define STATUS_OK           "QoK"       // The command executed successfully
#define STATUS_MINOR_ISSUE  "Qok"
#define STATUS_ERROR        "QeR"       // Meaning?
#define STATUS_ERROR2       "Qer"

// Function declarations
int open_connection(std::string host, unsigned char port);  // Returns a TCP/IP-socket handle if successful
int send_command(int sock, std::string cmd);                // Sends an R3-command via a given socket
int receive_response(int sock, std::string &response);      // Receives an R3-respinse via a given socket

int open_connection(std::string host, unsigned char port){
    int sock;
    int ret;
    struct sockaddr_in transmit_add;
    transmit_add.sin_family = AF_INET;
    transmit_add.sin_port = htons(port);
    transmit_addr.sin_addr.s_addr = inet_addr(host);

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        std::cout << "Could not create a new socket." << std::endl;
        return -1;
    }

    // Open connection
    ret = connect(sock, (sockaddr*)&transmit_addr, sizeof(transmit_addr);
    if(ret == -1){
        std::cout << "Could not open the socket." << std::endl;
        return -1;
    }
    std::cout << "Opened connection to " << host << ":" << port << std::endl;
    return sock;
}

int send_command(int sock, std::string cmd){
    ssize_t total_sent_bytes = 0;
    ssize_t sent_bytes = 0;
    void *data = static_cast<void*>(cmd);

    // Length of commands is limited
    if(cmd.length() > 127)
        return -1;

    // Ensure that all bytes of the command are sent
    while(total_sent_bytes < cmd.length){
        // Offset the send pointer by the number of bytes sent so far
        sent_bytes = send(sock, data + total_sent_bytes, sizeof(data) - total_sent_bytes, 0);
        if(sent_bytes == -1){
            std::cout << "Error while sending via TCP/IP (Socket:" << sock << ")." << std::endl;
            return -1;        
        }
        total_sent_bytes += sent_bytes;    
    }
    return 0;
}

int receive_response(int sock, std::string &response){
    ssize_t recv_bytes;
    char recv_buf[4096];
    std::string status;

    recv_bytes = recv(sock, (void*) &recv_buf, sizeof(recv_buf), 0);
    if(recv_bytes == -1){
        std::cout << "Error while receiving via TCP/IP (Socket:" << sock << ")." << std::endl;
        return -1;     
    }

    if(recv_bytes < 3){
        std::cout << "Received less than three bytes via TCP/IP (Socket:" << sock << ")." << std::endl;
        return -1;          
    }

    // Take the first three characters representing the status
    status = string(recv_buf).substr(0,3);
    if(recv_bytes > 3){
        response = string(recv_buf).substr(3, recv_bytes - 3);        
    }
    else{
        response = "";
    }
    

    // Evaluate the return status
    if(status == STATUS_OK){
        return 0;
    }
    else{
        return -1;
    }
        
}

void start_robot(int sock){
    bool servo_state;
    string response;
    int ret;

    // Open the communication
    send_command(sock, CMD_OPEN_COM);
    ret = receive_response(sock, response);
    if(ret == -1){
        std::cout << "Failed to open communication " << response << std::endl;
    }

    // Obtain control to start the servos
    send_command(sock, CMD_CTRL_ON);
    ret = receive_response(sock, response);
    if(ret == -1){
        std::cout << "Failed to obtain control " << response << std::endl;
    }

    // Start the servos
    send_command(sock, CMD_SRV_ON);
    ret = receive_response(sock, response);
    if(ret == -1){
        std::cout << "Failed to activate the servos " << response << std::endl;
    }
 
    // Poll the servo state
    do{
        // Send the command to read the servo variable
        send_command(sock, CMD_READ_VAR + VAR_SRV_STATE);
        ret = receive_response(sock, response);
        if(ret == -1){
            std::cout << "Failed to read the servo state " << response << std::endl;
        }
        // Receive the response
        servo_state = response.back() - '0';
    }while(!servo_state)
}

void stop_robot(int sock){
    string response;
    int ret;

    // Do the initialization in reverse order
    // Switch off the servos
    send_command(sock, CMD_SRV_OFF);
    ret = receive_response(sock, response);
    if(ret == -1){
        std::cout << "Failed to deactivate the servos " << response << std::endl;
    }

    // Release control rights
    send_command(sock, CMD_CTRL_OFF);
    ret = receive_response(sock, response);
    if(ret == -1){
        std::cout << "Failed to release control " << response << std::endl;
    }

    // Close the communication
    send_command(sock, CMD_CLOSE_COM);
    ret = receive_response(sock, response);
    if(ret == -1){
        std::cout << "Failed to close communication " << response << std::endl;
    }

    // Close the TCP-socket
    close(sock);
}