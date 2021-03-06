#ifndef TERRAIN_H
#define TERRAIN_H

#include "Material.h"
#include "Effect.h"
#include "Vertex.h"
#include "Mesh.h"
#include "../PetitMoteur3D/PetitMoteur3D/sommetbloc.h"

#include <vector>
#include <string>

namespace PM3D
{
	class CTexture;
    class CDispositifD3D11;
}

namespace PirateSimulator
{
    class Triangle;

    namespace ShaderTerrain
    {
        struct ShadersParams
        {
            XMMATRIX matWorldViewProj;	// la matrice totale 
            XMMATRIX matWorld;			// matrice de transformation dans le monde 
            
            XMVECTOR vLumiere; 			// la position de la source d'�clairage (Point)
            XMVECTOR vCamera; 			// la position de la cam�ra

            XMVECTOR vAEcl; 			// la valeur ambiante de l'�clairage
            XMVECTOR vAMat; 			// la valeur ambiante du mat�riau
            XMVECTOR vDEcl; 			// la valeur diffuse de l'�clairage 
            XMVECTOR vDMat; 			// la valeur diffuse du mat�riau 
            XMVECTOR vSEcl; 			// la valeur sp�culaire de l'�clairage 
            XMVECTOR vSMat; 			// la valeur sp�culaire du mat�riau 

            float puissance;
            float sunPower;
            XMFLOAT2 remplissage;


            ShadersParams()
            {
                vLumiere = XMVectorSet(-10.0f, 10.0f, -15.0f, 1.0f);
                //vLumiere = XMVectorSet(130.0f, 130.0f, -50.0f, 1.0f);
                vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
                vAMat = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
                vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
                vDMat = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
            }
            
            ShadersParams(
                    const XMVECTOR& lumiere,
                    const XMVECTOR& AEcl,
                    const XMVECTOR& AMat,
                    const XMVECTOR& DEcl,
                    const XMVECTOR& DMat
                )
            {
                vLumiere = lumiere;
                vAEcl = AEcl;
                vAMat = AMat;
                vDEcl = DEcl;
                vDMat = DMat;
            }

            ShadersParams(
                const XMVECTOR& lumiere,
                const XMVECTOR& AEcl,
                const XMVECTOR& AMat,
                const XMVECTOR& DEcl,
                const XMVECTOR& DMat,
                const XMVECTOR& SEcl,
                const XMVECTOR& SMat
            )
            {
                vLumiere = lumiere;
                vAEcl = AEcl;
                vAMat = AMat;
                vDEcl = DEcl;
                vDMat = DMat;
                vSEcl = SEcl;
                vSMat = SMat;
            }
        };
    }
    

    //  Classe : Terrain
    //
    //  BUT : 	Classe du terrain de notre jeu
    //
    class Terrain : public Mesh<ShaderTerrain::ShadersParams>
    {
    public:
        static D3D11_INPUT_ELEMENT_DESC Terrain::layout[];
        static UINT numElements;

    protected:
        PM3D::CDispositifD3D11* pDispositif;

        ID3D11Buffer* pVertexBuffer;
        ID3D11Buffer* pIndexBuffer;

        ID3D11InputLayout* pVertexLayout;

        // D�finitions des valeurs d'animation
        ID3D11Buffer* pConstantBuffer;

        //Pour texture
        Material m_material;

        Effect m_textureEffect;

    private:
        int m_terrainWidth;
        int m_terrainHeight;
        float m_terrainScale;
        std::vector<Vertex> m_vertexArray;
        std::vector<CSommetBloc> m_csommetsArray;
        std::vector<unsigned int> m_index_bloc;

    public:
        // Get the configuration from the config file
        Terrain();
        // Override the configuration file
        Terrain(int h, int w, float s, const std::string& fieldFileName, const std::string& textureFileName);

        // Destructeur
        virtual ~Terrain();

    public:
        virtual void Anime(float tempsEcoule);

        void Draw();

        void Init(const std::string& textureFileName);

        void addSommet(Vertex v);
        void addTriangle(const Triangle& t);
        float getHeight(XMVECTOR pos);
        
        float getScale()
        {
            return m_terrainScale;
        }
        int getWidth()
        {
            return m_terrainWidth;
        }

        int getHeight()
        {
            return m_terrainHeight;
        }
        std::vector<CSommetBloc>& getSommetsArray()
        {
            return std::move(m_csommetsArray);
        }
        std::vector<unsigned int>& getIndex()
        {
            return std::move(m_index_bloc);
        }


		void setTexture(PM3D::CTexture* texture);

		void loadTexture(const std::string& filename);

    protected:
        // Constructeur par d�faut
        void InitShaders();
    };

}

#endif // TERRAIN_H