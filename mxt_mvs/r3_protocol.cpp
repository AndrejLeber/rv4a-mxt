//
// Created by Patrick on 27.04.2021.
//
#include "r3_protocol.h"
#include <string>
#include <sys/socket.h>
#include <cygwin/in.h>
#include <iostream>
#include <arpa/inet.h>

int open_connection(const std::string &host, unsigned int port) {
    int sock;
    int ret;
    struct sockaddr_in transmit_add{};
    transmit_add.sin_family = AF_INET;
    transmit_add.sin_port = htons(port);
    transmit_add.sin_addr.s_addr = inet_addr(host.c_str());

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cout << "Could not create a new socket." << std::endl;
        return -1;
    }

    // Open connection
    ret = connect(sock, (sockaddr *) &transmit_add, sizeof(transmit_add));
    if (ret == -1) {
        std::cout << "Could not open the socket." << std::endl;
        return -1;
    }
    std::cout << "Opened connection to " << host << ":" << port << std::endl;
    return sock;
}

int send_command(int sock, std::string cmd) {
    ssize_t total_sent_bytes = 0;
    ssize_t sent_bytes = 0;

    // Length of commands is limited
    if (cmd.length() > 127)
        return -1;

    // Ensure that all bytes of the command are sent
    while (total_sent_bytes < cmd.length()) {
        // Offset the send pointer by the number of bytes sent so far
        sent_bytes = send(sock, static_cast<void *>(&cmd + total_sent_bytes), cmd.length() - total_sent_bytes, 0);
        if (sent_bytes == -1) {
            std::cout << "Error while sending via TCP/IP (Socket:" << sock << ")." << std::endl;
            return -1;
        }
        total_sent_bytes += sent_bytes;
    }
    return 0;
}

int receive_response(int sock, std::string &response) {
    ssize_t recv_bytes;
    char recv_buf[4096];
    std::string status;

    recv_bytes = recv(sock, (void *) &recv_buf, sizeof(recv_buf), 0);
    if (recv_bytes == -1) {
        std::cout << "Error while receiving via TCP/IP (Socket:" << sock << ")." << std::endl;
        return -1;
    }

    if (recv_bytes < 3) {
        std::cout << "Received less than three bytes via TCP/IP (Socket:" << sock << ")." << std::endl;
        return -1;
    }

    // Take the first three characters representing the status
    status = std::string(recv_buf).substr(0, 3);
    if (recv_bytes > 3) {
        response = std::string(recv_buf).substr(3, recv_bytes - 3);
    } else {
        response = "";
    }


    // Evaluate the return status
    if (status == STATUS_OK) {
        return 0;
    } else {
        return -1;
    }

}

int start_robot(int sock) {
    bool servo_state;
    std::string response;
    int ret;

    // Open the communication
    send_command(sock, CMD_OPEN_COM);
    ret = receive_response(sock, response);
    if (ret == -1) {
        std::cout << "Failed to open communication " << response << std::endl;
        return -1;
    }

    // Obtain control to start the servos
    send_command(sock, CMD_CTRL_ON);
    ret = receive_response(sock, response);
    if (ret == -1) {
        std::cout << "Failed to obtain control " << response << std::endl;
        return -1;
    }

    // Start the servos
    send_command(sock, CMD_SRV_ON);
    ret = receive_response(sock, response);
    if (ret == -1) {
        std::cout << "Failed to activate the servos " << response << std::endl;
        return -1;
    }

    // Poll the servo state
    do {
        // Send the command to read the servo variable
        send_command(sock, VAR_READ_SRV_STATE);
        ret = receive_response(sock, response);
        if (ret == -1) {
            std::cout << "Failed to read the servo state " << response << std::endl;
            return -1;
        }
        // Receive the response
        servo_state = response.back() - '0';
    } while (!servo_state);

    return 0;
}

void stop_robot(int sock) {
    std::string response;
    int ret;

    // Do the initialization in reverse order
    // Switch off the servos
    send_command(sock, CMD_SRV_OFF);
    ret = receive_response(sock, response);
    if (ret == -1) {
        std::cout << "Failed to deactivate the servos " << response << std::endl;
    }

    // Release control rights
    send_command(sock, CMD_CTRL_OFF);
    ret = receive_response(sock, response);
    if (ret == -1) {
        std::cout << "Failed to release control " << response << std::endl;
    }

    // Close the communication
    send_command(sock, CMD_CLOSE_COM);
    ret = receive_response(sock, response);
    if (ret == -1) {
        std::cout << "Failed to close communication " << response << std::endl;
    }

    // Close the TCP-socket
    //close(sock);
}