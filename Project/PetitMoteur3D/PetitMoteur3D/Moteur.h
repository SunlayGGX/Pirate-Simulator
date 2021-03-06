﻿#pragma once
#include "Singleton.h"
#include "dispositif.h" 

#include "GestionnaireDeTextures.h"
#include "DIManipulateur.h"
#include "PanneauPE.h"
#include "../../PirateSimulator/GameFabric.h"
#include "../../PirateSimulator/GameLogic.h"

// Manager
#include "../../PirateSimulator/TimeManager.h"
#include "../../PirateSimulator/RendererManager.h"
#include "../../PirateSimulator/InputManager.h"
#include "../../PirateSimulator/TaskManager.h"
#include "../../PirateSimulator/PhysicsManager.h"

//UI
#include "../../PirateSimulator/UIMainMenuLogic.h"
#include "../../PirateSimulator/UIPauseLogic.h"

#include "../../PirateSimulator/Transform.h"

#include <thread>
#include <vector>
#include "../../PirateSimulator/GameManager.h"


namespace PM3D
{
    const int IMAGESPARSECONDE = 60;

    //
    //   TEMPLATE : CMoteur
    //
    //   BUT : Template servant à construire un objet Moteur qui implantera les
    //         aspects "génériques" du moteur de jeu
    //
    //   COMMENTAIRES :
    //
    //        Comme plusieurs de nos objets représenteront des éléments uniques 
    //		  du système (ex: le moteur lui-même, le lien vers 
    //        le dispositif Direct3D), l'utilisation d'un singleton 
    //        nous simplifiera plusieurs aspects.
    //
    template <class T, class TClasseDispositif>
    class CMoteur : public CSingleton<T>
    {
    public:

        virtual void Run()
        {
            using PirateSimulator::UIPauseLogic;


            std::unique_ptr<CPanneauPE> pPanneauPE = std::make_unique<CPanneauPE>();

            PirateSimulator::GameLogic().startGameMusic();

            bool bBoucle = true;
            PirateSimulator::GameManager *gameManager = PirateSimulator::GameManager::getInstance();

            while(gameManager->getGameState() != PirateSimulator::GameState::Quitting && bBoucle)
            {
                // Propre à la plateforme - (Conditions d'arrêt, interface, messages)
                bBoucle = RunSpecific();

                CDIManipulateur& input = PirateSimulator::InputManager::singleton.getManipulator();
                if(input.getButtonDown(DIK_ESCAPE))
                {
                    gameManager->pause();
                }

                if(gameManager->getGameState() == PirateSimulator::GameState::Pause)
                {
                    // Get the Input
                    PirateSimulator::InputManager::singleton.update();

                    // Render
                    auto pDispositif = PirateSimulator::RendererManager::singleton.getDispositif();
                    ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
                    ID3D11RenderTargetView* pRenderTargetView = pDispositif->GetRenderTargetView();

                    float Couleur[4] = {0.0f, 0.5f, 0.0f, 1.0f};  //  RGBA - Vert pour le moment
                    pImmediateContext->ClearRenderTargetView(pRenderTargetView, Couleur);

                    ID3D11DepthStencilView* pDepthStencilView = pDispositif->GetDepthStencilView();
                    pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

                    BeginRenderSceneSpecific();

                    pPanneauPE->DebutPostEffect();

                    // Scene sur surface de rendu POST-EFFECT
                    BeginRenderSceneSpecific();

                    PirateSimulator::RendererManager::singleton.draw();

                    EndRenderSceneSpecific();

                    // Display the pause

                    gameManager->update();
                    //pauseMenu();
                    //Unbind RenderTarget because it can`t be set to both a texture to a shader and a render target view...
                    ID3D11RenderTargetView* pNullRTV = NULL;
                    pImmediateContext->OMSetRenderTargets(1, &pNullRTV, NULL);

                    if(pNullRTV)
                    {
                        pNullRTV->Release();
                    }

                    pPanneauPE->FinPostEffect();


                    pPanneauPE->Draw();


                    EndRenderSceneSpecific();


                    pDispositif->Present();

                    std::this_thread::sleep_for(50ms);
                }
                else
                {
                    PirateSimulator::TaskManager::GetInstance().update();

                    if (gameManager->getGameState() == PirateSimulator::GameState::PartyFinished)
                    {
                        //PirateSimulator::GameLogic().stopGameMusic(); //buggy

                        while (gameManager->getGameState() != PirateSimulator::GameState::InGame)
                        {
                            // Update the window
                            if (!RunSpecific())
                            {
                                return;
                            }
                            // Get the Input
                            PirateSimulator::InputManager::singleton.update();

                            // Render
                            auto pDispositif = PirateSimulator::RendererManager::singleton.getDispositif();
                            ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
                            ID3D11RenderTargetView* pRenderTargetView = pDispositif->GetRenderTargetView();

                            float Couleur[4] = { 0.0f, 0.5f, 0.0f, 1.0f };  //  RGBA - Vert pour le moment
                            pImmediateContext->ClearRenderTargetView(pRenderTargetView, Couleur);

                            ID3D11DepthStencilView* pDepthStencilView = pDispositif->GetDepthStencilView();
                            pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
                            // Display the mainMenu
                            if (gameManager->getGameState() == PirateSimulator::GameState::Loading)
                            {
                                gameManager->setGameState(PirateSimulator::GameState::InGame);
                                //PirateSimulator::GameLogic().restartGameMusic(); //buggy
                                TimeManager::GetInstance().startGameTime();
                                break;
                            }
                            gameManager->update();
                            pDispositif->Present();

                            //exit via title screen menu
                            if (gameManager->getGameState() == PirateSimulator::GameState::Quitting)
                            {
                                PirateSimulator::GameLogic().killEveryMusicFlow();

                                PirateSimulator::GameLogic::cleanAllTasks();

                                return;
                            }
                        }
                    }
                }
            }

            // Exit the game loop so cleanup
            PirateSimulator::GameLogic::cleanAllTasks();
        }

