#include "UIHUD.h"
#include "RendererManager.h"
#include "TimeManager.h"
#include "GameObjectManager.h"
#include "PhysicsManager.h"
#include "GameManager.h"
#include "VehicleShape.h"
#include "../PetitMoteur3D/PetitMoteur3D/AfficheurSprite.h"
#include "../PetitMoteur3D/PetitMoteur3D/AfficheurTexte.h"

#include <Gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

PirateSimulator::UIHUD::UIHUD() : GameObject(Transform(), "HUDGO")
{
    // Create the police
    const FontFamily oFamily(L"Edwardian Script ITC", NULL);
    m_police = new Gdiplus::Font(&oFamily, 40.00, FontStyleBold, UnitPixel);

    // Create afficheur sprite
    m_afficheurSprite = new PM3D::CAfficheurSprite();

    // Add speed text
    m_speed = new PM3D::CAfficheurTexte(250, 50, m_police, Gdiplus::Color(255, 255, 255, 255));
    m_speed->Ecrire(L"0 noeud");

    // Add score text
    m_score = new PM3D::CAfficheurTexte(250, 50, m_police, Gdiplus::Color(255, 255, 255, 255));
    m_score->Ecrire(L"Score: " + std::to_wstring(GameObjectManager::singleton.getPieceAdministrator()->getScore()));

    // Add time text
    m_time = new PM3D::CAfficheurTexte(250, 50, m_police, Gdiplus::Color(255, 255, 255, 255));
    std::chrono::milliseconds timeFromStart = TimeManager::GetInstance().getRemainingStartTime();
    m_time->Ecrire(TimeUtils::timeToString(timeFromStart));

    // Add all text to the afficheur sprite
    m_afficheurSprite->AjouterSpriteTexte(m_speed->GetTextureView(), 840, 770);
    m_afficheurSprite->AjouterSpriteTexte(m_score->GetTextureView(), 0, 50);
    m_afficheurSprite->AjouterSpriteTexte(m_time->GetTextureView(), 430, 50);

    // Add the mesh to our game object
    this->addComponent<PirateSimulator::IMesh>(m_afficheurSprite);
    PirateSimulator::RendererManager::singleton.addAnObligatoryMeshToDrawAtEnd(m_afficheurSprite);
}

void PirateSimulator::UIHUD::anime(float elapsedTime)
{
    // Update time text
    std::chrono::milliseconds timeFromStart = TimeManager::GetInstance().getRemainingStartTime();
    m_time->Ecrire(TimeUtils::timeToString(timeFromStart));

    if (timeFromStart <= 0ms)
    {
        GameManager::getInstance()->setGameState(PirateSimulator::GameState::PartyFinished);
    }

    // Update score text
    m_score->Ecrire(L"Score: " + std::to_wstring(GameObjectManager::singleton.getPieceAdministrator()->getScore()));

    // Update speed text
    int speed = PhysicsManager::singleton.getVehiculeShape()->pxActor()->getLinearVelocity().magnitude()/100;
    if(speed > 99)
    {
        speed = 99;
    }
    m_speed-> Ecrire(L"" +std::to_wstring(speed) + L"noeuds");
}
