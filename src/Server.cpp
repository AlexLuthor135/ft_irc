/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 16:28:07 by alappas           #+#    #+#             */
/*   Updated: 2024/06/03 18:25:38 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

//Server constuctor
Server::Server(std::string port, std::string password) : _server_fd(0), _epoll_fd(0), _port(port), _password(password),
_ip_to_server(), _client_amount(0){
	createServer();
}

//Server destructor
Server::~Server() {
	close(_server_fd);
	close(_epoll_fd);
}

Server::Server(const Server &other)
    : _server_fd(other._server_fd), _epoll_fd(other._epoll_fd),
	_ip_to_server(other._ip_to_server), _client_amount(other._client_amount){}

Server &Server::operator=(const Server &other) {
    if (this != &other) {
        _server_fd = other._server_fd;
        _epoll_fd = other._epoll_fd;
        _ip_to_server = other._ip_to_server;
		_client_amount = other._client_amount;
    }
    return *this;
}

// Create socket
int Server::createSocket(){
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		std::cerr << "Socket creation failed" << std::endl;
		return (0);
	}
	this->_server_fd = server_fd;
	return (1);
}



// Bind socket
int Server::bindSocket(std::string s_port){
	if (this->_server_fd == -1)
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
	setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	struct sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	if (c_ip != NULL)
		address.sin_addr.s_addr = inet_addr(c_ip);
	else
		address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	address.sin_port = htons(port);
	if (bind(_server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
		std::cerr << "Bind failed" << std::endl;
		return (0);
	}
	return (1);
}


// Create epoll instance
void	Server::createEpoll(){
	int epoll_fd = epoll_create1(0);
	this->_epoll_fd = epoll_fd;
	if (epoll_fd < 0) {
		std::cerr << "Epoll creation failed" << std::endl;
		exit(1);
	}
}

// Listen on socket
int Server::listenSocket(){
	if (listen(_server_fd, SOMAXCONN) == -1) {
		std::cerr << "Listen failed" << std::endl;
		return (0);
	}
	return (1);
}

// Combine file descriptors into epoll instance
int Server::combineFds(int socket_fd){
	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = EPOLLIN;
	event.data.fd = socket_fd;
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1) {
		std::cerr << "Epoll_ctl failed" << std::endl;
		return (0);
	}
	return (1);
}

// Get file descriptors for server
void	Server::createServer(){
	
	std::cout << "Creating server" << std::endl;
	createEpoll();
	if (!checkSocketPort(getPort()) || !createSocket() || !bindSocket(_port)
		|| !listenSocket() || !(setNonBlocking(_server_fd))|| !combineFds(_server_fd))
	{
		std::cerr << "Server creation failed" << std::endl;
		return ;
	}
	std::cout << "Server created on: " << _ip_to_server << std::endl;
	initEvents();
}

void Server::handleIncomingConnection(){
	struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int new_socket = accept(_server_fd, (struct sockaddr *)&address, &addrlen);
    if (new_socket == -1) {
        std::cerr << "Accept failed." << std::endl;
        return;
    }
	if (combineFds(new_socket) == 0)
	{
		if (close(new_socket) == -1)
			std::cerr << "Close failed with error: " << strerror(errno) << std::endl;
		return ;
	}
	_client_fds.push_back(new_socket);
	_client_data[new_socket] = Client();
	setTimeout(new_socket);
    std::cout << "Connection established on IP: " << _ip_to_server[_server_fd] << ", server:" << _server_fd << ", client: " << new_socket << "\n" << std::endl;
	_client_amount++;
}

void Server::handleIncomingData(int client_fd){
	std::string request;
	getRequest(client_fd, request);
	std::vector<std::string> request_lines = split(request, '\n');
	for (std::vector<std::string>::iterator it = request_lines.begin(); it != request_lines.end(); it++)
		std::cout << *it << std::endl;
	Client client = _client_data.find(client_fd)->second;
	if (client.getAuthorised() == false)
		if (!checkConnection(client_fd, request_lines))
			return;
	handleMessages(client_fd, request_lines);
	// std::cout << "Request: " << request << std::endl;
	// int i = 0;
	// for (std::vector<std::string>::iterator it = request_lines.begin(); it != request_lines.end(); it++)
	// {
	// 	std::cout << "Request line #" << ++i << std::endl;
	// 	std::cout << *it << std::endl;
	// }
	// if (!handleResponse(reqStatus, server_fd, client_fd, _client_data.find(client_fd)->second))
	// 	return;
	// if (finish && _cgi_clients.find(client_fd) == _cgi_clients.end())
	// 	deleteClient(client_fd);
}

// int Server::checkClient(int client_fd){
// 	int bytesRead = 0;
// 	std::string _body;
// 	char buffer[4096];
// 	while ((bytesRead = read(client_fd, buffer, sizeof(buffer))) > 0)
// 	{
// 		buffer[bytesRead] = '\0';
// 		_body += buffer;
// 	}
// 	if (bytesRead == -1)
// 	{
// 		if (errno == EAGAIN || errno == EWOULDBLOCK)
// 		{
// 			std::cout << "No more data to read" << std::endl;
// 		}
// 		else
// 		{
// 			std::cerr << "Read failed" << std::endl;
// 			close(client_fd);
// 			return (0);
// 		}
// 	}
// 	std::cout << "Received: " << buffer << std::endl;
// 	return (1);
// }

int Server::setNonBlocking(int fd){
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
	{
		std::cerr << "Fcntl failed" << std::endl;
		return (0);
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "Fcntl failed" << std::endl;
		return (0);
	}
	return (1);
}

// Initialize events that will be handled by epoll
void Server::initEvents(){
	while (1){
		try{
			struct epoll_event events[_client_amount + 1];
			int n = epoll_wait(this->_epoll_fd, events, _client_amount + 1, 1000);
			if (n == -1) {
				std::cerr << "Epoll_wait failed" << std::endl;
				return ;
			}
			for (int i = 0; i < n; i++) {
				bool server = false;
				if (events[i].data.fd == _server_fd) {
					std::cout << "\nIncoming connection on server." << std::endl;
					handleIncomingConnection();
					server = true;
					break ;
				}
				if (!server && events[i].events & EPOLLIN) {
					std::cout << "\nIncoming data on client: " << events[i].data.fd << std::endl;
					if (std::find((_client_fds.begin()), (_client_fds.end()), events[i].data.fd) != _client_fds.end())
					{
						// setTimeout(events[i].data.fd);
						handleIncomingData(events[i].data.fd);
					}
				}
			}
		} catch (std::exception &e){
			std::cerr << e.what() << std::endl;
		}
		// checkClientTimeout();
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

void Server::setTimeout(int client_fd){
	_client_time[client_fd] = time(NULL);
}

void Server::checkClientTimeout(){
	time_t current_time = time(NULL);
	for (std::map<int, time_t>::iterator it = _client_time.begin(); it != _client_time.end(); it++)
	{
		if (current_time - it->second > 30)
		{
			std::cout << "Client FD: " << it->first << " timed out\n";
			// deleteClient(it->first);
		}
	}
}

std::vector<std::string> Server::split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

bool Server::checkConnection(int client_fd, std::vector<std::string> &request)
{
	std::string pass;
	for (std::vector<std::string>::iterator it = request.begin(); it != request.end(); it++)
	{
		if (it->compare(0, 4, "PASS") == 0)
		{
			pass = it->substr(5);
			pass = std::string(pass.c_str());
			if (!pass.empty() && pass[pass.size() - 1] == '\r')
    			pass.erase(pass.size() - 1);
			// std::cout << "Password: ";
			// for (unsigned char c : pass)
    		// 	std::cout << static_cast<int>(c) << ' ';
			// std::cout << std::endl;

			// std::cout << "Real password: ";
			// for (unsigned char c : _password)
    		// 	std::cout << static_cast<int>(c) << ' ';
			std::cout << std::endl;
			if (pass == _password)
			{
				std::cout << "Client: " << client_fd << " connected successfully" << std::endl;
				_client_data.find(client_fd)->second.setAuthorised(true);
				return true;
			}
			else
				return (deleteClient(client_fd), false);
		}
	}
	return false;
}

void	Server::handleMessages(int client_fd, std::vector<std::string> &request){
	std::string response;
	for (std::vector<std::string>::iterator it = request.begin(); it != request.end(); it++)
	{
		// if (it->compare(0, 4, "NICK") == 0)
		// 	handleNick(client_fd, it->substr(5));
		if (it->compare(0, 4, "QUIT") == 0)
			handleQuit(client_fd);
		if (it->compare(0, 4, "JOIN") == 0)
		{
			// std::string channel = it->substr(5);
			// if (!channel.empty() && channel[channel.size() - 1] == '\r')
			// 	channel.erase(channel.size() - 1);
			// std::cout << "Client: " << client_fd << " joined channel: " << channel << std::endl;
		}
	}
}

void	Server::deleteClient(int client_fd){
	if (std::find(_client_fds.begin(), _client_fds.end(), client_fd) != _client_fds.end())
	{
		if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == -1)
			std::cerr << "Epoll_ctl failed with error: " << strerror(errno) << std::endl;
		_client_fds.erase(std::find(_client_fds.begin(), _client_fds.end(), client_fd));
		if (close(client_fd) == -1)
			std::cerr << "Close failed with error: " << strerror(errno) << std::endl;
	}
}

void	Server::handleQuit(int client_fd)
{
	std::cout << "Client: " << client_fd << " disconnected" << std::endl;
	return (deleteClient(client_fd));
}

// void	Server::handleNick(int client_fd, std::string nick)
// {
// 	std::vector<std::string> client = std::
// 	// if (nick.size() == 0)
// 	// {
		
// 	// }
	
// 	std::cout << "Client: " << client_fd << " changed nick to: " << nick << std::endl;
// 	_client_data.find(client_fd)->second.setNick(nick);
// }