        virtual int Initialisations()
        {
            // Propre à la plateforme
            InitialisationsSpecific();

            // Création des tasks
            PirateSimulator::GameLogic mainThreadGameLogic;
            mainThreadGameLogic.createAllTask();
            mainThreadGameLogic.startTitleScreenMusic();
            PirateSimulator::GameManager::getInstance()->initialize();

            bool resultInit = false;

            std::vector<std::thread> beginThread;

            beginThread.emplace_back([this, &resultInit]() {
                //Loading of some noises
                PirateSimulator::GameLogic().loadMusics();

                // * Initialisation de la scène
                InitScene();
                resultInit = true;
            });

            // Create the HUD here (to prevent thread bug (it call the same pDispositif as the menu called)
            PirateSimulator::GameFabric().createHUD();
			
            PirateSimulator::GameManager *gameManager = PirateSimulator::GameManager::getInstance();
            const PirateSimulator::GameState &gameState = PirateSimulator::GameManager::getInstance()->getGameState();
            while(gameManager->getGameState() != PirateSimulator::GameState::InGame)
            {
                // Update the window
                if(!RunSpecific())
                {
                    // Kill all thread if the user closes the window
                    beginThread.front().detach();
                    return 1;
                }
                // Get the Input
                PirateSimulator::InputManager::singleton.update();

                // Render
                auto pDispositif = PirateSimulator::RendererManager::singleton.getDispositif();
                ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
                ID3D11RenderTargetView* pRenderTargetView = pDispositif->GetRenderTargetView();

                float Couleur[4] = {0.0f, 0.5f, 0.0f, 1.0f};  //  RGBA - Vert pour le moment
                pImmediateContext->ClearRenderTargetView(pRenderTargetView, Couleur);

                ID3D11DepthStencilView* pDepthStencilView = pDispositif->GetDepthStencilView();
                pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
                // Display the mainMenu
                if(gameState == PirateSimulator::GameState::Loading && resultInit)
                {
                    gameManager->setGameState(PirateSimulator::GameState::InGame);
                    break;
                }
                gameManager->update();
                pDispositif->Present();

                std::this_thread::sleep_for(5ms);

                //exit via title screen menu
                if (gameManager->getGameState() == PirateSimulator::GameState::Quitting)
                {
                    PirateSimulator::GameLogic().killEveryMusicFlow();
                    for (size_t iter = 0; iter < beginThread.size(); ++iter)
                    {
                        beginThread[iter].join();
                    }

                    PirateSimulator::GameLogic::cleanAllTasks();

                    return 1;
                }
            }
            
            // Start the game time when all menu and loading screen are close to begin the game
            TimeManager::GetInstance().startGameTime();

            for(size_t iter = 0; iter < beginThread.size(); ++iter)
            {
                beginThread[iter].join();
            }

            return 0;
        }

