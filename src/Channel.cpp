/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 15:54:35 by alappas           #+#    #+#             */
/*   Updated: 2024/06/26 16:34:49 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Channel.hpp"

Channel::Channel(std::string name)
: _name(name) {}

Channel::~Channel()
{
    
}

Channel &Channel::operator=(const Channel &other)
{
    if (this == &other)
        return (*this);
    this->_name = other._name;
    this->_client_fds = other._client_fds;
    return (*this);
}

Channel::Channel(const Channel &other)
{
    *this = other;
}

void Channel::addClient(int client_fd)
{
    _client_fds.push_back(client_fd);
}

void Channel::removeClient(int client_fd)
{
    std::vector<int>::iterator it = std::find(_client_fds.begin(), _client_fds.end(), client_fd);
    if (it != _client_fds.end())
        _client_fds.erase(it);
}