#include "stdafx.h"
#include "Terrain.h"
#include "TerrainData.h"
#include "TerrainQuadTree.h"
#include "Tree.h"
//#include "AStar.h"
#include "../Model/Component.h"

#include "../Command/CommandTerrain.h"

#include "../Utilities/Xml.h"

#define USE_ASTAR 0
#define USE_TREE  0

Landscape::Terrain::Terrain(ExecuteValues* values)
	: values(values)
	, material(NULL)
	, terrainData(NULL)
	, editmode(EDITMODE_ALPHATEXTURE)
	, intensity(50)
	, vertexDataOld(NULL)
	, bChangeTerrain(false)
	, showAreaIndex(0)
{
	worldBuffer = new WorldBuffer();
	gridBuffer  = new GridBuffer();
	enemyAreaBuffer = new EnemyAreaBuffer();
	
	D3D11_SAMPLER_DESC samplerDesc;
	States::GetSamplerDesc(&samplerDesc);
	States::CreateSampler(&samplerDesc, &diffuseSampler);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	States::CreateSampler(&samplerDesc, &detailSampler);

	selectTexture = 0;
	ZeroMemory(&subDiffuse, sizeof(Texture*) * TEXTURE_MAX);
	ZeroMemory(&subNormal, sizeof(Texture*) * TEXTURE_MAX);

#if USE_ASTAR
	astar = new AStar(this);
#endif
}

Landscape::Terrain::~Terrain()
{
#if USE_ASTAR
	SAFE_DELETE(astar);
#endif

#if USE_TREE
	for (Tree* tree : trees)
	{
		SAFE_DELETE(tree);
	}
#endif

	for (UINT i = 0; i < TEXTURE_MAX; i++)
	{
		SAFE_DELETE(subDiffuse[i]);
		SAFE_DELETE(subNormal[i]);
	}

	SAFE_RELEASE(diffuseSampler);
	SAFE_RELEASE(detailSampler);

	SAFE_DELETE(terrainData);

	SAFE_DELETE(gridBuffer);
	SAFE_DELETE(enemyAreaBuffer);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(material);
}

void Landscape::Terrain::LoadData(Json::Value * value)
{
	wstring shaderFile = Shaders + L"Terrain.hlsl";
	wstring diffuseMap = Contents + L"Landscape/Dirt3.png";
	wstring normalMap = L"";
	wstring detailMap = Contents + L"Landscape/Dirt3_detail.png";

	wstring heightMap = Contents + L"Landscape/HeightMap.png";
	wstring alphaMap[ALPHAMAP_MAX] = { L"", L"" };
	wstring alphaDiffuse[TEXTURE_MAX] = { L"", L"", L"", L"", L"", L"" };
	wstring alphaNormal[TEXTURE_MAX] = { L"", L"", L"", L"", L"", L"" };

	Json::Value terrainValue = (*value)["Landscape"]["Terrain"];
	if (terrainValue.isNull() == false)
	{
		//Material
		{
			//Shader
			Json::GetValue(terrainValue, "Shader File", shaderFile);
			//Diffuse Map
			Json::GetValue(terrainValue, "Diffuse Map", diffuseMap);
			//Normal Map
			Json::GetValue(terrainValue, "Normal Map", normalMap);
			//Detail Map
			Json::GetValue(terrainValue, "Detail Map", detailMap);
		}

		//Editor Prop
		{
			//Brush
			Json::GetValue(terrainValue, "Brush Color Hover",		gridBuffer->Data.ColorHover);
			Json::GetValue(terrainValue, "Brush Color Selected",	gridBuffer->Data.ColorSelected);
			//Grid
			Json::GetValue(terrainValue, "Grid Color",				gridBuffer->Data.ColorGrid);
			Json::GetValue(terrainValue, "Grid Spacing",			gridBuffer->Data.GridSpacing);
			Json::GetValue(terrainValue, "Grid Thickness",			gridBuffer->Data.GridThickness);
		}

		//Terrain Data
		{
			//Height Map
			Json::GetValue(terrainValue, "Height Map", heightMap);
			//Alpha Map
			Json::GetValue(terrainValue, "Alpha Map 0", alphaMap[0]);
			Json::GetValue(terrainValue, "Alpha Map 1", alphaMap[1]);
		}

		//Textures
		for (UINT i = 0; i < TEXTURE_MAX; i++)
		{
			Json::GetValue(terrainValue, String::SInt("SubDiffuse%d", i), alphaDiffuse[i]);
			Json::GetValue(terrainValue, String::SInt("SubNormal%d", i),  alphaNormal[i]);
		}

		//EnemyArea
		{
			Json::Value enemyAreaValue = terrainValue["EnemyArea"];
			if (enemyAreaValue.isNull() == false)
			{
				size_t count = 0;
				Json::GetValue(enemyAreaValue, "EnemyAreaCount", count);
				for (size_t i = 0; i < count; i++)
				{
					EnemyArea area;
					Json::GetValue(enemyAreaValue, String::SInt("EnemyArea%d-Name", i),  area.Name);
					Json::GetValue(enemyAreaValue, String::SInt("EnemyArea%d-Start", i), area.Start);
					Json::GetValue(enemyAreaValue, String::SInt("EnemyArea%d-Size", i),  area.Size);
					Json::GetValue(enemyAreaValue, String::SInt("EnemyArea%d-Color", i), area.Color);

					enemyAreas.push_back(area);
				}
			}
		}
	}

	material = new Material(shaderFile);
	material->SetDiffuseMap(diffuseMap);
	material->SetNormalMap(normalMap);
	material->SetDetailMap(detailMap);

	terrainData = new TerrainData(heightMap);
	//terrainData = new TerrainQuadTree(heightMap);

	for (UINT i = 0; i < ALPHAMAP_MAX; i++)
	{
		if (alphaMap[i].length() > 0)
			terrainData->SetAlphaMapFile(i, alphaMap[i]);
	}
	
	
	for (UINT i = 0; i < TEXTURE_MAX; i++)
	{
		ChangeSubDiffuse(i, alphaDiffuse[i]);
		ChangeSubNormal(i,  alphaNormal[i]);
	}
}

