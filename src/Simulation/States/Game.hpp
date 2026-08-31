#pragma once
#include <Engine/Core/StateMachine/State.hpp>
#include <Engine/Core/StateMachine/StateManager.hpp>

class GameState final: public Core::State {
public:
    explicit GameState() {}

    void OnEnter() override {}
    void HandleEvent(const sf::Event& event) override;
    void Update(const float dt) override;
    void Render(sf::RenderTarget& target, float) override;
private:

};