#pragma once

#include "../Utility.h"
#include "../DataTypes.h"

class CShaderBase
{

};

class CVertexShaderBase : public CShaderBase
{
public:
	Mat4 ModelMat;

	virtual void ProcessVertex(SVertexRuntime* pVertex) = 0;

};

class CFragmentShaderBase : public CShaderBase
{
public:
	virtual Color4F ProcessFragment(SFragment* pFragment) = 0;

	vector<CMaterial*> m_vMaterial;
};

class CMeshVertexShader : public CVertexShaderBase
{
public:
	virtual void ProcessVertex(SVertexRuntime* pVertex) override;
};

class CMeshFragmentShader : public CFragmentShaderBase
{
public:
	virtual Color4F ProcessFragment(SFragment* pFragment) override;
};

class CSkinMeshVertexShader : public CVertexShaderBase
{
public:
	virtual void ProcessVertex(SVertexRuntime* pVertex) override;

	Vec4* m_vMatrixPallet;
};