        CGestionnaireDeTextures& GetTextureManager()
        {
            return TexturesManager;
        }

    protected:

        // Constructeur par défaut
        CMoteur(void)
        {}

        // Destructeur
        ~CMoteur(void)
        {
            Cleanup();
        }

        // Spécifiques - Doivent être implantés
        virtual bool RunSpecific() = 0;
        virtual int InitialisationsSpecific() = 0;
        virtual __int64 GetTimeSpecific() = 0;
        //virtual TClasseDispositif* CreationDispositifSpecific(const CDS_MODE cdsMode) = 0;
        virtual void BeginRenderSceneSpecific() = 0;
        virtual void EndRenderSceneSpecific() = 0;

        virtual void Cleanup()
        {
            // Vider les textures
            TexturesManager.Cleanup();
        }

        virtual int InitScene()
        {
            PirateSimulator::RendererManager::singleton.setSortingMesh(true);
            PirateSimulator::RendererManager::singleton.setDetailLevel(PirateSimulator::RendererManager::DEEP_ARRANGEMENT);

            // Initialisation des objets 3D - création et/ou chargement 
            if(!InitObjets()) return 1;

            return 0;
        }

        bool InitObjets()
        {
            using PirateSimulator::GameFabric;
            using PirateSimulator::Transform;

            // Initialisation des matrices View et Proj
            // Dans notre cas, ces matrices sont fixes
            Transform cameraTransform;

            cameraTransform.setPosition({0.f, 0.f, -10.f, 0.f}); //XMVectorSet before
            cameraTransform.setUp({0.f, 1.f, 0.f, 0.f}); //XMVectorSet before
            cameraTransform.setForward({0.f, 0.f, 1.f, 0.f}); //XMVectorSet before

            Transform boatTransform;

            boatTransform.setPosition(950.0f, 0.0f, 900.0f);
            boatTransform.setUp({0.0f, 1.0f, 0.0f, 0.0f});
            boatTransform.setForward({0.0f, 0.0f, -1.0f, 0.0f});

            Transform transformCrystal;

            transformCrystal.setPosition(1050.0f, 30.f, 1125.0f);
            transformCrystal.setUp({ 0.0f, 1.0f, 0.0f, 0.0f });
            transformCrystal.setForward({ 0.0f, 0.0f, -1.0f, 0.0f });

            Transform transformTunnel;

            transformTunnel.setPosition(150.f, 0.f, 1050.f);
            transformTunnel.setUp({0.0f, 1.0f, 0.0f, 0.0f});
            transformTunnel.setForward({0.0f, 0.0f, -1.0f, 0.0f});

            Transform TransformTerrain;

            TransformTerrain.setPosition(0.0f, 0.0f, 0.0f);
            TransformTerrain.setUp({0.0f, 1.0f, 0.0f, 0.0f});
            TransformTerrain.setForward({0.0f, 0.0f, -1.0f, 0.0f});


            GameFabric gameFabric;

            // Camera
            gameFabric.createCameraAndBoat(cameraTransform, boatTransform);

            //// Skybox
            gameFabric.createSkybox();

            //create our pieces
            gameFabric.createPieces();

            // Add our water plane
            gameFabric.createWater(TransformTerrain);

            // Add Tunnel
            gameFabric.createTunnel(transformTunnel);

            // Add Tunnel
            gameFabric.createCrystal(transformCrystal);

            // Add our terrain
            gameFabric.createField(TransformTerrain);


            return true;
        }


    protected:
        // Le gestionnaire de texture
        CGestionnaireDeTextures TexturesManager;
    };

} // namespace PM3D