void Landscape::Terrain::SaveData(Json::Value * value)
{
	Json::Value terrainValue;

	//Material
	{
		//Shader
		Json::SetValue(terrainValue, "Shader File", material->GetShader()->GetFile());
		//Diffuse Map
		Json::SetValue(terrainValue, "Diffuse Map", material->GetDiffuseMap()->GetFile());
		//Normal Map
		Json::SetValue(terrainValue, "Normal Map", material->GetNormalMap()->GetFile());
		//Detail Map
		Json::SetValue(terrainValue, "Detail Map", material->GetDetailMap()->GetFile());
	}

	//Editor Prop
	{
		//Brush
		Json::SetValue(terrainValue, "Brush Color Hover",	 gridBuffer->Data.ColorHover);
		Json::SetValue(terrainValue, "Brush Color Selected", gridBuffer->Data.ColorSelected);
		//Grid
		Json::SetValue(terrainValue, "Grid Color",	   gridBuffer->Data.ColorGrid);
		Json::SetValue(terrainValue, "Grid Spacing",   gridBuffer->Data.GridSpacing);
		Json::SetValue(terrainValue, "Grid Thickness", gridBuffer->Data.GridThickness);
	}

	//Terrain Data
	{
		//Height Map
		Json::SetValue(terrainValue, "Height Map", terrainData->GetHeightMapFile());
		//Alpha Map
		Json::SetValue(terrainValue, "Alpha Map 0", terrainData->GetAlphaMapFile(0));
		Json::SetValue(terrainValue, "Alpha Map 1", terrainData->GetAlphaMapFile(1));
	}

	//Textures
	string blank = "";
	for (UINT i = 0; i < TEXTURE_MAX; i++)
	{
		if (subDiffuse[i] == NULL)
			Json::SetValue(terrainValue, String::SInt("SubDiffuse%d", i), blank);
		else
			Json::SetValue(terrainValue, String::SInt("SubDiffuse%d", i), subDiffuse[i]->GetFile());

		//normals
		if (subNormal[i] == NULL)
			Json::SetValue(terrainValue, String::SInt("SubNormal%d", i), blank);
		else
			Json::SetValue(terrainValue, String::SInt("SubNormal%d", i), subNormal[i]->GetFile());
	}

	//EnemyArea
	{
		Json::Value enemyAreaValue;
		size_t count = enemyAreas.size();
		Json::SetValue(enemyAreaValue, "EnemyAreaCount", count);
		for (size_t i = 0; i < count; i++)
		{
			Json::SetValue(enemyAreaValue, String::SInt("EnemyArea%d-Name",  i), enemyAreas[i].Name);
			Json::SetValue(enemyAreaValue, String::SInt("EnemyArea%d-Start", i), enemyAreas[i].Start);
			Json::SetValue(enemyAreaValue, String::SInt("EnemyArea%d-Size",  i), enemyAreas[i].Size);
			Json::SetValue(enemyAreaValue, String::SInt("EnemyArea%d-Color", i), enemyAreas[i].Color);
		}
		terrainValue["EnemyArea"] = enemyAreaValue;
	}

	(*value)["Landscape"]["Terrain"] = terrainValue;
}

void Landscape::Terrain::Update(void)
{
	//if (values->editableFlag & (ULONG)ExcuteFlags::Terrain)
	if (values->GuiSettings->bShowLandscapeWindow)
	{
		UpdateMouse();
		UpdateKeyboard();
	}
	else
	{
		gridBuffer->Data.BrushType = 0;
		gridBuffer->Data.bView = 0;
	}

#if USE_TREE
	for (Tree* tree : trees)
	{
		tree->Update();
	}
#endif

	if (editmode == EDITMODE_ENEMYAREA)
	{
		EnemyArea blank;
		blank.Size = D3DXVECTOR2(0, 0);

		for (size_t i = 0; i < ENEMYAREA_MAX; i++)
		{
			if (i < enemyAreas.size() && enemyAreas[i].Show)
			{
				enemyAreaBuffer->SetArea(i, enemyAreas[i]);
			}
			else
			{
				enemyAreaBuffer->SetArea(i, blank);
			}
		}
	}
	else
	{
		EnemyArea blank;
		blank.Size = D3DXVECTOR2(0, 0);

		for (size_t i = 0; i < ENEMYAREA_MAX; i++)
		{
			enemyAreaBuffer->SetArea(i, blank);
		}
	}

}

