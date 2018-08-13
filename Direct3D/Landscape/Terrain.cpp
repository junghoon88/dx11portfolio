#include "stdafx.h"
#include "Terrain.h"
#include "TerrainData.h"
//#include "Tree.h"
//#include "AStar.h"

#include "../Command/CommandTerrain.h"

#include "../Utilities/Xml.h"

#define USE_ASTAR 0
#define USE_TREE 0

Landscape::Terrain::Terrain(ExecuteValues* values)
	: values(values)
	, material(NULL)
	, terrainData(NULL)
	, editmode(EDITMODE_HEIGHTUPDOWN)
	, intensity(1)
	, vertexDataOld(NULL)
	, bChangeTerrain(false)
{
	worldBuffer = new WorldBuffer();
	gridBuffer  = new GridBuffer();
	
	D3D11_SAMPLER_DESC samplerDesc;
	States::GetSamplerDesc(&samplerDesc);
	States::CreateSampler(&samplerDesc, &diffuseSampler);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	States::CreateSampler(&samplerDesc, &detailSampler);

	selectTexture = 0;
	ZeroMemory(&textures, sizeof(Texture*) * TEXTURE_MAX);

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

	SaveXmlTerrain(LevelDatas + L"Terrain.data");

	for (UINT i = 0; i < TEXTURE_MAX; i++)
	{
		SAFE_DELETE(textures[i]);
	}

	SAFE_RELEASE(diffuseSampler);
	SAFE_RELEASE(detailSampler);

	SAFE_DELETE(terrainData);

	SAFE_DELETE(gridBuffer);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(material);
}

