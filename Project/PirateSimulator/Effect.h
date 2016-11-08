#ifndef EFFECT_H
#define EFFECT_H


#include "../Chapitre 10/PetitMoteur3D/objet3d.h"
#include "../Chapitre 10/PetitMoteur3D/dispositifD3D11.h"
#include "../Chapitre 10/PetitMoteur3D/d3dx11effect.h"



namespace PirateSimulator
{
    class Effect
    {
    public:
        ID3D11SamplerState* m_sampleState;
        ID3D11Buffer* m_constantBuffer;
        ID3DX11Effect* m_effect;
        ID3DX11EffectTechnique* m_technique;
        ID3DX11EffectPass* m_pass;
        ID3D11InputLayout* m_vertexLayout;


    public:
        Effect() :
            m_sampleState{ nullptr },
            m_constantBuffer{ nullptr },
            m_effect{ nullptr },
            m_technique{ nullptr },
            m_pass{ nullptr },
            m_vertexLayout{ nullptr }
        {}
    };
}


#endif //!EFFECT_H