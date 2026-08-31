#include "Menu.hpp"
#include <Simulation/States/Game.hpp>

#include <Engine/Core/Context.hpp>
#include <Engine/Core/Logger.hpp>

namespace {
    constexpr sf::Vector2f ButtonSize{ 260.f, 56.f };
    constexpr float ButtonSpacing = 18.f;
}

void MenuState::OnEnter() {
    m_context = &GetContext();
    m_state_manager = &GetStates();
    m_text = new sf::Text(m_context->GetAssets().GetFont("Common/default"));
    m_text->setCharacterSize(32);

    m_buttons.clear();

    m_buttons.emplace_back("Launch", sf::Vector2f{}, ButtonSize, [this]() {
        m_state_manager->Push<GameState>();
    });

    // m_buttons.emplace_back("Settings", sf::Vector2f{}, ButtonSize, [this]() {
    //     m_state_manager->Push<SettingsState>(m_context);
    // });

    m_buttons.emplace_back("Quit", sf::Vector2f{}, ButtonSize, [this]() {
        m_context->GetWindow().close();
    });

    LayoutButtons();
}
void MenuState::OnExit() {
    delete m_text;
    m_buttons.clear();
}



void MenuState::LayoutButtons() {
    if (m_buttons.empty()) {
        return;
    }

    const sf::Vector2f WindowSize(m_context->GetWindow().getSize());
    const float Count = static_cast<float>(m_buttons.size());

    const float TotalHeight = Count * ButtonSize.y + (Count - 1.f) * ButtonSpacing;
    const float StartY = (WindowSize.y - TotalHeight) * 0.5f;
    const float x = WindowSize.x * 0.5f;

    for (std::size_t i = 0; i < m_buttons.size(); ++i) {
        const float y = StartY + static_cast<float>(i) * (ButtonSize.y + ButtonSpacing)
                      + ButtonSize.y * 0.5f;
        m_buttons[i].SetPosition({ x, y });
    }
}

sf::Vector2f MenuState::GetMousePosition() const {
    const sf::RenderWindow& window = m_context->GetWindow();
    return window.mapPixelToCoords(sf::Mouse::getPosition(window));
}

void MenuState::HandleEvent(const sf::Event& Event) {
    if (const auto* Resized = Event.getIf<sf::Event::Resized>()) {
        m_context->GetWindow().setView(
            sf::View(sf::FloatRect({ 0.f, 0.f }, sf::Vector2f(Resized->size)))
        );
        LayoutButtons();
    }

    if (const auto* Moved = Event.getIf<sf::Event::MouseMoved>()) {
        const sf::Vector2f Mouse = m_context->GetWindow().mapPixelToCoords(Moved->position);
        for (auto& CurrentButton: m_buttons) {
            CurrentButton.SetHovered(CurrentButton.Contains(Mouse));
        }
    }

    if (const auto* Released = Event.getIf<sf::Event::MouseButtonReleased>()) {
        if (Released->button == sf::Mouse::Button::Left) {
            const sf::Vector2f Mouse = m_context->GetWindow().mapPixelToCoords(Released->position);
            for (auto& CurrentButton: m_buttons) {
                if (CurrentButton.Contains(Mouse)) {
                    CurrentButton.OnClick();
                    return;
                }
            }
        }
    }

    for (auto& CurrentButton: m_buttons) {
        CurrentButton.HandleEvent(Event, m_context->GetWindow());
    }
}
void MenuState::Update(const float dt) {
    const sf::Vector2f Mouse = GetMousePosition();

    for (auto& button: m_buttons) {
        button.SetHovered(button.Contains(Mouse));
        button.Update(dt);
    }
}
void MenuState::Render(sf::RenderTarget& target, const float dt) {
    for (const auto& button: m_buttons) {
        button.Render(target, *m_text);
    }
}