void Landscape::Terrain::LoadData(Json::Value * value)
{
	wstring shaderFile = Shaders + L"Terrain.hlsl";
	wstring diffuseMap = Contents + L"Landscape/Dirt3.png";
	wstring detailMap = Contents + L"Landscape/Dirt3_detail.png";

	wstring heightMap = Contents + L"Landscape/HeightMap.png";
	wstring alphaMap[ALPHAMAP_MAX] = { L"", L"" };
	wstring alphaTextures[TEXTURE_MAX] = { L"", L"", L"", L"", L"", L"" };

	Json::Value terrainValue = (*value)["Landscape"]["Terrain"];
	if (terrainValue.isNull() == false)
	{
		//Material
		{
			//Shader
			Json::GetValue(terrainValue, "Shader File", shaderFile);
			//Diffuse Map
			Json::GetValue(terrainValue, "Diffuse Map", diffuseMap);
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
			Json::GetValue(terrainValue, String::SInt("Textures%d", i), alphaTextures[i]);
		}
	}

	material = new Material(shaderFile);
	material->SetDiffuseMap(diffuseMap);
	material->SetDetailMap(detailMap);

	terrainData = new TerrainData(heightMap);

	for (UINT i = 0; i < ALPHAMAP_MAX; i++)
	{
		if (alphaMap[i].length() > 0)
			terrainData->SetAlphaMapFile(i, alphaMap[i]);
	}
	
	
	for (UINT i = 0; i < TEXTURE_MAX; i++)
	{
		if (alphaTextures[i].length() > 0)
			ChangeTexture(i, alphaTextures[i]);
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
	for (UINT i = 0; i < TEXTURE_MAX; i++)
	{
		if (textures[i] == NULL)
		{
			string blank = "";
			Json::SetValue(terrainValue, String::SInt("Textures%d", i), blank);
			continue;
		}
		Json::SetValue(terrainValue, String::SInt("Textures%d", i), textures[i]->GetFile());
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
}

void Landscape::Terrain::Render(void)
{
	if (values->renderFlag & (ULONG)ExcuteFlags::Terrain)
	{
		worldBuffer->SetVSBuffer(1);
		gridBuffer->SetPSBuffer(2);
		material->SetBuffer();

		Texture::SetShaderResources(5, TEXTURE_MAX, textures);

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
							VertexTypePTNC2* vertexDataNew = NULL;
							terrainData->CopyVertexDataTo(&vertexDataNew);

							function<void(VertexTypePTNC2*)> func = bind(&Landscape::Terrain::ChangeVertexData, this, placeholders::_1);
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
							VertexTypePTNC2* vertexDataNew = NULL;
							terrainData->CopyVertexDataTo(&vertexDataNew);

							function<void(VertexTypePTNC2*)> func = bind(&Landscape::Terrain::ChangeVertexData, this, placeholders::_1);
							gCmdManager->AddCommand(new CommandTerrainUpDown(func, vertexDataOld, vertexDataNew));

							vertexDataOld = NULL;
							bChangeTerrain = false;
						}
					}
				}
				break;
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
							VertexTypePTNC2* vertexDataNew = NULL;
							terrainData->CopyVertexDataTo(&vertexDataNew);

							function<void(VertexTypePTNC2*)> func = bind(&Landscape::Terrain::ChangeVertexData, this, placeholders::_1);
							gCmdManager->AddCommand(new CommandTerrainUpDown(func, vertexDataOld, vertexDataNew));

							vertexDataOld = NULL;
							bChangeTerrain = false;
						}
					}
				}
				break;
				case EDITMODE_MAKETREE:
				{
#if USE_TREE
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
#endif
				}
				break;
			}
		}
		break;
		case BRUSHTYPE_GRID:
		{
			MousePick();

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

			switch (editmode)
			{
				case EDITMODE_HEIGHTUPDOWN:
				case EDITMODE_HEIGHTSMOOTH:
				case EDITMODE_ALPHATEXTURE:
				case EDITMODE_MAKETREE:
				{
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
							VertexTypePTNC2* vertexDataNew = NULL;
							terrainData->CopyVertexDataTo(&vertexDataNew);

							function<void(VertexTypePTNC2*)> func = bind(&Landscape::Terrain::ChangeVertexData, this, placeholders::_1);
							gCmdManager->AddCommand(new CommandTerrainUpDown(func, vertexDataOld, vertexDataNew));

							vertexDataOld = NULL;
							bChangeTerrain = false;
						}
					}
				}
				break;
				case EDITMODE_HEIGHTSMOOTH:
				break;
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
							VertexTypePTNC2* vertexDataNew = NULL;
							terrainData->CopyVertexDataTo(&vertexDataNew);

							function<void(VertexTypePTNC2*)> func = bind(&Landscape::Terrain::ChangeVertexData, this, placeholders::_1);
							gCmdManager->AddCommand(new CommandTerrainUpDown(func, vertexDataOld, vertexDataNew));

							vertexDataOld = NULL;
							bChangeTerrain = false;
						}
					}
				}
				break;
				case EDITMODE_MAKETREE:
				{
#if USE_TREE
					if (gKeyboard->IsDown(VK_RETURN))
					{
						AddTrees(gridBuffer->Data.SelectGridStart, gridBuffer->Data.SelectGridSize, intensity);
					}
					if (gKeyboard->IsDown(VK_DELETE))
					{
						DeleteTrees(gridBuffer->Data.SelectGridStart, gridBuffer->Data.SelectGridSize);
					}
#endif
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
		case EDITMODE_HEIGHTUPDOWN:	ImGui::Text("EDITMODE_HEIGHTUPDOWN");	break;
		case EDITMODE_HEIGHTSMOOTH:	ImGui::Text("EDITMODE_HEIGHTSMOOTH");	break;
		case EDITMODE_ALPHATEXTURE:	ImGui::Text("EDITMODE_ALPHATEXTURE");	break;
		case EDITMODE_MAKETREE:		ImGui::Text("EDITMODE_MAKETREE");		break;
		case EDITMODE_ASTARTEST:	ImGui::Text("EDITMODE_ASTARTEST");		break;
	}
	ImGui::SliderInt("Edit Mode", (int*)&editmode, EDITMODE_HEIGHTUPDOWN, EDITMODE_MAX - 1);

	//Height Map
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

	//Textures
	{
		ImGui::Text("Textures");
		for (UINT i = 0; i < TEXTURE_MAX; i++)
		{
			bool bSelect = (selectTexture & (1 << i)) == (1 << i);
			char num[10];
			sprintf(num, "%d", i+1);

			if (editmode == EDITMODE_ALPHATEXTURE)
			{
				if (ImGui::Checkbox(num, &bSelect))
				{
					selectTexture = bSelect ? (1 << i) : 0;
				}
				ImGui::SameLine();
			}

			if (textures[i] == NULL)
			{
				string str = string("Load Texture") + string(num);
				if (ImGui::Button(str.c_str()))
				{
					D3DDesc desc;
					D3D::GetDesc(&desc);
					function<void(wstring)> func = bind(&Landscape::Terrain::ChangeTexture, this, i, placeholders::_1);
					Path::OpenFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
				}
			}
			else
			{
				wstring wstr = textures[i]->GetFile();
				String::Replace(&wstr, Contents, L"");
				string str = "Load : " + String::ToString(wstr);
				if (ImGui::Button(str.c_str()))
				{
					D3DDesc desc;
					D3D::GetDesc(&desc);
					function<void(wstring)> func = bind(&Landscape::Terrain::ChangeTexture, this, i, placeholders::_1);
					Path::OpenFileDialog(L"", Path::ImageFilter, Landscapes, func, desc.Handle);
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					ChangeTexture(i, L"");
				}
			}
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
		tree->SetTransformPosition(position);
		tree->SetTransformScale(scale);
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
		tree->SetTransformPosition(position);
		tree->SetTransformScale(scale);
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
		D3DXVECTOR3 pos = trees[i]->GetTransformPosition();
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
		D3DXVECTOR3 pos = trees[i]->GetTransformPosition();
		dist = sqrt(pow(pos.x - positionXZ.x, 2) + pow(pos.z - positionXZ.y, 2));

		if (dist < radius)
		{
			trees.erase(trees.begin() + i);
		}
		else ++i;
	}
}