void Landscape::Terrain::Render(void)
{
	if (values->renderFlag & (ULONG)ExcuteFlags::Terrain)
	{
		worldBuffer->SetVSBuffer(1);
		gridBuffer->SetPSBuffer(2);
		enemyAreaBuffer->SetPSBuffer(3);
		material->SetBuffer();

		Texture::SetShaderResources(5, TEXTURE_MAX, subDiffuse);
		Texture::SetShaderResources(11, TEXTURE_MAX, subNormal);

		gDC->PSSetSamplers(0, 1, &diffuseSampler);
		gDC->PSSetSamplers(1, 1, &detailSampler);

		terrainData->Render();
	}


#if USE_TREE
	for (Tree* tree : trees)
	{
		tree->Render();
	}
#endif
}

void Landscape::Terrain::PostRender(void)
{
	if (values->GuiSettings->bShowLandscapeWindow)
	{
		ImGui::Begin("Landscape", &values->GuiSettings->bShowLandscapeWindow);

		//Brush
		{
			ImGui::Text("Brush Settings");
			ImGui::SliderInt("Brush Type", (int*)&gridBuffer->Data.BrushType, BRUSHTYPE_NONE, BRUSHTYPE_MAX-1);
			ImGui::SliderFloat("Radius", &gridBuffer->Data.Radius, 0.1f, 20.0f);
			ImGui::SliderInt("Intensity", &intensity, 1, 100);

			ImGui::ColorEdit4("Color Hover", gridBuffer->Data.ColorHover, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
			ImGui::SameLine();
			ImGui::ColorEdit4("Color Selected", gridBuffer->Data.ColorSelected, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
		}

		ImGui::Separator();

		//Grid
		{
			ImGui::Text("Grid Settings");
			ImGui::Checkbox("Grid View", (bool*)&gridBuffer->Data.bView);
			ImGui::SameLine();
			ImGui::ColorEdit3("Grid Color", gridBuffer->Data.ColorGrid, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
			ImGui::SliderInt("Grid Spacing", (int*)&gridBuffer->Data.GridSpacing, 1, 10);
			ImGui::SliderFloat("Grid Thickness", &gridBuffer->Data.GridThickness, 0.01f, 1.0f);
		}

		ImGui::Separator();

		PostRenderEditMenu();

		ImGui::End();
	}
}

float Landscape::Terrain::GetVertexHeight(D3DXVECTOR3 position)
{
	return terrainData->GetHeight(position);
}


void Landscape::Terrain::MousePick(void)
{
	D3DXVECTOR3 start;
	D3DXVECTOR3 direction;
	values->MainCamera->GetPosition(&start);
	direction = values->MainCamera->GetDirection(values->Viewport, values->Perspective);

	D3DXVECTOR2 position;
	if (terrainData->MousePickked(start, direction, position))
	{
		gridBuffer->Data.Position = position;

		D3DXVECTOR2 hoveringGrid;
		hoveringGrid.x = (float)((int)position.x / gridBuffer->Data.GridSpacing);
		hoveringGrid.y = (float)((int)position.y / gridBuffer->Data.GridSpacing);

		gridBuffer->Data.HoverGridStart = hoveringGrid * (float)gridBuffer->Data.GridSpacing;
		gridBuffer->Data.HoverGridSize = D3DXVECTOR2(1, 1) * (float)gridBuffer->Data.GridSpacing;
	}
}

void Landscape::Terrain::UpdateMouse(void)
{
	switch (gridBuffer->Data.BrushType)
	{
		case BRUSHTYPE_NONE:
			return;
		break;
		case BRUSHTYPE_ROUNDBRUSH:
		{
			MousePick();

			switch (editmode)
			{
				case EDITMODE_ALPHATEXTURE:
				{
					if (gMouse->IsPress(MOUSE_LBUTTON))
					{
						if (!bChangeTerrain)
						{
							bChangeTerrain = true;
							terrainData->CopyVertexDataTo(&vertexDataOld);
						}

						if (gKeyboard->IsPress(VK_SHIFT))
						{
							terrainData->AdjustVertexAlpha(selectTexture, gridBuffer->Data.Position, gridBuffer->Data.Radius, -intensity);
						}
						else
						{
							terrainData->AdjustVertexAlpha(selectTexture, gridBuffer->Data.Position, gridBuffer->Data.Radius, intensity);
						}
					}
					else
					{
						if (bChangeTerrain)
						{
							TerrainVertexType* vertexDataNew = NULL;
							terrainData->CopyVertexDataTo(&vertexDataNew);

							function<void(TerrainVertexType*)> func = bind(&Landscape::Terrain::ChangeVertexData, this, placeholders::_1);
							gCmdManager->AddCommand(new CommandTerrainUpDown(func, vertexDataOld, vertexDataNew));

							vertexDataOld = NULL;
							bChangeTerrain = false;
						}
					}
				}
				break;
				case EDITMODE_HEIGHTUPDOWN:
				{
					if (gMouse->IsPress(MOUSE_LBUTTON))
					{
						if (!bChangeTerrain)
						{
							bChangeTerrain = true;
							terrainData->CopyVertexDataTo(&vertexDataOld);
						}

						if (gKeyboard->IsPress(VK_SHIFT))
						{
							terrainData->MoveVertexUpDown(gridBuffer->Data.Position, gridBuffer->Data.Radius, -intensity);
						}
						else
						{
							terrainData->MoveVertexUpDown(gridBuffer->Data.Position, gridBuffer->Data.Radius, intensity);
						}
					}
					else
					{
						if (bChangeTerrain)
						{
							TerrainVertexType* vertexDataNew = NULL;
							terrainData->CopyVertexDataTo(&vertexDataNew);

							function<void(TerrainVertexType*)> func = bind(&Landscape::Terrain::ChangeVertexData, this, placeholders::_1);
							gCmdManager->AddCommand(new CommandTerrainUpDown(func, vertexDataOld, vertexDataNew));

							vertexDataOld = NULL;
							bChangeTerrain = false;
						}
					}
				}
				break;
				case EDITMODE_HEIGHTSMOOTH:
				{
					if (gMouse->IsPress(MOUSE_LBUTTON))
					{
						if (!bChangeTerrain)
						{
							bChangeTerrain = true;
							terrainData->CopyVertexDataTo(&vertexDataOld);
						}

						terrainData->MoveVertexSmooth(gridBuffer->Data.Position, gridBuffer->Data.Radius, intensity);
					}
					else
					{
						if (bChangeTerrain)
						{
							TerrainVertexType* vertexDataNew = NULL;
							terrainData->CopyVertexDataTo(&vertexDataNew);

							function<void(TerrainVertexType*)> func = bind(&Landscape::Terrain::ChangeVertexData, this, placeholders::_1);
							gCmdManager->AddCommand(new CommandTerrainUpDown(func, vertexDataOld, vertexDataNew));

							vertexDataOld = NULL;
							bChangeTerrain = false;
						}
					}
				}
				break;
#if USE_TREE
				case EDITMODE_MAKETREE:
				{
					if (gMouse->IsDown(MOUSE_LBUTTON))
					{
						if (gKeyboard->IsPress(VK_SHIFT))
						{
							DeleteTrees(gridBuffer->Data.Position, gridBuffer->Data.Radius);
						}
						else
						{
							AddTrees(gridBuffer->Data.Position, gridBuffer->Data.Radius, intensity);
						}
					}
				}
				break;
#endif
			}
		}
		break;
		case BRUSHTYPE_GRID:
		{
			MousePick();

			switch (editmode)
			{
				case EDITMODE_ALPHATEXTURE:
				case EDITMODE_HEIGHTUPDOWN:
				case EDITMODE_HEIGHTSMOOTH:
#if USE_TREE
				case EDITMODE_MAKETREE:
#endif
				case EDITMODE_ENEMYAREA:
				{
					//Grid Select
					if (gMouse->IsDown(MOUSE_LBUTTON))
					{
						clickedGrid = gridBuffer->Data.HoverGridStart;
						gridBuffer->Data.SelectGridStart = gridBuffer->Data.HoverGridStart;
						gridBuffer->Data.SelectGridSize = D3DXVECTOR2(1, 1) * (float)gridBuffer->Data.GridSpacing;
					}
					else if (gMouse->IsPress(MOUSE_LBUTTON))
					{
						//gridBuffer->Data.SelectGridStart = gridBuffer->Data.HoverGrid;

						D3DXVECTOR2 size = Math::Abs(gridBuffer->Data.HoverGridStart - clickedGrid);
						size += D3DXVECTOR2(1, 1) * (float)gridBuffer->Data.GridSpacing;

						gridBuffer->Data.SelectGridSize = size;

						if (clickedGrid.x >= gridBuffer->Data.HoverGridStart.x)
						{
							gridBuffer->Data.SelectGridStart.x = gridBuffer->Data.HoverGridStart.x;
						}
						if (clickedGrid.y >= gridBuffer->Data.HoverGridStart.y)
						{
							gridBuffer->Data.SelectGridStart.y = gridBuffer->Data.HoverGridStart.y;
						}
					}
					else if (gMouse->IsDown(MOUSE_RBUTTON))
					{
						gridBuffer->Data.SelectGridSize = D3DXVECTOR2(0, 0);
					}
				}
				break;
				case EDITMODE_ASTARTEST:
				{
#if USE_ASTAR
					if (gMouse->IsDown(MOUSE_LBUTTON))
					{
						clickedGrid = gridBuffer->Data.HoverGridStart;
						gridBuffer->Data.SelectGridStart = gridBuffer->Data.HoverGridStart;
						gridBuffer->Data.SelectGridSize = D3DXVECTOR2(1, 1) * (float)gridBuffer->Data.GridSpacing;
					}
					else if (gMouse->IsPress(MOUSE_LBUTTON))
					{
						//gridBuffer->Data.SelectGridStart = gridBuffer->Data.HoverGrid;

						D3DXVECTOR2 size = Math::Abs(gridBuffer->Data.HoverGridStart - clickedGrid);
						size += D3DXVECTOR2(1, 1) * (float)gridBuffer->Data.GridSpacing;

						gridBuffer->Data.SelectGridSize = size;

						if (clickedGrid.x >= gridBuffer->Data.HoverGridStart.x)
						{
							gridBuffer->Data.SelectGridStart.x = gridBuffer->Data.HoverGridStart.x;
						}
						if (clickedGrid.y >= gridBuffer->Data.HoverGridStart.y)
						{
							gridBuffer->Data.SelectGridStart.y = gridBuffer->Data.HoverGridStart.y;
						}
					}
					else if (gMouse->IsDown(MOUSE_RBUTTON))
					{
						//CalcAStarPath()
						astar->ClearTiles();
						astar->SetTiles(gridBuffer->Data.SelectGridStart, gridBuffer->Data.HoverGridStart);
						astar->PathFinder();
					}
#endif
				}
				break;
			}

		}
		break;
	}
}

void Landscape::Terrain::UpdateKeyboard(void)
{
	switch (gridBuffer->Data.BrushType)
	{
		case BRUSHTYPE_NONE:
		break;
		case BRUSHTYPE_ROUNDBRUSH:
		break;
		case BRUSHTYPE_GRID:
		{
			if (gridBuffer->Data.SelectGridSize.x < D3DX_16F_EPSILON
				|| gridBuffer->Data.SelectGridSize.y < D3DX_16F_EPSILON)
				break;

			switch (editmode)
			{
				case EDITMODE_ALPHATEXTURE:
				{
					//키보드 UP, DOWN 으로 Height 수정
					if (gKeyboard->IsPress(VK_UP))
					{
						if (!bChangeTerrain)
						{
							bChangeTerrain = true;
							terrainData->CopyVertexDataTo(&vertexDataOld);
						}

						terrainData->AdjustVertexAlpha(selectTexture, gridBuffer->Data.SelectGridStart, gridBuffer->Data.SelectGridSize, intensity);
					}
					else if (gKeyboard->IsPress(VK_DOWN))
					{
						if (!bChangeTerrain)
						{
							bChangeTerrain = true;
							terrainData->CopyVertexDataTo(&vertexDataOld);
						}

						terrainData->AdjustVertexAlpha(selectTexture, gridBuffer->Data.SelectGridStart, gridBuffer->Data.SelectGridSize, -intensity);
					}
					else
					{
						if (bChangeTerrain)
						{
							TerrainVertexType* vertexDataNew = NULL;
							terrainData->CopyVertexDataTo(&vertexDataNew);

							function<void(TerrainVertexType*)> func = bind(&Landscape::Terrain::ChangeVertexData, this, placeholders::_1);
							gCmdManager->AddCommand(new CommandTerrainUpDown(func, vertexDataOld, vertexDataNew));

							vertexDataOld = NULL;

							bChangeTerrain = false;
						}
					}
				}
				break;
				case EDITMODE_HEIGHTUPDOWN:
				{
					//키보드 UP, DOWN 으로 Height 수정
					if (gKeyboard->IsPress(VK_UP))
					{
						if (!bChangeTerrain)
						{
							bChangeTerrain = true;
							terrainData->CopyVertexDataTo(&vertexDataOld);
						}

						terrainData->MoveVertexUpDown(gridBuffer->Data.SelectGridStart, gridBuffer->Data.SelectGridSize, intensity);
					}
					else if (gKeyboard->IsPress(VK_DOWN))
					{
						if (!bChangeTerrain)
						{
							bChangeTerrain = true;
							terrainData->CopyVertexDataTo(&vertexDataOld);
						}

						terrainData->MoveVertexUpDown(gridBuffer->Data.SelectGridStart, gridBuffer->Data.SelectGridSize, -intensity);
					}
					else
					{
						if (bChangeTerrain)
						{
							TerrainVertexType* vertexDataNew = NULL;
							terrainData->CopyVertexDataTo(&vertexDataNew);

							function<void(TerrainVertexType*)> func = bind(&Landscape::Terrain::ChangeVertexData, this, placeholders::_1);
							gCmdManager->AddCommand(new CommandTerrainUpDown(func, vertexDataOld, vertexDataNew));

							vertexDataOld = NULL;
							bChangeTerrain = false;
						}
					}
				}
				break;
				case EDITMODE_HEIGHTSMOOTH:
					break;
#if USE_TREE
				case EDITMODE_MAKETREE:
				{
					if (gKeyboard->IsDown(VK_RETURN))
					{
						AddTrees(gridBuffer->Data.SelectGridStart, gridBuffer->Data.SelectGridSize, intensity);
					}
					if (gKeyboard->IsDown(VK_DELETE))
					{
						DeleteTrees(gridBuffer->Data.SelectGridStart, gridBuffer->Data.SelectGridSize);
					}
				}
#endif
				break;
				case EDITMODE_ENEMYAREA:
				{
					if (gKeyboard->IsDown(VK_RETURN))
					{
						if (enemyAreas.size() > 0 && showAreaIndex < enemyAreas.size())
						{
							enemyAreas[showAreaIndex].Start = gridBuffer->Data.SelectGridStart;
							enemyAreas[showAreaIndex].Size = gridBuffer->Data.SelectGridSize;
						}
					}
				}
				break;
				case EDITMODE_ASTARTEST:
				{
#if USE_ASTAR
					if (gKeyboard->IsDown(VK_CONTROL, '1'))
					{
						terrainData->SetAttribute(SellAttribute::NONE, gridBuffer->Data.SelectGridStart, gridBuffer->Data.SelectGridSize);
						astar->UpdateMapInfo();
					}
					if (gKeyboard->IsDown(VK_CONTROL, '2'))
					{
						terrainData->SetAttribute(SellAttribute::MOVE_DISABLE, gridBuffer->Data.SelectGridStart, gridBuffer->Data.SelectGridSize);
						astar->UpdateMapInfo();
					}
#endif
				}
				break;
			}//switch (editmode)
		}
		break;
	}//switch (gridBuffer->Data.BrushType)
}

void Landscape::Terrain::PostRenderEditMenu(void)
{
	switch (editmode)
	{
		case EDITMODE_ALPHATEXTURE:	ImGui::Text("EDITMODE_ALPHATEXTURE");	break;
		case EDITMODE_HEIGHTUPDOWN:	ImGui::Text("EDITMODE_HEIGHTUPDOWN");	break;
		case EDITMODE_HEIGHTSMOOTH:	ImGui::Text("EDITMODE_HEIGHTSMOOTH");	break;
#if USE_TREE
		case EDITMODE_MAKETREE:		ImGui::Text("EDITMODE_MAKETREE");		break;
#endif
		case EDITMODE_ENEMYAREA:	ImGui::Text("EDITMODE_ENEMYAREA");		break;
		case EDITMODE_ASTARTEST:	ImGui::Text("EDITMODE_ASTARTEST");		break;
	}
	ImGui::SliderInt("Edit Mode", (int*)&editmode, EDITMODE_ALPHATEXTURE, EDITMODE_MAX - 1);

	wstring shaderFile = L"Load : ";
	shaderFile += material->GetShader()->GetFile();
	if (ImGui::Button(String::ToString(shaderFile).c_str()))
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		function<void(wstring)> f = bind(&Landscape::Terrain::ChangeShader, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::ShaderFilter, Shaders, f, desc.Handle);
	}


	if (editmode == EDITMODE_ALPHATEXTURE)
	{
		PostRenderTextures();
	}
	else if (editmode == EDITMODE_HEIGHTUPDOWN
			|| editmode == EDITMODE_HEIGHTSMOOTH)
	{
		PostRenderHeightMap();
	}
	else if (editmode == EDITMODE_ENEMYAREA)
	{
		PostRenderEnemyArea();
	}
}

void Landscape::Terrain::PostRenderHeightMap(void)
{
	ImGui::Text("Height Map");
	ImGui::SameLine();
	if (ImGui::Button("Clear Height"))
	{
		terrainData->ClearVertexHeight();
	}

	if (ImGui::Button("Load Height"))//, ImVec2(30, 30)))
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);
		function<void(wstring)> func = bind(&Landscape::TerrainData::SetHeightMapFile, terrainData, placeholders::_1, 10.0f);
		Path::OpenFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
	
		//선택한것 해제
	
	}
	ImGui::SameLine();
	if (ImGui::Button("Save Height"))//, ImVec2(30, 30)))
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);
		function<void(wstring)> func = bind(&Landscape::TerrainData::SaveHeightMap, terrainData, placeholders::_1);
		Path::SaveFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
	}
}

