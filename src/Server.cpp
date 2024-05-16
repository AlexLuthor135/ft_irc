/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 16:28:07 by alappas           #+#    #+#             */
/*   Updated: 2024/05/16 21:28:00 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

//Server constuctor
Server::Server(std::string port, std::string password) : _server_fds(), _port(port), _password(password){
	createServer();
}

//Server destructor
Server::~Server() {
	close(_server_fds);
	close(_epoll_fds);
}

// Create socket
int Server::createSocket(){
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		std::cerr << "Socket creation failed" << std::endl;
		return (0);
	}
	this->_server_fds = server_fd;
	return (1);
}

// Bind socket
int Server::bindSocket(std::string s_port){
	if (this->_server_fds == -1)
	{
		std::cerr << "Socket binding impossible!" << std::endl;
		return (0);
	}
	std::stringstream ss;
	ss << s_port;
	int port;
	std::string ip_string;
	const char *c_ip = NULL;
	port = std::atoi(s_port.c_str());
	_ip_to_server = "127.0.0.1:" + s_port;
	int opt = 1;
	setsockopt(_server_fds, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	struct sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	if (c_ip != NULL)
		address.sin_addr.s_addr = inet_addr(c_ip);
	else
		address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	address.sin_port = htons(port);
	if (bind(_server_fds, (struct sockaddr *)&address, sizeof(address)) == -1) {
		std::cerr << "Bind failed" << std::endl;
		return (0);
	}
	return (1);
}


// Create epoll instance
void	Server::createEpoll(){
	int epoll_fd = epoll_create1(0);
	this->_epoll_fds = epoll_fd;
	if (epoll_fd < 0) {
		std::cerr << "Epoll creation failed" << std::endl;
		exit(1);
	}
}

// Listen on socket
int Server::listenSocket(){
	if (listen(_server_fds, SOMAXCONN) == -1) {
		std::cerr << "Listen failed" << std::endl;
		return (0);
	}
	return (1);
}

// Combine file descriptors into epoll instance
int Server::combineFds(){
	int flags = fcntl(_server_fds, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	if (flags == -1)
	{
		std::cerr << "Fcntl failed" << std::endl;
		return (0);
	}
	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = EPOLLIN;
	event.data.fd = _server_fds;
	if (epoll_ctl(this->_epoll_fds, EPOLL_CTL_ADD, _server_fds, &event) == -1) {
		std::cerr << "Epoll_ctl failed" << std::endl;
		return (0);
	}
	return (1);
}

// Get file descriptors for server
void	Server::createServer(){
	
	std::cout << "Creating server" << std::endl;
	createEpoll();
	if (!checkSocketPort(getPort()) || !createSocket() || !bindSocket(_port) || !listenSocket() || !combineFds())
	{
		std::cerr << "Server creation failed" << std::endl;
		return ;
	}
	std::cout << "Server created on: " << _ip_to_server << std::endl;
	initEvents();
}

// Handle incoming connection from clients
void Server::handleIncomingConnection(int server_fd){
	struct sockaddr_in address;
	// struct timeval timeout;
	std::string request;
    socklen_t addrlen = sizeof(address);
    char ip[INET_ADDRSTRLEN];
    int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (new_socket == -1) {
        std::cerr << "Accept failed." << std::endl;
        return;
    }
	if (inet_ntop(AF_INET, &(address.sin_addr), ip, INET_ADDRSTRLEN) == NULL) {
    	std::cerr << "inet_ntop failed with error: " << strerror(errno) << std::endl;
		close(new_socket);
    	return;
}
    std::cout << "Connection established on IP: " << _ip_to_server[server_fd] << ", server:" << server_fd << "\n" << std::endl;
	int flags = fcntl(new_socket, F_SETFL, FD_CLOEXEC, O_NONBLOCK);//COMMENT THE 0_NONBLOCK LINE IF THE BEHAVIOUR IS UNDEFINED.
	if (flags == -1)
	{
		std::cerr << "Fcntl failed" << std::endl;
		close(new_socket);
		return;
	}

    if (close(new_socket) == -1)
		std::cerr << "Close failed with error: " << strerror(errno) << std::endl;
}

// Initialize events that will be handled by epoll
void Server::initEvents(){
	const int MAX_EVENTS = 10;
	struct epoll_event events[MAX_EVENTS];
	
	while (true){
		try{
			int n = epoll_wait(this->_epoll_fds, events, MAX_EVENTS, -1);
			if (n == -1) {
				std::cerr << "Epoll_wait failed" << std::endl;
				return ;
			}
			for (int i = 0; i < n; i++) {
				if (events[i].data.fd == _server_fds) {
					std::cout << "\nIncoming connection on server: " << _ip_to_server << std::endl;
					handleIncomingConnection(_server_fds);
				}
			}
		} catch (std::exception &e){
			std::cerr << e.what() << std::endl;
		}
	}
}


// Check if port is valid
int Server::checkSocketPort(std::string port){
	for (std::string::iterator it = port.begin(); it != port.end(); it++)
	{
		if (!std::isdigit(*it) || std::atoi(port.c_str()) < 0 || std::atoi(port.c_str()) > 65535)
		{
			std::cerr << "Invalid port" << std::endl;
			return (0);
		}
	}
	return (1);
}

bool Server::getRequest(int client_fd, std::string &request){
	
	char buffer[4096];
	request.clear();
	int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes > 0)
	{
		buffer[bytes] = '\0';
		request.append(buffer, bytes);
	}
	if (bytes == 0)
	{
		return true;
	}
	if (bytes == -1)
	{
		std::cerr << "Recv failed" << std::endl;
		return true;
	}
	return false;
}


std::string Server::getPort() const {
	return (this->_port);
}