/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 16:28:30 by alappas           #+#    #+#             */
/*   Updated: 2024/05/16 21:25:49 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>
# include <vector>
# include <map>
# include <cstring>
# include <istream>
# include <fstream>
# include <sstream>
# include <cstdlib>
# include <unistd.h>
# include <sys/socket.h>
# include <sys/epoll.h>
# include <fcntl.h>
# include <netinet/in.h>
# include <arpa/inet.h>

class Server {
	private:
		int _server_fds;
		int _epoll_fds;
		std::string _port;
		std::string _password;
		std::string _ip_to_server;
	public:
		
		//Constructors
		Server(std::string port, std::string password);
		~Server();

		//Member functions
		int		checkSocketPort(std::string port);
		int		createSocket();
		int		bindSocket(std::string port);
		int		listenSocket();
		int		combineFds();
		void	createEpoll();
		void	createServer();
		void	initEvents();
		bool getRequest(int client_fd, std::string &request);
		std::string getPort() const;

		//Temporal function until we have a completed config file
		void handleIncomingConnection(int server_fd);
};

#endif