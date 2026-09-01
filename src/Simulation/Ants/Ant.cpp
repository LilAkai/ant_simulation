#include "Ant.hpp"

namespace Sim {

AntBodyPlan AntBodyPlan::Worker() {
    AntBodyPlan Plan;

    Plan.HipLocal = {
        sf::Vector2f{ 1.4f, -0.9f}, sf::Vector2f{ 1.4f,  0.9f},
        sf::Vector2f{ 0.1f, -1.0f}, sf::Vector2f{ 0.1f,  1.0f},
        sf::Vector2f{-1.2f, -0.9f}, sf::Vector2f{-1.2f,  0.9f}
    };

    // Les pattes avant pointent vers l'avant, celles du milieu sur le cote,
    // celles de l'arriere vers l'arriere : c'est ce qui donne la silhouette de fourmi.
    Plan.RestLocal = {
        sf::Vector2f{ 4.4f, -3.6f}, sf::Vector2f{ 4.4f,  3.6f},
        sf::Vector2f{ 0.2f, -4.6f}, sf::Vector2f{ 0.2f,  4.6f},
        sf::Vector2f{-3.8f, -3.8f}, sf::Vector2f{-3.8f,  3.8f}
    };

    // Tripode : avant-gauche + milieu-droite + arriere-gauche bougent ensemble,
    // les trois autres forment le tripode oppose.
    Plan.TripodGroup = {0, 1, 1, 0, 0, 1};

    return Plan;
}

Ant::Ant(sf::Vector2f StartPosition, sf::Angle StartHeading, int NestId)
    : Position(StartPosition),
      Heading(StartHeading),
      DesiredHeading(StartHeading),
      NestId(NestId) {
}

void Ant::Update(float DeltaTime) {
    sf::Angle Delta = (DesiredHeading - Heading).wrapSigned();
    const sf::Angle MaxTurn = sf::radians(TurnRate * DeltaTime);

    if (Delta > MaxTurn) {
        Delta = MaxTurn;
    }
    else if (Delta < -MaxTurn) {
        Delta = -MaxTurn;
    }

    Heading = (Heading + Delta).wrapSigned();

    if (Speed > MaxSpeed) {
        Speed = MaxSpeed;
    }

    Position += sf::Vector2f(Speed * DeltaTime, Heading);
    Age += DeltaTime;

    if (PheromoneCooldown > 0.0f) {
        PheromoneCooldown -= DeltaTime;
    }
}

void Ant::PlantFeet(const AntBodyPlan& Plan) {
    for (std::size_t Index = 0; Index < LegCount; ++Index) {
        const sf::Vector2f FootPosition = LocalToWorld(Plan.RestLocal[Index]);

        Body.FootWorld[Index] = FootPosition;
        Body.StepOrigin[Index] = FootPosition;
        Body.StepTarget[Index] = FootPosition;
        Body.StepProgress[Index] = 1.0f;
    }
}

sf::Vector2f Ant::LocalToWorld(sf::Vector2f Local) const {
    return Position + Local.rotatedBy(Heading);
}

sf::Vector2f Ant::WorldToLocal(sf::Vector2f World) const {
    return (World - Position).rotatedBy(-Heading);
}

sf::Vector2f Ant::Forward() const {
    return sf::Vector2f(1.0f, Heading);
}

} // namespace Sim
