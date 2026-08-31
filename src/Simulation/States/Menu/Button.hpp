#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

#include <cmath>
#include <cstdint>
#include <functional>
#include <string>
#include <utility>

class Button final {
public:
    Button(std::string name, sf::Vector2f pos, sf::Vector2f size, std::function<void()> on_click):
        m_pos(pos), m_size(size), m_on_click(std::move(on_click)), m_name(std::move(name)) {}

    // --- Réglages de l'animation (modifiables par bouton) -------------------
    float     HoverScale = 1.12f;                  // agrandissement au survol
    float     HoverSpeed = 14.f;                   // plus grand = plus sec
    float     HoverShift = 0.f;                    // décalage horizontal, si tu en veux un
    sf::Color IdleColor{ 220, 220, 220 };
    sf::Color HoverColor{ 255, 205, 110 };

    // --- État -------------------------------------------------------------
    void SetHovered(bool hovered) noexcept { m_hovered = hovered; }
    [[nodiscard]] bool IsHovered() const noexcept { return m_hovered; }

    // Fait glisser l'animation vers sa cible. Lissage exponentiel : le résultat
    // est le même à 30 ou à 240 fps, contrairement à un simple
    // m_hover += (cible - m_hover) * 0.2f qui dépend du nombre de frames.
    void Update(float dt) {
        const float target = m_hovered ? 1.f : 0.f;
        m_hover += (target - m_hover) * (1.f - std::exp(-HoverSpeed * dt));

        if (std::fabs(target - m_hover) < 0.001f) {
            m_hover = target;   // évite de traîner indéfiniment à 0.998
        }
    }

    // Survol et clic, gérés par le bouton lui-même.
    // Renvoie true si CE bouton vient d'être cliqué : l'appelant doit alors
    // arrêter sa boucle, le state ayant pu changer sous ses pieds.
    //
    // La fenêtre sert à mapPixelToCoords : après un redimensionnement, les
    // pixels de l'évènement ne correspondent plus aux coordonnées de la vue.
    [[nodiscard]] bool HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
            m_hovered = Contains(window.mapPixelToCoords(moved->position));
            return false;
        }

        if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (pressed->button == sf::Mouse::Button::Left) {
                m_pressed = Contains(window.mapPixelToCoords(pressed->position));
            }
            return false;
        }

        // Le clic se valide au relâchement, et seulement si l'appui avait
        // commencé sur ce bouton : appuyer ailleurs puis relâcher ici ne
        // déclenche rien, comme dans n'importe quelle interface.
        if (const auto* released = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (released->button != sf::Mouse::Button::Left) {
                return false;
            }
            const bool inside = Contains(window.mapPixelToCoords(released->position));
            const bool clicked = m_pressed && inside;
            m_pressed = false;
            m_hovered = inside;
            if (clicked) {
                OnClick();
            }
            return clicked;
        }

        if (event.is<sf::Event::MouseLeft>()) {
            m_hovered = false;
            m_pressed = false;
        }
        return false;
    }

    void Render(sf::RenderTarget& target, sf::Text& text) const {
        text.setString(m_name);

        const sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.position + bounds.size / 2.f);

        // Courbe en S : démarrage et arrivée adoucis, au lieu d'une pente
        // constante qui donne un mouvement mécanique.
        const float eased = m_hover * m_hover * (3.f - 2.f * m_hover);
        const float scale = 1.f + (HoverScale - 1.f) * eased;

        text.setPosition({ m_pos.x + HoverShift * eased, m_pos.y });
        text.setScale({ scale, scale });
        text.setFillColor(Mix(IdleColor, HoverColor, eased));

        target.draw(text);

        text.setScale({ 1.f, 1.f });   // l'objet est partagé entre tous les boutons
    }

    void SetPosition(sf::Vector2f pos) noexcept { m_pos = pos; }
    [[nodiscard]] sf::Vector2f GetPosition() const noexcept { return m_pos; }
    [[nodiscard]] sf::Vector2f GetSize() const noexcept { return m_size; }
    [[nodiscard]] const std::string& GetName() const noexcept { return m_name; }

    // m_pos est le CENTRE du bouton, la boîte s'étend d'une demi-taille de
    // chaque côté.
    [[nodiscard]] bool Contains(sf::Vector2f point) const noexcept {
        const sf::Vector2f half = m_size / 2.f;
        return point.x >= m_pos.x - half.x && point.x <= m_pos.x + half.x &&
               point.y >= m_pos.y - half.y && point.y <= m_pos.y + half.y;
    }

    void OnClick() const {
        if (m_on_click) {
            m_on_click();
        }
    }

private:
    [[nodiscard]] static sf::Color Mix(sf::Color a, sf::Color b, float t) noexcept {
        const auto channel = [t](std::uint8_t from, std::uint8_t to) {
            return static_cast<std::uint8_t>(static_cast<float>(from) +
                                             (static_cast<float>(to) - static_cast<float>(from)) * t);
        };
        return sf::Color(channel(a.r, b.r), channel(a.g, b.g), channel(a.b, b.b), channel(a.a, b.a));
    }

    sf::Vector2f          m_pos;
    sf::Vector2f          m_size;
    std::function<void()> m_on_click;
    std::string           m_name;

    bool  m_hovered = false;
    bool  m_pressed = false;   // l'appui a commencé sur ce bouton
    float m_hover   = 0.f;     // 0 = au repos, 1 = survolé
};