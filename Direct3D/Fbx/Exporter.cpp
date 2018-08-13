#include "stdafx.h"
#include "Exporter.h"
#include "Type.h"
#include "BoneWeights.h"
#include "Utility.h"

#include "../Utilities/BinaryFile.h"

Fbx::Exporter::Exporter(wstring fbxFile, bool bXna)
	: bXna(bXna)
{
	manager = FbxManager::Create();
	scene = FbxScene::Create(manager, ""); //기본씬을 불러온다.

	//io setting
	ios = FbxIOSettings::Create(manager, IOSROOT); //Root 노드로부터 적용
	ios->SetBoolProp(IMP_FBX_TEXTURE, true); //Texture 정보를 얻기 위해 셋팅
	manager->SetIOSettings(ios);

	//importer
	importer = FbxImporter::Create(manager, "");

	string sFile = String::ToString(fbxFile);
	bool bCheck = importer->Initialize(sFile.c_str(), -1, ios);
	assert(bCheck);

	bCheck = importer->Import(scene);
	assert(bCheck);

	converter = new FbxGeometryConverter(manager);
}

Fbx::Exporter::~Exporter()
{
	SAFE_DELETE(converter);

	ios->Destroy();
	importer->Destroy();
	scene->Destroy();
	manager->Destroy();
}

void Fbx::Exporter::ExportMatMesh(wstring fbxFile, wstring savePath)
{
	Fbx::Exporter* exporter = NULL;

	exporter = new Fbx::Exporter(fbxFile);
	wstring fileName = Path::GetFileNameWithoutExtension(fbxFile);
	exporter->ExportMaterial(savePath, fileName);
	exporter->ExportMesh(savePath, fileName);
	SAFE_DELETE(exporter);
}


void Fbx::Exporter::ExportMaterial(wstring saveFolder, wstring saveName)
{
	//씬이 가진 메터리얼 개수
	int count = scene->GetMaterialCount();

	//메터리얼 리스트 순회
	//사용자 메터리얼 클래스에 데이터 삽입
	for (int i = 0; i < count; i++)
	{
		FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(i);

		//Fbx 파일중에서 원하는 정보만 따로 담기 위해 struct 생성
		MyFbxMaterial* material = new MyFbxMaterial();
		material->Name = fbxMaterial->GetName();

		//.Is -> dynamic_cast 와 비슷함
		//Surface Lambert
		if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)fbxMaterial;

			material->Ambient  = Utility::ToColor(lambert->Ambient,  lambert->AmbientFactor);
			material->Diffuse  = Utility::ToColor(lambert->Diffuse,  lambert->DiffuseFactor);
			material->Emissive = Utility::ToColor(lambert->Emissive, lambert->EmissiveFactor);
		}

		//Surface Phong
		if (fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			FbxSurfacePhong* phong = (FbxSurfacePhong*)fbxMaterial;

			material->Specular = Utility::ToColor(phong->Specular, phong->SpecularFactor);
			material->Shininess = (float)phong->Shininess;
		}

		FbxProperty prop;
		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		material->DiffuseFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
		material->SpecularFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
		material->EmissiveFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sBump);
		material->NormalFile = Utility::GetTextureFile(prop);

		materials.push_back(material);
	}//for(i : materialCount)

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//존재하지 않는 경로면 새로 생성
	Path::CreateFolders(saveFolder);

	Json::Value root;
	for (MyFbxMaterial* material : materials)
	{
		Json::Value val;
		Json::SetValue(val, "Name", material->Name);

		string shaderName = "999_Mesh.hlsl"; //임시
		Json::SetValue(val, "ShaderName", shaderName);

		Json::SetValue(val, "Ambient",  material->Ambient);
		Json::SetValue(val, "Diffuse",  material->Diffuse);
		Json::SetValue(val, "Emissive", material->Emissive);
		Json::SetValue(val, "Specular", material->Specular);
		Json::SetValue(val, "Shininess", material->Shininess);

		//파일 복사, json에 경로 저장
		CopyTextureFile(material->DiffuseFile, saveFolder);
		Json::SetValue(val, "DiffuseFile", material->DiffuseFile);
		CopyTextureFile(material->SpecularFile, saveFolder);
		Json::SetValue(val, "SpecularFile", material->SpecularFile);
		CopyTextureFile(material->EmissiveFile, saveFolder);
		Json::SetValue(val, "EmissiveFile", material->EmissiveFile);
		CopyTextureFile(material->NormalFile, saveFolder);
		Json::SetValue(val, "NormalFile", material->NormalFile);

		root[material->Name.c_str()] = val;

		SAFE_DELETE(material);
	}
	materials.clear();
	Json::WriteData(saveFolder + saveName + L".material", &root);
}

