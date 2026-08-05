#pragma once
#include <iostream>
#include <string>

#define TEST_ASSERT(condition, massege)\
    if(!(condition)) {\
        std::cerr << " Ошибка: " << massege << std::endl;\
        return false;\
    }

#define TEST_OK() std::cout << "- Ok " << std::endl

#define TEST_START(name)\
    std::cout << name << std::endl
