cbuffer param
{ 
	float4x4 matWorldViewProj;   // la matrice totale 
	float4x4 matWorld;			// matrice de transformation dans le monde 
	float4 vLumiere; 			// la position de la source d'�clairage (Point)
	float4 vCamera; 			// la position de la cam�ra
	float4 vAEcl; 				// la valeur ambiante de l'�clairage
	float4 vAMat; 				// la valeur ambiante du mat�riau
	float4 vDEcl; 				// la valeur diffuse de l'�clairage 
	float4 vDMat; 				// la valeur diffuse du mat�riau 
}

struct VS_Sortie
{
	float4 Pos : SV_Position;
	float3 Norm :    TEXCOORD0;
	float3 vDirLum : TEXCOORD1;
	float3 vDirCam : TEXCOORD2;
};


VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL)
{
VS_Sortie sortie = (VS_Sortie)0;
	
	sortie.Pos = mul(Pos, matWorldViewProj); 
	sortie.Norm = mul(Normale, matWorld); 
	
 	float3 PosWorld = mul(Pos, matWorld);

	sortie.vDirLum = vLumiere - PosWorld; 
	sortie.vDirCam = vCamera - PosWorld; 

	return sortie;
}


float4 MiniPhongPS( VS_Sortie vs ) : SV_Target
{
	// Normaliser les param�tres
	float3 N = normalize(vs.Norm);
 	float3 L = normalize(vs.vDirLum);
	float3 V = normalize(vs.vDirCam);

	// Valeur de la composante diffuse
	float4 diff = saturate(dot(N, L)); 

	// R = 2 * (N.L) * N � L
	float3 R = normalize(2 * diff * N - L);
    
	float4 S = pow(saturate(dot(R, V)), 12); 

	float lastCoeff = vAEcl * vAMat + 
              vDEcl * vDMat * diff;

	float4 couleur = {lastCoeff, lastCoeff, 0.f, 0.f}; //Jaune
	
	couleur+= S;
    
	return couleur;
}


technique11 MiniPhong
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, MiniPhongVS()));
        SetPixelShader(CompileShader(ps_4_0, MiniPhongPS()));
        SetGeometryShader(NULL);
    }
}

 