void Fbx::Exporter::ExportMesh(wstring saveFolder, wstring saveName)
{
	ReadBoneData(scene->GetRootNode(), -1, -1);
	ReadSkinData();

	WriteMeshData(saveFolder, saveName);
}

void Fbx::Exporter::ExportAnimation(wstring saveFolder, wstring saveName)
{
	ReadAnimation();

	WriteAnimation(saveFolder, saveName);
}


void Fbx::Exporter::CopyTextureFile(string& textureFile, wstring saveFolder)
{
	if (textureFile.length() == 0)
		return;

	wstring file = String::ToWString(textureFile);
	wstring fileName = Path::GetFileName(file);
	
	if (Path::ExistFile(textureFile))
	{
		CopyFile(file.c_str(), (saveFolder + fileName).c_str(), FALSE);
		textureFile = String::ToString(fileName);
	}
	else
	{
		assert(false);
		textureFile = "";
	}
}

void Fbx::Exporter::ReadBoneData(FbxNode * node, int index, int parent)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();

		bool bCheck = false;
		bCheck |= (nodeType == FbxNodeAttribute::EType::eSkeleton);
		bCheck |= (nodeType == FbxNodeAttribute::EType::eMesh);
		bCheck |= (nodeType == FbxNodeAttribute::EType::eNull);
		bCheck |= (nodeType == FbxNodeAttribute::EType::eMarker);


		if (bCheck)
		{
			MyFbxBoneData* bone = new MyFbxBoneData();
			bone->Index = index;
			bone->Parent = parent;
			bone->Name = node->GetName();

			bone->Transform		= Utility::ToMatrix(node->EvaluateLocalTransform(),  bXna);
			bone->AbsTransform  = Utility::ToMatrix(node->EvaluateGlobalTransform(), bXna);

			boneDatas.push_back(bone);

			if (nodeType == FbxNodeAttribute::EType::eMesh)
			{
				converter->Triangulate(attribute, true, true);

				ReadMeshData(node, index);
			}
		}//if (bCheck)
	}

	//자식 노드 재귀
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		ReadBoneData(node->GetChild(i), boneDatas.size(), index);
	}
}

