#ifndef PLANE_H
#define PLANE_H

#include "Material.h"
#include "Effect.h"
#include "Mesh.h"

#include <vector>
#include <DirectXMath.h>

namespace PM3D
{
    class CTexture;
    class CDispositifD3D11;
}

namespace PirateSimulator
{
    class SommetPlane
    {
    public:
        static UINT numElements;
        static D3D11_INPUT_ELEMENT_DESC layout[];


    public:
        DirectX::XMFLOAT3 m_position;
        DirectX::XMFLOAT3 m_normale;
        DirectX::XMFLOAT2 m_coordTex;
        float m_originAngle;


    public:
        SommetPlane() {}
        SommetPlane(
            const DirectX::XMFLOAT3& position, 
            const DirectX::XMFLOAT3& normal, 
            const DirectX::XMFLOAT2& coordinate = DirectX::XMFLOAT2(0.0f, 0.0f),
            float originAngle = 0.f
        ) :
            m_originAngle{ originAngle }
        {
            m_normale = normal;
            m_position = position;
            m_coordTex = coordinate;
        }
    };


    namespace ShaderPlane
    {
        struct ShadersParams
        {
        public:
            DirectX::XMMATRIX matWorldViewProj;	// la matrice totale 
            DirectX::XMMATRIX matWorld;			// matrice de transformation dans le monde 

            DirectX::XMVECTOR vLumiere; 			// la position de la source d'�clairage (Point)
            DirectX::XMVECTOR vCamera; 			// la position de la cam�ra

            DirectX::XMVECTOR vLightPoint1;
            DirectX::XMVECTOR vLightPoint2;
            DirectX::XMVECTOR vLightPoint3;
            DirectX::XMVECTOR vLightPoint4;
            DirectX::XMVECTOR mappedLightPointPower1;

            DirectX::XMVECTOR vLightPoint5;
            DirectX::XMVECTOR vLightPoint6;
            DirectX::XMVECTOR vLightPoint7;
            DirectX::XMVECTOR vLightPoint8;
            DirectX::XMVECTOR mappedLightPointPower2;

            DirectX::XMVECTOR vAEcl; 			// la valeur ambiante de l'�clairage
            DirectX::XMVECTOR vAMat; 			// la valeur ambiante du mat�riau
            DirectX::XMVECTOR vDEcl; 			// la valeur diffuse de l'�clairage 
            DirectX::XMVECTOR vDMat; 			// la valeur diffuse du mat�riau 
            DirectX::XMVECTOR vSMat; 			// la valeur sp�culaire du mat�riau 

            float puissance;
            float sunPower;

            float tick;
            float waveAmplitude;
            float waveFrequency;

            DirectX::XMFLOAT2 undertow;

            float uselessFill;


        public:
            ShadersParams() :
                tick{ 0.f }
            {
                vLumiere = DirectX::XMVectorSet(130.0f, 130.0f, -50.0f, 1.0f);
                vAEcl = DirectX::XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
                vAMat = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
                vDEcl = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
                vDMat = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
                undertow = { 0.f, 0.f };
            }

            ShadersParams(
                const DirectX::XMVECTOR& lumiere,
                const DirectX::XMVECTOR& AEcl,
                const DirectX::XMVECTOR& AMat,
                const DirectX::XMVECTOR& DEcl,
                const DirectX::XMVECTOR& DMat
            ) :
                tick{ 0.f }
            {
                vLumiere = lumiere;
                vAEcl = AEcl;
                vAMat = AMat;
                vDEcl = DEcl;
                vDMat = DMat;
                undertow = { 0.f, 0.f };
            }

            ShadersParams(
                const DirectX::XMVECTOR& lumiere,
                const DirectX::XMVECTOR& AEcl,
                const DirectX::XMVECTOR& AMat,
                const DirectX::XMVECTOR& DEcl,
                const DirectX::XMVECTOR& DMat,
                const DirectX::XMVECTOR& SEcl,
                const DirectX::XMVECTOR& SMat
            ) :
                tick{ 0.f }
            {
                vLumiere = lumiere;
                vAEcl = AEcl;
                vAMat = AMat;
                vDEcl = DEcl;
                vDMat = DMat;
                vSMat = SMat;
                undertow = { 0.f, 0.f };
            }
        };
    }

    class Plane : public Mesh<ShaderPlane::ShadersParams>
    {
    public:
        enum
        {
            X_MIN = 0,
            Z_MIN = 0,

            POINTS_X_COUNT = 300,
            POINTS_Z_COUNT = 300,

            LAST_X_POINT_INDEX = POINTS_X_COUNT - 1,
            LAST_Z_POINT_INDEX = POINTS_Z_COUNT - 1,
            NBPOINTS = POINTS_X_COUNT * POINTS_Z_COUNT,
            TRIANGLE_COUNT = LAST_X_POINT_INDEX * LAST_Z_POINT_INDEX * 2,
            INDEX_COUNT = TRIANGLE_COUNT * 3
        };

        /*enum
        {
            SIN_ARRAY_ELEMENTS_COUNT = 360
        };*/

        static constexpr const float DEFAULT_Y_LEVEL_WATER_PLANE = 0.f;

        static constexpr const float TICK_INCREMENT = 0.04806f;
        static constexpr const float WAVE_AMPLITUDE = 0.5f;
        static constexpr const float WAVE_FREQUENCY = 0.70f;
        static constexpr const float WAVE_SPEED_COEFFICIENT = 0.01f;


    private:
        std::vector<unsigned int> m_index;
        std::vector<SommetPlane> m_sommets;

        float xWidth;
        float zHeight;
        //float m_sinArray[SIN_ARRAY_ELEMENTS_COUNT];


    protected:
        PM3D::CDispositifD3D11* pDispositif;

        ID3D11Buffer* pVertexBuffer;
        ID3D11Buffer* pIndexBuffer;

        ID3D11VertexShader*  pVertexShader;
        ID3D11PixelShader*  pPixelShader;
        ID3D11InputLayout* pVertexLayout;

        // D�finitions des valeurs d'animation
        ID3D11Buffer* pConstantBuffer;

        //Pour texture
        Material m_material;
        Effect m_textureEffect;

        /*ID3D11ShaderResourceView* pSinTex;
        ID3D11Texture1D* pSinText1D;
        ID3D11SamplerState* pSinSampler;*/


    public:
        Plane(const std::string& textureFileName);

        virtual ~Plane(void);

        
    public:
        void Draw();
        void SetTexture(PM3D::CTexture* pTexture);


    protected:
        void setTexture(PM3D::CTexture* texture);
        void loadTexture(const std::string& filename);    

        //void InitSin();
    };
}
#endif