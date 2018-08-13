#pragma once

namespace Fbx
{
	class Utility
	{
	public:
		static D3DXMATRIX Negative(bool bXna);

		static D3DXVECTOR2 ToVector2(FbxVector2& vec);
		static D3DXVECTOR3 ToVector3(FbxVector4& vec);
		static D3DXCOLOR   ToColor(FbxVector4& vec);
		static D3DXCOLOR   ToColor(FbxPropertyT<FbxDouble3>& vec, FbxPropertyT<FbxDouble>& factor);
		static D3DXMATRIX  ToMatrix(FbxAMatrix& matrix, bool bXna);

		static D3DXVECTOR3 CalcTangent(D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2
										, D3DXVECTOR2 uv0, D3DXVECTOR2 uv1, D3DXVECTOR2 uv2);

		static string GetTextureFile(FbxProperty& prop);
		static string GetMaterialName(FbxMesh* mesh, int polygonIndex, int cpIndex);
		static D3DXVECTOR2 GetUv(FbxMesh* mesh, int cpIndex, int uvIndex);
	};
}