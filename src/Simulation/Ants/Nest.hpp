#pragma once
#include <vector>

#include "AntData.hpp"

class Nest final {
public:

    bool DetectNearbyAnts();

    const sf::Vector2f GetPosition() const { return m_pos; }
    void SetPosition(const sf::Vector2f& pos) { m_pos = pos; }
    
    const float GetSize() const { return m_size; }

private:
    float m_size = 50.f;
    sf::Vector2f m_pos;
    std::vector<AntData> m_datas;
};