/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 22:51:30 by alappas           #+#    #+#             */
/*   Updated: 2024/05/27 00:39:02 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Headers.hpp"

class Client {
	private:
		std::string _nickname;
		std::string _username;
		bool 		_authorised;

	public:
		Client();
		~Client();
		Client(const Client &other);
		Client &operator=(const Client &other);

		std::string getNickname() const;
		std::string getUsername() const;
		bool getAuthorised() const;
		void setAuthorised(bool authorised);
};

#endif