void Landscape::Terrain::PostRenderTextures(void)
{
	//Diffuse Map
	{
		ImGui::Text("Diffuse Map");
		wstring wstr = material->GetDiffuseMap()->GetFile();
		String::Replace(&wstr, Contents, L"");
		string str = "Load : " + String::ToString(wstr);
		if (ImGui::Button(str.c_str()))
		{
			D3DDesc desc;
			D3D::GetDesc(&desc);
			function<void(wstring)> func = bind(&Landscape::Terrain::ChangeDiffuseMap, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
		}
	}
	//Normal Map
	{
		ImGui::Text("Normal Map");
		wstring wstr = L"";
		if (material->GetNormalMap())
		{
			wstr = material->GetNormalMap()->GetFile();
			String::Replace(&wstr, Contents, L"");
		}
		string str = "Load : " + String::ToString(wstr);
		if (ImGui::Button(str.c_str()))
		{
			D3DDesc desc;
			D3D::GetDesc(&desc);
			function<void(wstring)> func = bind(&Landscape::Terrain::ChangeNormalMap, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
		}
	}


	//Alpha Map
	{
		ImGui::Text("Alpha Map");
		ImGui::SameLine();
		if (ImGui::Button("Clear Alpha"))
		{
			terrainData->ClearVertexAlpha();
		}

		for (UINT i = 0; i < ALPHAMAP_MAX; i++)
		{
			char num[10];
			sprintf(num, "%d", i + 1);

			string str = string("Load Alpha Map") + string(num);
			if (ImGui::Button(str.c_str()))
			{
				D3DDesc desc;
				D3D::GetDesc(&desc);
				function<void(wstring)> func = bind(&Landscape::TerrainData::SetAlphaMapFile, terrainData, i, placeholders::_1);
				Path::OpenFileDialog(L"", Path::ImageFilter, Textures, func, desc.Handle);
			}
			ImGui::SameLine();
			str = string("Save Alpha Map") + string(num);
			if (ImGui::Button(str.c_str()))
			{
				D3DDesc desc;
				D3D::GetDesc(&desc);
				function<void(wstring)> func = bind(&Landscape::TerrainData::SaveAlphaMap, terrainData, i, placeholders::_1);
				Path::SaveFileDialog(L"", Path::ImageFilter, Textures, func, desc.Handle);
			}
		}
	}

	//subDiffuse
	{
		ImGui::Text("SubDiffuse");
		for (UINT i = 0; i < TEXTURE_MAX; i++)
		{
			bool bSelect = (selectTexture & (1 << i)) == (1 << i);

			//if (editmode == EDITMODE_ALPHATEXTURE)
			{
				if (ImGui::Checkbox(String::SInt("%d", i+1).c_str(), &bSelect))
				{
					selectTexture = bSelect ? (1 << i) : 0;
				}
				ImGui::SameLine();
			}

			if (subDiffuse[i] == NULL)
			{
				string str = String::SInt("Load SubDiffuse%d", i + 1);
				if (ImGui::Button(str.c_str()))
				{
					D3DDesc desc;
					D3D::GetDesc(&desc);
					function<void(wstring)> func = bind(&Landscape::Terrain::ChangeSubDiffuse, this, i, placeholders::_1);
					Path::OpenFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
				}
			}
			else
			{
				wstring wstr = subDiffuse[i]->GetFile();
				String::Replace(&wstr, Contents, L"");
				string str = "Load : " + String::ToString(wstr);
				if (ImGui::Button(str.c_str()))
				{
					D3DDesc desc;
					D3D::GetDesc(&desc);
					function<void(wstring)> func = bind(&Landscape::Terrain::ChangeSubDiffuse, this, i, placeholders::_1);
					Path::OpenFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					ChangeSubDiffuse(i, L"");
				}
			}
		}
	}

	//subNormal
	{
		ImGui::Text("SubNormal");
		for (UINT i = 0; i < TEXTURE_MAX; i++)
		{
			if (subNormal[i] == NULL)
			{
				string str = String::SInt("Load SubNormal%d", i + 1);
				if (ImGui::Button(str.c_str()))
				{
					D3DDesc desc;
					D3D::GetDesc(&desc);
					function<void(wstring)> func = bind(&Landscape::Terrain::ChangeSubNormal, this, i, placeholders::_1);
					Path::OpenFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
				}
			}
			else
			{
				wstring wstr = subNormal[i]->GetFile();
				String::Replace(&wstr, Contents, L"");
				string str = "Load : " + String::ToString(wstr);
				if (ImGui::Button(str.c_str()))
				{
					D3DDesc desc;
					D3D::GetDesc(&desc);
					function<void(wstring)> func = bind(&Landscape::Terrain::ChangeSubNormal, this, i, placeholders::_1);
					Path::OpenFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					ChangeSubNormal(i, L"");
				}
			}
		}
	}

	ImGui::Separator();
	//Total Texture
	{
		if (ImGui::Button("Create Total Diffuse"))
		{
			D3DDesc desc;
			D3D::GetDesc(&desc);
			function<void(wstring)> func = bind(&Landscape::Terrain::CreateTotalDiffuse, this, placeholders::_1);
			Path::SaveFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
		}
		if (ImGui::Button("Create Total Normal"))
		{
			D3DDesc desc;
			D3D::GetDesc(&desc);
			function<void(wstring)> func = bind(&Landscape::Terrain::CreateTotalNormal, this, placeholders::_1);
			Path::SaveFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
		}
	}


}

void Landscape::Terrain::PostRenderEnemyArea(void)
{
	size_t count = enemyAreas.size();
	string list;
	for (size_t i = 0; i < count; i++)
	{
		list += enemyAreas[i].Name.c_str();
		list.push_back('\0');
	}
	ImGui::Combo("Areas", &showAreaIndex, list.c_str());

	if (count > 0)
	{
		char name[128] = "";
		strcpy_s(name, 128, enemyAreas[showAreaIndex].Name.c_str());
		if (ImGui::InputText("Name", name, 128))
		{
			enemyAreas[showAreaIndex].Name = name;
		}
		ImGui::DragFloat2("Start XZ", enemyAreas[showAreaIndex].Start);
		ImGui::DragFloat2("Size XZ", enemyAreas[showAreaIndex].Size);
		ImGui::ColorEdit3("Color", enemyAreas[showAreaIndex].Color);
		ImGui::Checkbox("Show", &enemyAreas[showAreaIndex].Show);
	}

	if (ImGui::Button("Add Area"))
	{
		if (enemyAreas.size() < ENEMYAREA_MAX)
		{
			EnemyArea area;
			area.Start = gridBuffer->Data.SelectGridStart;
			area.Size = gridBuffer->Data.SelectGridSize;
			enemyAreas.push_back(area);
			showAreaIndex = enemyAreas.size() - 1;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		enemyAreas.erase(enemyAreas.begin() + showAreaIndex);
		if (showAreaIndex >= enemyAreas.size())
		{
			showAreaIndex = 0;
		}
	}
}

#if USE_TREE

void Landscape::Terrain::AddTrees(D3DXVECTOR2 startXZ, D3DXVECTOR2 sizeXZ, int intensity)
{
	for (UINT i = 0; i < (UINT)intensity; i++)
	{
		D3DXVECTOR3 position;
		position.x = Math::Random(startXZ.x, startXZ.x + sizeXZ.x);
		position.z = Math::Random(startXZ.y, startXZ.y + sizeXZ.y);

		D3DXVECTOR3 scale;
		scale.x = Math::Random(10.0f, 20.0f);
		scale.y = Math::Random(10.0f, 20.0f);
		scale.z = 1.0f;

		position.y = GetVertexHeight(position) + (scale.y * 0.5f);

		Tree* tree = new Tree(values);
		tree->GetTransform()->Position = position;
		tree->GetTransform()->Scale = scale;
		tree->SetFiexedY(true);

		//bool bFixedY = false;
		//bFixedY = Math::Random(0, 1) == 1 ? true : false;
		//tree->SetFiexedY(bFixedY);

		trees.push_back(tree);
	}
}

void Landscape::Terrain::AddTrees(D3DXVECTOR2 positionXZ, float radius, int intensity)
{
	for (UINT i = 0; i < (UINT)intensity; i++)
	{
		D3DXVECTOR3 position;
		float radian = Math::Random(0.0f, (float)D3DX_PI);
		position.x = positionXZ.x + radius * cosf(radian);
		position.z = positionXZ.y + radius * sinf(radian);

		D3DXVECTOR3 scale;
		scale.x = Math::Random(10.0f, 20.0f);
		scale.y = Math::Random(10.0f, 20.0f);
		scale.z = 1.0f;

		position.y = GetVertexHeight(position) + (scale.y * 0.5f);

		Tree* tree = new Tree(values);
		tree->GetTransform()->Position = position;
		tree->GetTransform()->Scale = scale;
		tree->SetFiexedY(true);

		//bool bFixedY = false;
		//bFixedY = Math::Random(0, 1) == 1 ? true : false;
		//tree->SetFiexedY(bFixedY);

		trees.push_back(tree);
	}
}

void Landscape::Terrain::DeleteTrees(D3DXVECTOR2 startXZ, D3DXVECTOR2 sizeXZ)
{
	for (UINT i = 0; i < trees.size();)
	{
		D3DXVECTOR3 pos = trees[i]->GetTransform()->Position;
		if (pos.x >= startXZ.x && pos.x <= startXZ.x + sizeXZ.x
			&& pos.z >= startXZ.y && pos.z <= startXZ.y + sizeXZ.y)
		{
			trees.erase(trees.begin() + i);
		}
		else ++i;
	}
}

void Landscape::Terrain::DeleteTrees(D3DXVECTOR2 positionXZ, float radius)
{
	float dist = 0.0f;
	for (UINT i = 0; i < trees.size();)
	{
		D3DXVECTOR3 pos = trees[i]->GetTransform()->Position;
		dist = sqrt(pow(pos.x - positionXZ.x, 2) + pow(pos.z - positionXZ.y, 2));

		if (dist < radius)
		{
			trees.erase(trees.begin() + i);
		}
		else ++i;
	}
}

#endif

void Landscape::Terrain::CreateTotalDiffuse(wstring filename)
{
	terrainData->CreateTotalDiffuse(filename, material->GetDiffuseMap(), subDiffuse);
}

void Landscape::Terrain::CreateTotalNormal(wstring filename)
{
	terrainData->CreateTotalNormal(filename, material->GetNormalMap(), subNormal);
}

void Landscape::Terrain::ChangeShader(wstring filename)
{
	material->SetShader(filename);
}

void Landscape::Terrain::ChangeVertexData(TerrainVertexType* vertexData)
{
	assert(terrainData != NULL);

	terrainData->CopyVertexDataFrom(vertexData);
}

void Landscape::Terrain::ChangeDiffuseMap(wstring filename)
{
	assert(material != NULL);

	material->SetDiffuseMap(filename);
}

void Landscape::Terrain::ChangeNormalMap(wstring filename)
{
	assert(material != NULL);

	material->SetNormalMap(filename);
}

void Landscape::Terrain::ChangeSubDiffuse(UINT index, wstring filename)
{
	assert(index < TEXTURE_MAX);

	Texture* pTexture = filename.size() == 0 ? NULL : new Texture(filename);
	
	SAFE_DELETE(subDiffuse[index]);

	subDiffuse[index] = pTexture;
}

void Landscape::Terrain::ChangeSubNormal(UINT index, wstring filename)
{
	assert(index < TEXTURE_MAX);

	Texture* pTexture = filename.size() == 0 ? NULL : new Texture(filename);

	SAFE_DELETE(subNormal[index]);

	subNormal[index] = pTexture;
}

#if USE_ASTAR
void Landscape::Terrain::CalcAStarPath(Model::BinModel* model, float* movableHeight)
{
	D3DXVECTOR3 modelPos = model->GetTransformPosition();

	MousePick();

	//소수점 자르기. 나중에 단위가 1이 아니라면 수정해야함
	modelPos.x = float(int(modelPos.x));
	modelPos.z = float(int(modelPos.z));
	D3DXVECTOR2 gridStart = D3DXVECTOR2(modelPos.x, modelPos.z);
	D3DXVECTOR2 gridEnd = gridBuffer->Data.HoverGridStart;

	astar->RequestPathFind(model, gridStart, gridEnd, movableHeight);
}

void Landscape::Terrain::CalcAStarPath(Model::BinModel * model, D3DXVECTOR2 targetGrid, float * movableHeight)
{
	D3DXVECTOR3 modelPos = model->GetTransformPosition();

	//소수점 자르기. 나중에 단위가 1이 아니라면 수정해야함
	modelPos.x = float(int(modelPos.x));
	modelPos.z = float(int(modelPos.z));
	D3DXVECTOR2 gridStart = D3DXVECTOR2(modelPos.x, modelPos.z);
	D3DXVECTOR2 gridEnd = targetGrid;

	astar->RequestPathFind(model, gridStart, gridEnd, movableHeight);
}
#endif


D3DXVECTOR2 Landscape::Terrain::GetHoveringGrid(void)
{
	MousePick();

	return gridBuffer->Data.HoverGridStart;
}