#endif

void Landscape::Terrain::SaveJsonTerrain(wstring filename)
{
}

void Landscape::Terrain::LoadJsonTerrain(wstring filename)
{
}

void Landscape::Terrain::SaveXmlTerrain(wstring filename)
{
	Xml::XMLDocument* doc = new Xml::XMLDocument();
	XmlElement* element = NULL;
	string textValue;

	//Material
	{
		vector<XmlElement*> FirstElements;
		element = doc->NewElement("Material");
		FirstElements.push_back(element);
		{
			XmlElement* parent = element;
			vector<XmlElement*> SecondElements;

			//Shader
			element = doc->NewElement("Shader");
			textValue = String::ToString(material->GetShader()->GetFile());
			element->SetText(textValue.c_str());
			SecondElements.push_back(element);

			//DiffuseMap
			element = doc->NewElement("DiffuseMap");
			textValue = String::ToString(material->GetDiffuseMap()->GetFile());
			element->SetText(textValue.c_str());
			SecondElements.push_back(element);

			//DetailMap
			element = doc->NewElement("DetailMap");
			textValue = String::ToString(material->GetDetailMap()->GetFile());
			element->SetText(textValue.c_str());
			SecondElements.push_back(element);


			parent->InsertFirstChild(SecondElements[0]);
			for (size_t i = 0; i < SecondElements.size() - 1; i++)
				parent->InsertAfterChild(SecondElements[i], SecondElements[i + 1]);
		}

		element = doc->NewElement("HeightMap");
		textValue = String::ToString(terrainData->GetHeightMapFile());
		element->SetText(textValue.c_str());
		FirstElements.push_back(element);

		doc->InsertFirstChild(FirstElements[0]);
		for (size_t i = 0; i < FirstElements.size() - 1; i++)
			doc->InsertAfterChild(FirstElements[i], FirstElements[i + 1]);

		//textures
	}
	
	

	string path = String::ToString(filename);
	Xml::XMLError error = doc->SaveFile(path.c_str());
	assert(error == Xml::XML_SUCCESS);

	SAFE_DELETE(doc);
}

void Landscape::Terrain::LoadXmlTerrain(wstring filename)
{
}

void Landscape::Terrain::ChangeVertexData(VertexTypePTNC2* vertexData)
{
	assert(terrainData != NULL);

	terrainData->CopyVertexDataFrom(vertexData);
}

void Landscape::Terrain::ChangeDiffuseMap(wstring filename)
{
	assert(material != NULL);

	material->SetDiffuseMap(filename);
}

void Landscape::Terrain::ChangeTexture(UINT index, wstring filename)
{
	assert(index < TEXTURE_MAX);

	Texture* pTexture = filename.size() == 0 ? NULL : new Texture(filename);
	
	SAFE_DELETE(textures[index]);

	textures[index] = pTexture;
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

