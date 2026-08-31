#include <Simulation/States/Pause.hpp>

#include <Simulation/States/Game.hpp>

void PauseState::HandleEvent(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>();
        key && key->code == sf::Keyboard::Key::Escape) {
        GetStates().Push<GameState>();
    }
}

void PauseState::Update(const float dt) {

}

void PauseState::Render(sf::RenderTarget& target, float) {
    
}