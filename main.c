#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

// This is an error-handling function that prints error messages
// and exits the program when something goes wrong.

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// checks if the user provided a URL as a command-line argument.
// If not, it displays a usage message and quits.


int main(int argc, char *argv[]) {
    // The first parameter, argc (argument count) is an integer that indicates how many arguments
    // were entered on the command line when the program was started. 
    // The second parameter, argv (argument vector), is an array of pointers to arrays of character objects.


    // I'm comparing it to the number two because The program expects to be run with one command-line argument,
    // which is the URL. When argc is not equal to 2, it means that either no arguments were provided 
    // or more than one argument was provided. In either case, it's considered incorrect usage.

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <URL>\n", argv[0]);
        exit(1);
    }

    // This line stores the URL provided by the user in the url variable.
    char *url = argv[1];

    // Here, two arrays, host and path, are declared to store the hostname and 
    // path extracted from the URL. The default port for HTTP is set to 80.

    char host[100], path[100];
    int port = 80;


    // This line checks if the provided URL starts with "http://," and if it does, 
    // it extracts the host and path. If the URL is not in the expected format, 
    // it prints an error message and exits.

    // By checking if sscanf doesn't return 2 (i.e., != 2), the code is ensuring that both the
    //  host and path have been successfully assigned values according to the format string. 
    // If sscanf didn't find and assign values to both host and path, it indicates that the URL doesn't 
    // match the expected format, and the program prints an error message and exits.

    if (sscanf(url, "http://%99[^/]/%99[^\n]", host, path) != 2) {
        fprintf(stderr, "Invalid URL format.\n");
        exit(1);
    }

    // This line resolves the hostname to an IP address using
    // the gethostbyname and stores the result in the server variable.

    struct hostent *server = gethostbyname(host);

    if (server == NULL) {
        error("Error: Unable to resolve host");
    }


    // Setting network connection details, just like in socket chat project,
    // such as the IP address and port, in a struct sockaddr_in named server_addr.

    struct sockaddr_in server_addr;
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);

    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error: Unable to open socket");
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Error: Unable to connect");
    }

    // Send an HTTP GET request
    char request[200];
    snprintf(request, sizeof(request), "GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);

    if (write(sockfd, request, strlen(request)) < 0) {
        error("Error: Unable to write to socket");
    }

    // Read and print the response
    char buffer[1024];
    int n;
    while ((n = read(sockfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);
    }

    if (n < 0) {
        error("Error: Unable to read from socket");
    }

    // Close the socket
    close(sockfd);

    return 0;
}