void Fbx::Exporter::ReadMeshData(FbxNode* node, int parentBone)
{
	FbxMesh* mesh = node->GetMesh();

	vector<MyFbxVertex*> vertices;
	int vertexCount = 0;
	for (int p = 0; p < mesh->GetPolygonCount(); p++)
	{
		//폴리곤의 버텍스 개수. 3개
		int vertexInPolygon = mesh->GetPolygonSize(p);
		assert(vertexInPolygon == 3);

		//삼각형 정점 순회
		//for (int vi = 0; vi < vertexInPolygon; vi++)
		//정점 뒤집기 위해 반대로
		for (int vi = vertexInPolygon - 1; vi >= 0; vi--)
		{
			MyFbxVertex* vertex = new MyFbxVertex;

			//삼각형 정점으로 컨트롤 포인트 받음
			int cpIndex = mesh->GetPolygonVertex(p, vi);
			vertex->ControlPoint = cpIndex;

			//Material
			vertex->MaterialName = Utility::GetMaterialName(mesh, p, cpIndex);

			//Position
			D3DXVECTOR3 temp;
			FbxVector4 position = mesh->GetControlPointAt(cpIndex);
			temp = Utility::ToVector3(position);
			//Negative 행렬로 X축 반전
			D3DXVec3TransformCoord(&vertex->Vertex.position, &temp, &Utility::Negative(bXna));

			//Normal
			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(p, vi, normal);
			normal.Normalize();
			temp = Utility::ToVector3(normal);
			//Negative 행렬로 X축 반전
			D3DXVec3TransformNormal(&vertex->Vertex.normal, &temp, &Utility::Negative(bXna));

			//Uv
			int uvIndex = mesh->GetTextureUVIndex(p, vi);
			vertex->Vertex.uv = Utility::GetUv(mesh, cpIndex, uvIndex);

			vertices.push_back(vertex);
		}//for(vi)

		D3DXVECTOR3 p0 = vertices[vertices.size() - 1 - 2]->Vertex.position;
		D3DXVECTOR3 p1 = vertices[vertices.size() - 1 - 1]->Vertex.position;
		D3DXVECTOR3 p2 = vertices[vertices.size() - 1 - 0]->Vertex.position;

		D3DXVECTOR2 uv0 = vertices[vertices.size() - 1 - 2]->Vertex.uv;
		D3DXVECTOR2 uv1 = vertices[vertices.size() - 1 - 1]->Vertex.uv;
		D3DXVECTOR2 uv2 = vertices[vertices.size() - 1 - 0]->Vertex.uv;

		D3DXVECTOR3 tangent = Utility::CalcTangent(p0, p1, p2, uv0, uv1, uv2);

		vertices[vertices.size() - 1 - 2]->Vertex.tangent += tangent;
		vertices[vertices.size() - 1 - 1]->Vertex.tangent += tangent;
		vertices[vertices.size() - 1 - 0]->Vertex.tangent += tangent;

	}//for(p)

	for (size_t i = 0; i < vertices.size(); i++)
	{
		D3DXVECTOR3 n = vertices[i]->Vertex.normal;
		D3DXVECTOR3 t = vertices[i]->Vertex.tangent;

		D3DXVECTOR3 temp = (t - n * D3DXVec3Dot(&n, &t));
		D3DXVec3Normalize(&vertices[i]->Vertex.tangent, &temp);
	}


	MyFbxMeshData* meshData = new MyFbxMeshData();
	meshData->Name = node->GetName();
	meshData->ParentBone = parentBone;
	meshData->Vertices = vertices;
	meshData->Mesh = mesh;

	meshDatas.push_back(meshData);
}

void Fbx::Exporter::ReadSkinData(void)
{
	for (MyFbxMeshData* meshData : meshDatas)
	{
		FbxMesh* mesh = meshData->Mesh;

		///////////////////////////////////////////////////////////////////////////////////////

		int deformerCount = mesh->GetDeformerCount();
		vector<MyFbxBoneWeights> boneWeights(mesh->GetControlPointsCount(), MyFbxBoneWeights());

		for (int i = 0; i < deformerCount; i++)
		{
			FbxDeformer* deformer = mesh->GetDeformer(i, FbxDeformer::eSkin);

			FbxSkin* skin = reinterpret_cast<FbxSkin *>(deformer);

			if (skin == NULL) continue;

			for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); clusterIndex++)
			{
				FbxCluster* cluster = skin->GetCluster(clusterIndex);
				assert(cluster->GetLinkMode() == FbxCluster::eNormalize);

				string linkName = cluster->GetLink()->GetName();
				UINT boneIndex = GetBoneIndexByName(linkName);


				FbxAMatrix transform;
				FbxAMatrix linkTransform;

				cluster->GetTransformMatrix(transform);
				cluster->GetTransformLinkMatrix(linkTransform);

				boneDatas[boneIndex]->Transform		= Utility::ToMatrix(transform,     bXna);
				boneDatas[boneIndex]->AbsTransform  = Utility::ToMatrix(linkTransform, bXna);


				for (int indexCount = 0; indexCount < cluster->GetControlPointIndicesCount(); indexCount++)
				{
					int temp = cluster->GetControlPointIndices()[indexCount];
					double* weights = cluster->GetControlPointWeights();

					boneWeights[temp].AddBoneWeight(boneIndex, (float)weights[indexCount]);
				}
			}//for(clusterIndex)
		}//for(joints)

		//Bone 가중치 정규화
		size_t boneWeightsCount = boneWeights.size();
		for (size_t i = 0; i < boneWeightsCount; i++)
			boneWeights[i].Normalize();

		//Vertex에 Bone 가중치 입력
		for (MyFbxVertex* vertex : meshData->Vertices)
		{
			int cpIndex = vertex->ControlPoint;

			MyFbxBlendWeight weights;
			boneWeights[cpIndex].GetBlendWeights(weights);
			vertex->Vertex.blendIndices = weights.Indices;
			vertex->Vertex.blendWeights = weights.Weights;
		}

		//Material 의 이름 기준으로 Vertex 정리 
		int materialCount = scene->GetMaterialCount();
		for (int i = 0; i < materialCount; i++)
		{
			FbxSurfaceMaterial* material = scene->GetMaterial(i);
			string materialName = material->GetName();

			vector<MyFbxVertex*> gather;
			for (MyFbxVertex* temp : meshData->Vertices)
			{
				if (temp->MaterialName == materialName)
					gather.push_back(temp);
			}
			if (gather.size() == 0) continue;

			//meshPart 생성
			MyFbxMeshPartData* meshPart = new MyFbxMeshPartData();
			meshPart->MaterialName = materialName;
			size_t gatherSize = gather.size();
			for (size_t i = 0; i < gatherSize; i++)
			{
				meshPart->Vertices.push_back(gather[i]->Vertex);
				meshPart->Indices.push_back(i);
			}
			meshData->MeshParts.push_back(meshPart);
		}
	}//for(MeshData)
}

