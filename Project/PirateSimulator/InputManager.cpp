#include "InputManager.h"


using PirateSimulator::InputManager;


InputManager InputManager::singleton;

CDIManipulateur& PirateSimulator::InputManager::Key::m_gestionnaireDeSaisie = InputManager::singleton.getManipulator();


void PirateSimulator::InputManager::Key::update()
{
    lastState = currentState;
    currentState = static_cast<State>(m_gestionnaireDeSaisie.getKey(keyValue));
}

void InputManager::update()
{
    // Prendre en note le statut du clavier
    m_manipulator.StatutClavier();

    // Prendre en note l'�tat de la souris
    m_manipulator.SaisirEtatSouris();

    for(size_t iter = KEY1; iter < NBKEYS; ++iter)
    {
        keys[iter].update();
    }
}