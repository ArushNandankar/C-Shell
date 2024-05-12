#include "headers.h"

void execute_man(char *command)
{
    // Initialize hints struct
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // Allow both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM; // Use TCP

    // Perform DNS resolution for "man.he.net"
    int status = getaddrinfo("man.he.net", "80", &hints, &res); // Specify port "80" for HTTP
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return;
    }

    // Open a TCP socket to the IP address
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0)
    {
        perror("Socket error");
        freeaddrinfo(res);
        return;
    }

    // Connect to the server
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0)
    {
        perror("Connect error");
        close(sockfd);
        freeaddrinfo(res);
        return;
    }

    // Send an HTTP GET request
    char get_request[1024];

    sprintf(get_request, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\nConnection: close\r\n\r\n", command);

    if (send(sockfd, get_request, strlen(get_request), 0) < 0)
    {
        perror("Send error");
        close(sockfd);
        freeaddrinfo(res);
        return;
    }


    char output[16384];
    int oidx = 0;
    // Read and print the response from the server
    char buffer[1024];
    int bytes_received;
    int count = 0;

    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytes_received] = '\0';
        // dont print anything inside < > including < >
        
        int i = 0;
        while (buffer[i] != '\0')
        {
            if (buffer[i] == '<')
            {
                while (buffer[i] != '>')
                {
                    i++;
                }
            }
            else
            {
                if(buffer[i] != '>' && buffer[i] != '<')
                    if(buffer[i] == '\n')
                        count++;

                if(count > 15)
                {
                    if(buffer[i] != '>' && buffer[i] != '<')
                        output[oidx++] = buffer[i];    //printf("%c", buffer[i]);

                }
                i++;
            }
        }
        if(strstr(output, "NAME") != NULL)
            printf("%s\n", output);
        else
            fprintf(stderr, "ERROR : no such command\n");
    }

    if (bytes_received < 0)
    {
        perror("Receive error");
    }

    // Close the socket
    close(sockfd);

    // Free the memory allocated for the addrinfo struct
    freeaddrinfo(res);
}