void Fbx::Exporter::WriteMeshData(wstring saveFolder, wstring saveName)
{
	//존재하지 않는 경로면 새로 생성
	Path::CreateFolders(saveFolder);

	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + saveName + L".mesh");

	//////////////////////Bone 데이터//////////////////////////
	w->UInt(boneDatas.size());
	for (MyFbxBoneData* bone : boneDatas)
	{
		w->Int(bone->Index);
		w->String(bone->Name);

		w->Int(bone->Parent);
		w->Matrix(bone->Transform);
		w->Matrix(bone->AbsTransform);

		SAFE_DELETE(bone);
	}
	//////////////////////////////////////////////////////////

	w->UInt(meshDatas.size());
	for (MyFbxMeshData* meshData : meshDatas)
	{
		w->String(meshData->Name);
		w->Int(meshData->ParentBone);

		w->UInt(meshData->MeshParts.size());
		for (MyFbxMeshPartData* part : meshData->MeshParts)
		{
			w->String(part->MaterialName);

			w->UInt(part->Vertices.size());
			w->Byte(&part->Vertices[0], sizeof(ModelVertexType) * part->Vertices.size());

			w->UInt(part->Indices.size());
			w->Byte(&part->Indices[0], sizeof(UINT) * part->Indices.size());

			SAFE_DELETE(part);
		}

		SAFE_DELETE(meshData);
	}

	w->Close();
	SAFE_DELETE(w);
}

int Fbx::Exporter::GetBoneIndexByName(string name)
{
	for (size_t i = 0; i < boneDatas.size(); i++)
	{
		if (boneDatas[i]->Name == name)
			return i;
	}
	return -1;
}

void Fbx::Exporter::ReadAnimation(void)
{
	FbxTime::EMode mode = scene->GetGlobalSettings().GetTimeMode();
	float frameRate = (float)FbxTime::GetFrameRate(mode);

	//애니메이션 이름 목록 확인용 코드
	//FbxArray<FbxString*> animationArray;
	//FbxDocument* document = dynamic_cast<FbxDocument*>(scene);
	//document->FillAnimStackNameArray(animationArray);

	for (int i = 0; i < importer->GetAnimStackCount(); i++)
	{
		MyFbxAnimation* animation = new MyFbxAnimation();
		animation->FrameRate = frameRate;
		
		FbxTakeInfo* takeInfo = importer->GetTakeInfo(i);
		animation->Name = takeInfo->mName.Buffer();

		FbxTimeSpan span = takeInfo->mLocalTimeSpan;
		int start = (int)span.GetStart().GetFrameCount();
		int end = (int)span.GetStop().GetFrameCount();

		if (start < end)
		{
			ReadAnimation(animation, scene->GetRootNode(), start, end);
		}

		animation->TotalFrame = (UINT)animation->Keyframes[0]->Transforms.size();

		animDatas.push_back(animation);
	}//for (i)
}

