#pragma once

#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>

#include <array>
#include <cstddef>
#include <cstdint>

namespace Sim {

// Unite du monde : 1 unite = 1 mm. Une ouvriere fait ~11 mm de long.
// Repere local d'une fourmi : +X = vers l'avant (tete), Y = lateral.

inline constexpr std::size_t LegCount = 6;

// Ordre des pattes :
// 0 AvantGauche, 1 AvantDroite, 2 MilieuGauche, 3 MilieuDroite, 4 ArriereGauche, 5 ArriereDroite
inline constexpr std::size_t LegFrontLeft  = 0;
inline constexpr std::size_t LegFrontRight = 1;
inline constexpr std::size_t LegMidLeft    = 2;
inline constexpr std::size_t LegMidRight   = 3;
inline constexpr std::size_t LegRearLeft   = 4;
inline constexpr std::size_t LegRearRight  = 5;

enum class AntState : std::uint8_t {
    Idle,
    Wandering,      // sortie de la fourmiliere, exploration
    SeekingFood,    // suit une piste de pheromones de nourriture
    ReturningHome   // rentre au nid, depose de la pheromone
};

// Constantes partagees par toutes les fourmis d'une meme caste.
// Une seule instance vit dans le monde ; les fourmis y referent, elles ne la copient pas.
struct AntBodyPlan {
    // Segments du corps, exprimes en coordonnees locales (ellipses)
    float HeadOffset = 3.2f;
    sf::Vector2f HeadRadius{1.3f, 1.1f};

    float ThoraxOffset = 0.0f;
    sf::Vector2f ThoraxRadius{1.9f, 1.1f};

    float AbdomenOffset = -3.8f;
    sf::Vector2f AbdomenRadius{2.6f, 1.9f};

    // Antennes
    float AntennaLength = 3.0f;
    sf::Angle AntennaSpread = sf::degrees(35.0f);
    sf::Angle AntennaSwing = sf::degrees(18.0f);   // amplitude du balayage
    float AntennaFrequency = 6.0f;                 // Hz

    // Pattes : deux segments (femur + tibia) resolus en IK a deux os
    float FemurLength = 3.0f;
    float TibiaLength = 3.4f;

    std::array<sf::Vector2f, LegCount> HipLocal{};    // attache sur le thorax
    std::array<sf::Vector2f, LegCount> RestLocal{};   // position ideale du pied
    std::array<std::uint8_t, LegCount> TripodGroup{}; // 0 ou 1 : les deux tripodes

    // Parametres de demarche (utilises a l'etape 2)
    float StepDuration = 0.10f;          // duree d'un pas, en secondes
    float StepTriggerDistance = 2.6f;    // ecart pied/repos qui declenche un pas
    float StepOvershoot = 1.25f;         // le pied vise au-dela du repos, dans le sens du deplacement

    static AntBodyPlan Worker();
};

// Etat d'animation propre a chaque fourmi. Aucune logique de simulation ici.
struct AntBody {
    std::array<sf::Vector2f, LegCount> FootWorld{};    // pied pose, en coordonnees monde
    std::array<sf::Vector2f, LegCount> StepOrigin{};   // d'ou part le pas en cours
    std::array<sf::Vector2f, LegCount> StepTarget{};   // ou il arrive
    std::array<float, LegCount> StepProgress{};        // 1 = pied pose, [0,1) = pas en cours

    sf::Angle HeadAngle{};      // relatif au thorax
    sf::Angle AbdomenAngle{};   // relatif au thorax, traine dans les virages
    float AntennaPhase = 0.0f;
};

class Ant {
public:
    Ant() = default;
    Ant(sf::Vector2f StartPosition, sf::Angle StartHeading, int NestId);

    // Integration du mouvement seule : oriente vers DesiredHeading en respectant TurnRate,
    // puis avance. L'IA (etape 3) se contente d'ecrire DesiredHeading, Speed et State.
    void Update(float DeltaTime);

    // Pose les six pieds a leur position de repos. A appeler au spawn.
    void PlantFeet(const AntBodyPlan& Plan);

    sf::Vector2f LocalToWorld(sf::Vector2f Local) const;
    sf::Vector2f WorldToLocal(sf::Vector2f World) const;
    sf::Vector2f Forward() const;

    // --- Etat de simulation ---
    sf::Vector2f Position{};
    sf::Angle Heading{};
    sf::Angle DesiredHeading{};
    float Speed = 0.0f;          // mm/s, vitesse courante
    float MaxSpeed = 45.0f;      // mm/s
    float TurnRate = 6.0f;       // rad/s

    AntState State = AntState::Wandering;
    int NestId = -1;
    bool HasFood = false;
    float Age = 0.0f;

    // Reserve pour l'etape pheromones
    float PheromoneCooldown = 0.0f;

    // --- Etat visuel ---
    AntBody Body{};
};

} // namespace Sim
