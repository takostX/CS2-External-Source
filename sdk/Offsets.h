#pragma once
#include <cstdint>

namespace Offsets {
// client.dll
constexpr uintptr_t dwLocalPlayerController = 0x22EF0B8;
constexpr uintptr_t dwEntityList = 0x24AA0D8;
constexpr uintptr_t dwViewMatrix = 0x230ADE0;

// Class members (Schema)
constexpr uintptr_t m_hPawn = 0x6C4;          // CBasePlayerController
constexpr uintptr_t m_iszPlayerName = 0x6F8;  // CBasePlayerController
constexpr uintptr_t m_iHealth = 0x354;        // C_BaseEntity
constexpr uintptr_t m_iTeamNum = 0x3F3;       // C_BaseEntity
constexpr uintptr_t m_pGameSceneNode = 0x338; // C_BaseEntity
constexpr uintptr_t m_vecAbsOrigin = 0xD0;    // CGameSceneNode
constexpr uintptr_t m_iIDEntIndex = 0x3EAC;   // C_CSPlayerPawn
constexpr uintptr_t m_iShotsFired = 0x270C;   // C_CSPlayerPawn
constexpr uintptr_t m_aimPunchAngle = 0x16CC; // C_CSPlayerPawn
constexpr uintptr_t m_fFlags = 0x3EC;         // C_BaseEntity
} // namespace Offsets
