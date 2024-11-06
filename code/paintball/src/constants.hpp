#ifndef __CONSTANTS_H
#define __CONSTANTS_H

// TODO: arrange into namespaces
constexpr int MainFont = 1;
constexpr int BulletLimit = 100;
constexpr int PlayerCount = 4;
constexpr float SpeedLimit = 80.f;
// Same range as analog input, max value that can be generated by the controller
constexpr float ForceLimit = 60.f;
constexpr float invMass = 10;
constexpr float BulletVelocity = 500;
constexpr float PlayerRadius = 10;

constexpr int Damage = 10;
constexpr int MaxHealth = 100;

constexpr int MapWidth = 1024;
constexpr int MapHeight = 1024;

#endif // __CONSTANTS_H