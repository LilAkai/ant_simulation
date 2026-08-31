#include <Simulation/States/Game.hpp>

#include <Simulation/States/Pause.hpp>

void GameState::HandleEvent(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>();
        key && key->code == sf::Keyboard::Key::Escape) {
        GetStates().Push<PauseState>();
    }
}

void GameState::Update(const float dt) {

}

void GameState::Render(sf::RenderTarget& target, float) {
    
}