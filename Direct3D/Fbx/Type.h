#pragma once
#include "stdafx.h"

namespace Fbx
{
	struct MyFbxMaterial
	{
		string Name;

		D3DXCOLOR Ambient;
		D3DXCOLOR Diffuse;
		D3DXCOLOR Specular;
		D3DXCOLOR Emissive;

		float Shininess;

		string DiffuseFile;
		string SpecularFile;
		string EmissiveFile;
		string NormalFile;
	};

	struct MyFbxBoneData
	{
		int Index;
		string Name;

		int Parent;
		D3DXMATRIX Transform;
		D3DXMATRIX AbsTransform;
	};

	struct MyFbxVertex
	{
		string MaterialName;
		int ControlPoint;

		ModelVertexType Vertex;
	};

	struct MyFbxMeshPartData
	{
		string MaterialName;

		vector<ModelVertexType> Vertices;
		vector<UINT> Indices;
	};

	struct MyFbxMeshData
	{
		string Name;
		int ParentBone;
		
		FbxMesh* Mesh;

		vector<MyFbxVertex*> Vertices;
		vector<MyFbxMeshPartData*> MeshParts;
	};

	struct MyFbxKeyframeData
	{
		D3DXMATRIX Transform;
		D3DXQUATERNION Rotation;
		D3DXVECTOR3 Translation;
		D3DXVECTOR3 Scale;
	};

	struct MyFbxKeyframe
	{
		string BoneName;
		vector<MyFbxKeyframeData> Transforms;
	};

	struct MyFbxAnimation
	{
		string Name;

		UINT TotalFrame;
		float FrameRate;

		vector<MyFbxKeyframe*> Keyframes;
	};
}