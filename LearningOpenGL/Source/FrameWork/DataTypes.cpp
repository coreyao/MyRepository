#include "DataTypes.h"

void SMeshData::WriteToFile(FILE* hFile)
{
	int iMeshNameSize = m_MeshName.size();
	fwrite(&iMeshNameSize, sizeof(int), 1, hFile);
	fwrite(m_MeshName.c_str(), iMeshNameSize, 1, hFile);

	int nSubNum = m_SubMeshVec.size();
	fwrite(&nSubNum,sizeof(int), 1, hFile);

	for(int s = 0 ; s < nSubNum ; s++)
	{  
		SSubMeshData* pSubMesh = &m_SubMeshVec[s];

		int iSubMeshNameSize = pSubMesh->m_SubMeshName.size();
		fwrite(&iSubMeshNameSize, sizeof(int), 1, hFile);
		fwrite(pSubMesh->m_SubMeshName.c_str(), iSubMeshNameSize, 1, hFile);

		int iSubTextureNum = pSubMesh->m_cMaterial.m_SubTextureVec.size();
		fwrite(&iSubTextureNum, sizeof(int), 1, hFile);
		for (auto& rTex : pSubMesh->m_cMaterial.m_SubTextureVec)
		{  
			int iTexNameSize = rTex.m_sFileName.size();
			fwrite(&iTexNameSize, sizeof(int), 1, hFile);
			fwrite(rTex.m_sFileName.c_str(), iTexNameSize, 1, hFile);
		}

		int iVertexCount = pSubMesh->m_vVectex.size();
		fwrite(&iVertexCount, sizeof(int), 1, hFile);  

		int iIndexCount = pSubMesh->m_vFace.size();
		fwrite(&iIndexCount, sizeof(int), 1, hFile);  

		if(iVertexCount > 0)  
		{  
			fwrite(&pSubMesh->m_vVectex.front(), sizeof(SVertex), pSubMesh->m_vVectex.size(), hFile);  
		}  

		if(iIndexCount > 0 )  
		{  
			fwrite(&pSubMesh->m_vFace.front(), sizeof(SFace), pSubMesh->m_vFace.size(), hFile);  
		}  

		fwrite(&pSubMesh->m_SubMeshMatrix, sizeof(pSubMesh->m_SubMeshMatrix), 1, hFile);
	}  
}

void SMeshData::ReadFromFile( FILE* hFile )
{
	int iMeshNameSize = 0;
	fread(&iMeshNameSize, sizeof(int), 1, hFile);
	char tempBuf[_MAX_PATH] = {0};

	fread(tempBuf, iMeshNameSize, 1, hFile);
	m_MeshName = tempBuf;

	int nSubNum = m_SubMeshVec.size();
	fread(&nSubNum, sizeof(int), 1, hFile);
	if ( nSubNum > 0 )
	{
		m_SubMeshVec.resize(nSubNum);
		for(int s = 0 ; s < nSubNum ; s++)
		{  
			SSubMeshData* pSubMesh = &m_SubMeshVec[s];

			int iSubMeshNameSize = 0;
			fread(&iSubMeshNameSize, sizeof(int), 1, hFile);
			if ( iSubMeshNameSize <= 0 )
				continue;

			fread(tempBuf, iSubMeshNameSize, 1, hFile);
			pSubMesh->m_SubMeshName = tempBuf;

			int iSubTextureNum = 0;
			fread(&iSubTextureNum, sizeof(int), 1, hFile);
			if ( iSubTextureNum > 0 )
				pSubMesh->m_cMaterial.m_SubTextureVec.resize(iSubTextureNum);

			for (auto& rTex : pSubMesh->m_cMaterial.m_SubTextureVec)
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
				pSubMesh->m_vVectex.resize(iVertexCount);
				fread(&pSubMesh->m_vVectex.front(), sizeof(SVertex), iVertexCount, hFile);
			}

			if(iIndexCount > 0)  
			{  
				pSubMesh->m_vFace.resize(iIndexCount);
				fread(&pSubMesh->m_vFace.front(), sizeof(SFace), iIndexCount, hFile);  
			}

			fread(&pSubMesh->m_SubMeshMatrix, sizeof(pSubMesh->m_SubMeshMatrix), 1, hFile);
		}  
	}
}

Vec4* CSkeleton::GetMatrixPalette()
{
	return nullptr;
}
