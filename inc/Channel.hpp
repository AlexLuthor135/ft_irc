/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 15:54:15 by alappas           #+#    #+#             */
/*   Updated: 2024/06/26 15:59:26 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Headers.hpp"

#pragma once

class Channel{

private:
    std::string _name;
    std::vector<int> _client_fds;

public:
    Channel(std::string name);
    ~Channel();
    Channel(const Channel &other);
    Channel &operator=(const Channel &other);

    void addClient(int client_fd);
    void removeClient(int client_fd);
};