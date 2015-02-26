#include "DataTypes.h"

void SMeshData::WriteToFile(FILE* hFile)
{
	int iMeshNameSize = m_MeshName.size();
	fwrite(&iMeshNameSize, sizeof(int), 1, hFile);
	fwrite(m_MeshName.c_str(), iMeshNameSize, 1, hFile);

	int iSubTextureNum = m_cMaterial.m_SubTextureVec.size();
	fwrite(&iSubTextureNum, sizeof(int), 1, hFile);
	for (auto& rTex : m_cMaterial.m_SubTextureVec)
	{  
		int iTexNameSize = rTex.m_sFileName.size();
		fwrite(&iTexNameSize, sizeof(int), 1, hFile);
		fwrite(rTex.m_sFileName.c_str(), iTexNameSize, 1, hFile);
	}

	int iVertexCount = m_vVectex.size();
	fwrite(&iVertexCount, sizeof(int), 1, hFile);  

	int iIndexCount = m_vFace.size();
	fwrite(&iIndexCount, sizeof(int), 1, hFile);  

	if(iVertexCount > 0)  
	{  
		fwrite(&m_vVectex.front(), sizeof(SVertex), m_vVectex.size(), hFile);  
	}  

	if(iIndexCount > 0 )  
	{  
		fwrite(&m_vFace.front(), sizeof(SFace), m_vFace.size(), hFile);  
	}  

	fwrite(&m_MeshMatrix, sizeof(m_MeshMatrix), 1, hFile);
}

void SMeshData::ReadFromFile( FILE* hFile )
{
	int iMeshNameSize = 0;
	fread(&iMeshNameSize, sizeof(int), 1, hFile);
	char tempBuf[_MAX_PATH] = {0};

	fread(tempBuf, iMeshNameSize, 1, hFile);
	m_MeshName = tempBuf;

	int iSubTextureNum = 0;
	fread(&iSubTextureNum, sizeof(int), 1, hFile);
	if ( iSubTextureNum > 0 )
		m_cMaterial.m_SubTextureVec.resize(iSubTextureNum);

	for (auto& rTex : m_cMaterial.m_SubTextureVec)
	{  
		int iTexNameSize = 0;
		fread(&iTexNameSize, sizeof(int), 1, hFile);

		fread(tempBuf, iTexNameSize, 1, hFile);
		rTex.m_sFileName = tempBuf;
	}

	int iVertexCount = 0;
	fread(&iVertexCount, sizeof(int), 1, hFile);  

	int iIndexCount = 0;
	fread(&iIndexCount, sizeof(int), 1, hFile);  

	if(iVertexCount > 0)  
	{  
		m_vVectex.resize(iVertexCount);
		fread(&m_vVectex.front(), sizeof(SVertex), iVertexCount, hFile);
	}

	if(iIndexCount > 0)  
	{  
		m_vFace.resize(iIndexCount);
		fread(&m_vFace.front(), sizeof(SFace), iIndexCount, hFile);  
	}

	fread(&m_MeshMatrix, sizeof(m_MeshMatrix), 1, hFile);
}

