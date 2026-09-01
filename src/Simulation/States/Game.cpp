#include <Simulation/States/Game.hpp>
#include <Simulation/States/Menu/Menu.hpp>

void GameState::OnEnter() {
}

void GameState::HandleEvent(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>();
        key && key->code == sf::Keyboard::Key::Escape) {
        GetStates().Push<MenuState>();
    }
}

void GameState::Update(const float dt) {
    if (!GetContext().Config->first_nest_placed) {
        m_nest_shape.setPosition(GetMousePosition());
        m_nest_shape.setOrigin({m_nest.GetSize(), m_nest.GetSize()});

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            GetContext().Config->first_nest_placed = true;
        }
    }
}

void GameState::Render(sf::RenderTarget& target, float) {
    if (!GetContext().Config->first_nest_placed) {
        m_nest_shape.setFillColor(sf::Color{255, 255, 0 ,125});
    } else m_nest_shape.setFillColor(sf::Color::Yellow);

    m_nest_shape.setRadius(m_nest.GetSize());
    target.draw(m_nest_shape);
}