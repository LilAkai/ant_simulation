#pragma once
#include <Engine/Core/StateMachine/State.hpp>
#include <Engine/Core/StateMachine/StateManager.hpp>
#include <Engine/Core/AssetCache.hpp>
#include <Engine/Core/Context.hpp>

#include "Button.hpp"

#include <SFML/Graphics/Text.hpp>

#include <vector>

class MenuState final: public Core::State {
public:
    MenuState() = default;

    void OnEnter() override;
    void OnExit() override;

    void HandleEvent(const sf::Event& event) override;
    void Update(const float) override;
    void Render(sf::RenderTarget&, const float) override;

private:
    void LayoutButtons();
    sf::Vector2f GetMousePosition() const;

    std::vector<Button> m_buttons;
    Core::Context      *m_context;
    Core::StateManager *m_state_manager;

    sf::Text *m_text;
};
