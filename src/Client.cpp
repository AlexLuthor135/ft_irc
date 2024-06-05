/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 00:30:33 by alappas           #+#    #+#             */
/*   Updated: 2024/05/27 00:38:54 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Headers.hpp"

Client::Client() : _nickname(""), _username(""), _authorised(false) {}

Client::~Client() {}

Client::Client(const Client &other) : _nickname(other._nickname), _username(other._username), _authorised(other._authorised) {}

Client &Client::operator=(const Client &other) {
    if (this != &other) {
        _nickname = other._nickname;
        _username = other._username;
        _authorised = other._authorised;
    }
    return *this;
}

std::string Client::getNickname() const {
    return _nickname;
}

std::string Client::getUsername() const {
    return _username;
}

bool Client::getAuthorised() const {
    return _authorised;
}

void Client::setAuthorised(bool authorised) {
    _authorised = authorised;
}