#pragma once

class Model;

namespace Fbx
{
	struct MyFbxMaterial;
	struct MyFbxMeshData;
	struct MyFbxMeshPartData;
	struct MyFbxJoint;
	struct MyFbxBoneData;
	struct MyFbxAnimation;

	class Exporter
	{
	public:
		Exporter(wstring fbxFile, bool bXna = false);
		~Exporter();

	public:
		static void ExportMatMesh(wstring fbxFile, wstring savePath);
	

	public:
		void ExportMaterial(wstring saveFolder, wstring saveName);
		void ExportMesh(wstring saveFolder, wstring saveName);
		void ExportAnimation(wstring saveFolder, wstring saveName);

	private:
		void CopyTextureFile(string& textureFile, wstring saveFolder);

		void ReadBoneData(FbxNode* node, int index, int parent);
		void ReadMeshData(FbxNode* node, int parentBone);
		void ReadSkinData(void);

		void WriteMeshData(wstring saveFolder, wstring saveName);

		int GetBoneIndexByName(string name);

		void ReadAnimation(void);
		void ReadAnimation(MyFbxAnimation* animation, FbxNode* node, int start, int end);
		void WriteAnimation(wstring saveFolder, wstring saveName);

	private:
		bool bXna; //임시코드


		Model* model;

		FbxManager*				manager;
		FbxScene*				scene;
		FbxIOSettings*			ios;
		FbxImporter*			importer;
		FbxGeometryConverter*	converter;

	private:
		//material
		vector<MyFbxMaterial*>	materials;
		//mesh
		vector<MyFbxBoneData*>	boneDatas;
		vector<MyFbxMeshData*>	meshDatas;
		//animation
		vector<MyFbxAnimation*> animDatas;

	};
}