void Fbx::Exporter::ReadAnimation(MyFbxAnimation * animation, FbxNode * node, int start, int end)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();
		if (nodeType == FbxNodeAttribute::eSkeleton)
		{
			MyFbxKeyframe* keyframe = new MyFbxKeyframe();
			keyframe->BoneName = node->GetName();

			for (int i = start; i <= end; i++)
			{
				FbxTime animTime;
				animTime.SetFrame(i);

				//Local Matrix
				FbxAMatrix matrix = node->EvaluateLocalTransform(animTime);
				D3DXMATRIX transform = Utility::ToMatrix(matrix, bXna);

				MyFbxKeyframeData data;
				data.Transform = transform;
				data.Translation = D3DXVECTOR3(transform._41, transform._42, transform._43);
				D3DXQuaternionRotationMatrix(&data.Rotation, &transform);
				FbxVector4 scale = node->EvaluateLocalScaling(animTime);
				data.Scale = Utility::ToVector3(scale);

				keyframe->Transforms.push_back(data);
			}//for (i)

			animation->Keyframes.push_back(keyframe);
		}
		//legacy일땐 mesh에서 얻어야함

	}

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		ReadAnimation(animation, node->GetChild(i), start, end);
	}
}

void Fbx::Exporter::WriteAnimation(wstring saveFolder, wstring saveName)
{
	//존재하지 않는 경로면 새로 생성
	Path::CreateFolders(saveFolder);

	//TODO : *.anim 파일 저장 형식 통일해야함

	for (size_t i = 0; i < animDatas.size(); i++)
	{
		MyFbxAnimation* anim = animDatas[i];

		BinaryWriter* w = new BinaryWriter();
		wstring fileName = saveFolder + saveName;
		//fileName += String::ToWString(anim->Name);
		fileName += String::WSInt(L"-%02d.anim", i);
		w->Open(fileName);

		float Duration = anim->TotalFrame / anim->FrameRate;
		float Interval = 1.0f / anim->FrameRate;

		w->UInt(anim->Keyframes.size());
		w->Float(Duration); //Duration

		//w->String(anim->Name);
		//w->UInt(anim->TotalFrame);
		//w->Float(anim->FrameRate);
		//w->UInt(anim->Keyframes.size());

		for (MyFbxKeyframe* frame : anim->Keyframes)
		{
			w->String(frame->BoneName);
			w->UInt(frame->Transforms.size());
			w->Float(Duration);
			w->Float(Interval);

			UINT size = frame->Transforms.size();
			//Translate
			w->UInt(size);
			if (size > 0)
			{
				//벡터 복사 하지 않고 배열복사하도록
				D3DXVECTOR3* temp = (D3DXVECTOR3*)malloc(sizeof(D3DXVECTOR3)*size);
				assert(temp != NULL);

				for (size_t i = 0; i < size; i++)
				{
					temp[i] = frame->Transforms[i].Translation;
				}
				w->Byte(&temp[0], sizeof(D3DXVECTOR3)*size);
				free(temp);
			}

			//Rotation
			w->UInt(size);
			if (size > 0)
			{
				//벡터 복사 하지 않고 배열복사하도록
				D3DXQUATERNION* temp = (D3DXQUATERNION*)malloc(sizeof(D3DXQUATERNION)*size);
				assert(temp != NULL);

				for (size_t i = 0; i < size; i++)
				{
					temp[i] = frame->Transforms[i].Rotation;
				}
				w->Byte(&temp[0], sizeof(D3DXQUATERNION)*size);
				free(temp);
			}

			//Scale
			w->UInt(size);
			if (size > 0)
			{
				//벡터 복사 하지 않고 배열복사하도록
				D3DXVECTOR3* temp = (D3DXVECTOR3*)malloc(sizeof(D3DXVECTOR3)*size);
				assert(temp != NULL);

				for (size_t i = 0; i < size; i++)
				{
					temp[i] = frame->Transforms[i].Scale;
				}
				w->Byte(&temp[0], sizeof(D3DXVECTOR3)*size);
				free(temp);
			}

			//Time 정보는 없으므로 0
			w->UInt(0);

			//w->Byte(&frame->Transforms[0], sizeof(MyFbxKeyframeData) * frame->Transforms.size());
			SAFE_DELETE(frame);
		}

		SAFE_DELETE(anim);

		w->Close();
		SAFE_DELETE(w);
	}

}



