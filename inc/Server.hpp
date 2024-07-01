/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 16:28:30 by alappas           #+#    #+#             */
/*   Updated: 2024/06/26 16:35:43 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "Headers.hpp"

class Server {
	private:
		int _server_fd;
		int _epoll_fd;
		std::string _port;
		std::string _password;
		std::string _ip_to_server;
		std::vector<int> _client_fds;
		std::map<int, Client> _client_data;
		std::map<int, time_t> _client_time;
		int _client_amount;
	public:
		
		//Constructors
		Server(std::string port, std::string password);
		~Server();
		Server(const Server &other);
		Server& operator=(const Server &other);

		//Member functions
		int		checkSocketPort(std::string port);
		int		createSocket();
		int		bindSocket(std::string port);
		int		listenSocket();
		int		combineFds(int socket_fd);
		void	createEpoll();
		void	createServer();
		void	initEvents();
		int		checkClient(int client_fd);
		bool	getRequest(int client_fd, std::string &request);
		int		setNonBlocking(int fd);
		std::string getPort() const;
		void	setTimeout(int client_fd);
		void	checkClientTimeout();
		void 	handleIncomingConnection();
		void 	handleIncomingData(int client_fd);
		bool 	checkConnection(int client_fd, std::vector<std::string> &request);
		void 	handleMessages(int client_fd, std::vector<std::string> &request);
		void	deleteClient(int client_fd);
		std::vector<std::string> split(const std::string &s, char delim);
		

		void	handleQuit(int client_fd);
		void 	handleJoin(int client_fd, std::vector<std::string> channel);
		void	handleNick(int client_fd, std::string nick);

};

#endif