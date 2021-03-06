#include "RenderTask.h"

#include "../PetitMoteur3D/PetitMoteur3D/dispositif.h"
#include "RendererManager.h"
#include "../PetitMoteur3D/PetitMoteur3D/AfficheurTexte.h"
#include "../PetitMoteur3D/PetitMoteur3D/MoteurWindows.h"

using namespace PirateSimulator;

void RenderTask::init()
{
    // Init afficheur texte
    PM3D::CAfficheurTexte::Init();

    PirateSimulator::RendererManager& rendererManager = PirateSimulator::RendererManager::singleton;

    rendererManager.init(PM3D::CDS_FENETRE, PM3D::CMoteurWindows::GetInstance().getMainWindowHandle());

    rendererManager.setSortingMesh(true);
    rendererManager.setDetailLevel(PirateSimulator::RendererManager::DEEP_ARRANGEMENT);
}

void RenderTask::update()
{
    // Affichage optimis� 
    RendererManager::singleton.getDispositif()->Present();

    RendererManager::singleton.update();

    auto pDispositif = PirateSimulator::RendererManager::singleton.getDispositif();
    ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
    ID3D11RenderTargetView* pRenderTargetView = pDispositif->GetRenderTargetView();
    
    // On efface la surface de rendu
    float Couleur[4] = {0.0f, 0.5f, 0.0f, 1.0f};  //  RGBA - Vert pour le moment
    pImmediateContext->ClearRenderTargetView(pRenderTargetView, Couleur);

    // On r�-initialise le tampon de profondeur
    ID3D11DepthStencilView* pDepthStencilView = pDispositif->GetDepthStencilView();
    pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // On rend l'image sur la surface de travail 
    // (tampon d'arri�re plan)
    // Appeler les fonctions de dessin de chaque objet de la sc�ne
    PirateSimulator::RendererManager::singleton.draw();
}

void RenderTask::cleanup()
{
    PirateSimulator::RendererManager::singleton.removeAllMesh();
}