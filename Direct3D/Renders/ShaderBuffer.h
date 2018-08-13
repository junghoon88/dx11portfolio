#pragma once
#include "stdafx.h"

class ShaderBuffer
{
public:
	void SetVSBuffer(UINT slot)
	{
		MapData(data, dataSize);

		gDC->VSSetConstantBuffers(slot, 1, &buffer);
	}

	void SetPSBuffer(UINT slot)
	{
		MapData(data, dataSize);

		gDC->PSSetConstantBuffers(slot, 1, &buffer);
	}

protected:
	ShaderBuffer(void* data, UINT dataSize)
		: data(data), dataSize(dataSize)
	{
		D3D11_BUFFER_DESC desc;
		desc.Usage				 = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth			 = dataSize;
		desc.BindFlags			 = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags		 = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags			 = 0;
		desc.StructureByteStride = 0;

		HRESULT hr = gDevice->CreateBuffer(&desc, NULL, &buffer);
		assert(SUCCEEDED(hr));
	}

	virtual ~ShaderBuffer()
	{
		SAFE_RELEASE(buffer);
	}

private:
	void MapData(void* data, UINT dataSize)
	{
		D3D11_MAPPED_SUBRESOURCE subResource;

		HRESULT hr = gDC->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		assert(SUCCEEDED(hr));

		memcpy(subResource.pData, data, dataSize);

		gDC->Unmap(buffer, 0);
	}

	ID3D11Buffer* buffer;

	void* data;
	UINT dataSize;
};