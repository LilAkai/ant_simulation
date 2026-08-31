#pragma once
#include <Engine/Core/StateMachine/State.hpp>
#include <Engine/Core/StateMachine/StateManager.hpp>

#include <SFML/Graphics/Text.hpp>

class PauseState final: public Core::State {
    public:
        explicit PauseState() {}
    
        void OnEnter() override {}
        void HandleEvent(const sf::Event& event) override;
        void Update(const float dt) override;
        void Render(sf::RenderTarget& target, float) override;
    private:
        
    };