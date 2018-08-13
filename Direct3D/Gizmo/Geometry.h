#pragma once

namespace Gizmo
{
	class GizmoModel
	{
	public:
		GizmoModel(vector<D3DXVECTOR3>& positions, vector<D3DXVECTOR3>& normals, vector<UINT>& indices);


	public:
		inline const vector<VertexTextureNormal> GetVirtices(void) { return Vertices; }
		inline const vector<UINT> GetIndices(void) { return Indices; }


	private:
		vector<VertexTextureNormal> Vertices;
		vector<UINT>				Indices;


	};

	class Geometry
	{
	public:
		Geometry();
		~Geometry();

	public:
		static GizmoModel* Rotate;
		static GizmoModel* Translate;
		static GizmoModel* Scale;
	};


}
