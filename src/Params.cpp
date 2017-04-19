/*
 * params.cpp
 *
 *  Created on: 19.04.2017
 *      Author: Christian
 */

#include "Params.h"

#include <cstdlib>
#include <iostream>

const char* P_INSTANCE;
int P_IP_TIMEOUT = 60;
int P_IP_THREADS = 2;

#define _CHCK(i,j) {if(i <= j) {std::cout << "Missing argument" << std::endl; exit(1);}}

void printParams()
{
    std::cout << "----- Parameters: -----" << std::endl;
    std::cout << "Instance: " << P_INSTANCE << std::endl;
    std::cout << "IP-Timeout: " << P_IP_TIMEOUT << std::endl;
    std::cout << "IP-Threads: " << P_IP_THREADS << std::endl;
    std::cout << "-----------------------" << std::endl;
}

void readParams(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        std::string s(argv[i]);
        if (s.compare("-i") == 0)
        {
            i++;
            _CHCK(argc, i);
            P_INSTANCE = argv[i];
        } else if (s.compare("-t") == 0)
        {
            i++;
            _CHCK(argc, i);
            P_IP_TIMEOUT = std::strtol(argv[i], nullptr, 10);
        } else if (s.compare("-T") == 0)
        {
            i++;
            _CHCK(argc, i);
            P_IP_THREADS = std::strtol(argv[i], nullptr, 10);
        } else
        {
            std::cout << "Unknown argument: " << argv[i] << std::endl;
            exit(1);
        }
    }
}
