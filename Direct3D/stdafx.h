#pragma once

#include <Windows.h>
#include <assert.h>

//#include <crtdbg.h>//C Run Time DEBUG

//STL
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map> //hash 로 동작
#include <set>
#include <functional>
#include <memory>	//shared_ptr<int> pTest(new int(10));
#include <thread>
#include <mutex>

using namespace std;

//Direct3D
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//ImGui
#include <imgui.h>
#include <imguiDx11.h>
#pragma comment(lib, "imgui.lib")

//FBX SDK
#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")
using namespace fbxsdk;

//Json Cpp
#include <json/json.h>
#pragma comment(lib, "jsoncpp.lib")
//warning 무시
#pragma warning(disable : 4996) //C4996 (warning C4996: 'Json::StyledWriter': Use StreamWriterBuilder instead)
#pragma warning(disable : 4099) 
// --> lib 파일내부 디버깅 정보가 없을 때
// jsoncpp, DirectXTex

//DirectXTex
#include <DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

//region 만들기 
//#pragma region 테스트
//#pragma endregion

#define EPSILON6 1e-6

#define SAFE_RELEASE(p)			{ if(p) { p->Release(); p = NULL; } }
#define SAFE_DELETE(p)			{ if(p) { delete p; p = NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] p; p = NULL; } }

//#define GETFUNC(varType, varName, funName) \
//inline void Get##funName(varType& var) const { var = varName; }
//#define SETFUNC(varType, varName, funName) \
//inline void Set##funName(varType& var) const { varName = var; }
//#define GETSETFUNC(varType, varName, funName) \
//inline void Get##funName(varType& var) const { var = varName; } \
//inline void Set##funName(varType& var) const { varName = var; }

//TODO : 프로젝트 경로 변경시 수정해야함
const wstring Assets     = L"../_Assets/";
const wstring Contents   = L"../_Contents/";
const wstring Textures   = L"../_Contents/Textures/";
const wstring Models	 = L"../_Contents/Models/";
const wstring Landscapes = L"../_Contents/Landscape/";
const wstring FbxModels  = L"../_Contents/FbxModels/";

const wstring Shaders    = L"./Shaders/";
const wstring LevelDatas = L"./LevelDatas/";

extern string	gStringParentDirectory;
extern wstring  gWstringParentDirectory;


#include "./Systems/D3D.h"
#include "./Systems/Time.h"
#include "./Systems/Mouse.h"
#include "./Systems/Keyboard.h"

#include "./Renders/VertexLayouts.h"
#include "./Renders/States.h"
#include "./Renders/Shader.h"
#include "./Renders/ShaderBuffer.h"
#include "./Renders/GlobalBuffers.h"
#include "./Renders/Texture.h"
#include "./Renders/Material.h"
#include "./Renders/RenderTarget.h"

#include "./Utilities/JsonManager.h"
#include "./Utilities/Math.h"
#include "./Utilities/MathDX.h"
#include "./Utilities/String.h"
#include "./Utilities/Path.h"

#include "./Viewer/FrustumCulling.h"


#include "./Model/Component.h"
#include "./Model/Model.h"
#include "./Model/ModelBone.h"
#include "./Model/ModelMesh.h"
#include "./Model/Bounding/ModelShape.h"


#include "./Lights/LightManager.h"

#include "./Executes/GuiSettings.h"
#include "./Executes/Execute.h"



typedef VertexTextureNormalTangentBlend ModelVertexType;
typedef VertexTexture OceanVertexType;