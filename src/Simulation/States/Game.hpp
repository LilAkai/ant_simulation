#pragma once
#include <Engine/Core/StateMachine/State.hpp>
#include <Engine/Core/StateMachine/StateManager.hpp>
#include <Engine/Core/Context.hpp>

#include <SFML/Graphics/CircleShape.hpp>

#include <Simulation/Ants/Nest.hpp>

class GameState final: public Core::State {
public:
    explicit GameState() {}

    void OnEnter() override;
    void HandleEvent(const sf::Event& event) override;
    void Update(const float dt) override;
    void Render(sf::RenderTarget& target, float) override;

private:
    sf::Vector2f GetMousePosition() const {
        const sf::RenderWindow& window = GetContext().GetWindow();
        return window.mapPixelToCoords(sf::Mouse::getPosition(window));
    }

    sf::CircleShape m_nest_shape;
    Nest m_nest;
};