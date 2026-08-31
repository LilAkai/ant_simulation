#pragma once
#include <vector>

#include "AntData.hpp"

class Nest final {
public:

    bool DetectNearbyAnts();

private:
    float m_size = 10.f;
    std::vector<AntData> m_datas;
};