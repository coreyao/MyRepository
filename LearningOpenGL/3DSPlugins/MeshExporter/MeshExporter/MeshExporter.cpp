//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "MeshExporter.h"
#include "../../../Source/FrameWork/DataTypes.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <stdmat.h>
#include <IGame/IGameType.h>
#include <iskin.h>
#include <modstack.h>

#if VERSION_3DSMAX >= 7 << 16
#include <CS/bipexp.h>
#else
#include <bipexp.h>
#endif

using namespace std;

#define MeshExporter_CLASS_ID	Class_ID(0x9e2e287e, 0xc6e1aa9e)

HWND g_hListBox = NULL;    
void AddStrToOutPutListBox(const char* szText)  
{  
	if( g_hListBox )  
	{  
		SendMessage(g_hListBox,LB_ADDSTRING,0,(LPARAM)szText);  
	}  
}

class MeshExporter : public SceneExport 
{
	public:
		static HWND hParams;
		
		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL			SupportsOptions(int ext, DWORD options);
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);
		int             ExportMesh(const char* szMeshName);
		void			ExportToFile( const char* szMeshName );
		void			DoClean();
		BOOL			SubTextureEnum(MtlBase* vMtl, vector<STextureData>& vTextureVec);
		BOOL			EnumBones(INode* node);
		BOOL			EnumGeomObjects(INode* node);
		void			ParseAllInfo();
		void			ParseGeomObject(INode* nodee);
		void			ParseBones(INode* pNode);
		void			ParseBoneAnimation();
		void			ParseMaterials();
		bool			IsBone(INode* node);
		bool			IsMesh(INode* node);
		SBoneData*		FindBoneDataByName(const char* pName);
		vector<ISkin*>	FindSkinModifier(INode* pNode);
		void			ConvertGMatrixToMat4(Mat4& outMat, const GMatrix& inputMatrix);

		//Constructor/Destructor
		MeshExporter();
		~MeshExporter();		

		Interface*			m_pInterface;   
		ExpInterface*		m_pExpInterface;
		BOOL				m_exportSelected;
		char				m_szExportPath[_MAX_PATH];

		vector<SMaterialData>	m_AllMaterialVec;
		vector<SMeshData>		m_MeshNodeVec;
		map<INode*, SBoneData>	m_allBoneData;
		vector<SBoneFrame>		m_allBoneFrames;
};

class MeshExporterClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new MeshExporter(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return MeshExporter_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("MeshExporter"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
}; 

ClassDesc2* GetMeshExporterDesc() 
{ 
	static MeshExporterClassDesc MeshExporterDesc;
	return &MeshExporterDesc; 
}

INT_PTR CALLBACK MeshExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static MeshExporter *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			{
				imp = (MeshExporter *)lParam;
				CenterWindow(hWnd,GetParent(hWnd));
				g_hListBox = ::GetDlgItem(hWnd, IDC_LIST1);

				std::string strPathName = imp->m_szExportPath;
				std::string strFileName;  
				std::string::size_type pos1 = strPathName.find_last_of('\\');  
				std::string strFileName_NoExt;  
				if (pos1 != std::string::npos)  
					strFileName = strPathName.substr(pos1 + 1);  
				else  
					strFileName = strPathName;  

				std::string::size_type pos2 = strFileName.find_last_of('.');  
				if (pos2 != std::string::npos)  
					strFileName_NoExt = strFileName.substr(0, pos2);  
				else  
					strFileName_NoExt = strFileName;

				HWND hNameEdit = ::GetDlgItem(hWnd, IDC_EDIT1);  
				SetWindowText(hNameEdit,strFileName_NoExt.c_str()); 
			}
			return TRUE;
		case WM_COMMAND:  
			{  
				switch(wParam)  
				{  
				case IDC_BUTTON1:  
					{  
						if(imp)  
						{  
							HWND hNameEdit = ::GetDlgItem(hWnd,IDC_EDIT1);  
							char szMeshName[64];  
							GetWindowText(hNameEdit,szMeshName,64);  
							imp->ExportMesh(szMeshName);  
						}  
					}  
					break;  
				case IDC_BUTTON2:  
					{  
						EndDialog(hWnd, 0);  
						return 0;  
					}  
					break;  
				}  
			}  
			break;  

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}

//--- MeshExporter -------------------------------------------------------
MeshExporter::MeshExporter()
{

}

MeshExporter::~MeshExporter() 
{

}

int MeshExporter::ExtCount()
{
	#pragma message(TODO("Returns the number of file name extensions supported by the plug-in."))
	return 1;
}

const TCHAR *MeshExporter::Ext(int n)
{		
	#pragma message(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	return _T("CSTM");
}

const TCHAR *MeshExporter::LongDesc()
{
	#pragma message(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\")"))
	return _T("Mesh Exporter Author : Yao Jinhua");
}
	
const TCHAR *MeshExporter::ShortDesc() 
{			
	#pragma message(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("Mesh Exporter");
}

const TCHAR *MeshExporter::AuthorName()
{			
	#pragma message(TODO("Return ASCII Author name"))
	return _T("Yao Jinhua");
}

const TCHAR *MeshExporter::CopyrightMessage() 
{	
	#pragma message(TODO("Return ASCII Copyright message"))
	return _T("Mesh Exporter Author : Yao Jinhua");
}

const TCHAR *MeshExporter::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *MeshExporter::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int MeshExporter::Version()
{				
	#pragma message(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void MeshExporter::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL MeshExporter::SupportsOptions(int ext, DWORD options)
{
	#pragma message(TODO("Decide which options to support.  Simply return true for each option supported by each Extension the exporter supports."))
	return TRUE;
}

int	MeshExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	#pragma message(TODO("Implement the actual file Export here and"))

	strcpy(m_szExportPath, name);
	m_pExpInterface = ei;  
	m_pInterface = i;  
	m_exportSelected = (options & SCENE_EXPORT_SELECTED);

	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				MeshExporterOptionsDlgProc, (LPARAM)this);

	#pragma message(TODO("return TRUE If the file is exported properly"))
	return FALSE;
}

int MeshExporter::ExportMesh(const char* szMeshName)  
{  
	ParseAllInfo();

	ExportToFile(szMeshName);
	DoClean();

	AddStrToOutPutListBox("Export Finished!");

	return 0;  
}  

BOOL MeshExporter::EnumBones(INode* node)   
{  
	if (!node) return FALSE;

	if ( IsBone(node) )
		ParseBones(node);
	
	for (int i = 0; i < node->NumberOfChildren(); ++i)
		EnumBones(node->GetChildNode(i));

	return TRUE;  
}  

BOOL MeshExporter::EnumGeomObjects( INode* node )
{
	if (!node || IsBone(node) ) return FALSE;

	if ( IsMesh(node) )
		ParseGeomObject(node);

	for (int i = 0; i < node->NumberOfChildren(); ++i)
		EnumGeomObjects(node->GetChildNode(i));

	return TRUE;  
}

BOOL MeshExporter::SubTextureEnum( MtlBase* vMtl, vector<STextureData>& vTextureVec )
{
	int tTextureNum = vMtl->NumSubTexmaps();  
	for (int j = 0; j < tTextureNum ; j++)  
	{  
		Texmap* tmap = vMtl->GetSubTexmap(j); 
		if (tmap)  
		{  
			if (tmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))  
			{  
				BitmapTex* bmt = (BitmapTex*)tmap;

				STextureData tParseTexture;
				std::string strMapName = bmt->GetMapName();  
				if (!strMapName.empty())
				{  
					std::string strFullName;  
					std::string::size_type pos = strMapName.find_last_of('\\');  
					if (pos != std::string::npos)  
						strFullName = strMapName.substr(pos+1);  
					else  
						strFullName = strMapName;
					tParseTexture.m_sFileName = strFullName;
				}  
				vTextureVec.push_back(tParseTexture);  
			}  
		}  
	}  

	return TRUE;  
}

void MeshExporter::ParseGeomObject(INode* pNode)
{  
	char  tText[200] = {0};
	ObjectState os = pNode->EvalWorldState(0);   
	Object *obj = os.obj;
	if ( obj )  
	{  
		if(obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))  
		{  
			TriObject* tri = (TriObject *) obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));  
			if (tri)
			{  
				sprintf(tText,"Export Object:<%s>.............", pNode->GetName());  
				AddStrToOutPutListBox(tText);

				SMeshData tMesh;
				tMesh.m_MeshName = pNode->GetName();
				Mtl* nodemtl = pNode->GetMtl();
				if (nodemtl)
				{  
					MtlBaseLib* scenemats = m_pInterface->GetSceneMtls();  
					int tCount = scenemats->Count();  
					for(int i = 0 ; i < tCount ; i++)  
					{  
						MtlBase* mtl = (*scenemats)[i];  
						if(strcmp(mtl->GetName(),nodemtl->GetName()) == 0)  
						{  
							tMesh.m_cMaterial = m_AllMaterialVec[i];  
							break;  
						}  
					}  

					sprintf(tText,"Relating Material:<%s>", nodemtl->GetName());  
					AddStrToOutPutListBox(tText);
				}

				Mesh* mesh = &tri->GetMesh();
				mesh->buildNormals();
				mesh->checkNormals(TRUE);

				sprintf(tText,"Mesh:<%s> VertexCount:<%d>FaceCount<%d>",pNode->GetName(),mesh->getNumVerts(),mesh->getNumFaces());  
				AddStrToOutPutListBox(tText);

				int tVertexNum = mesh->getNumVerts();   
				int tFaceNum = mesh->getNumFaces();  
				Matrix3 tTMAfterWSMM = pNode->GetNodeTM(0);
				GMatrix tGMeshTM(tTMAfterWSMM);  
				ConvertGMatrixToMat4(tMesh.m_MeshMatrix, tGMeshTM);

				for (int i = 0; i < tFaceNum; i++)
				{  
					int tDestTexIndex1 = mesh->faces[i].v[0];  
					int tDestTexIndex2 = mesh->faces[i].v[1];  
					int tDestTexIndex3 = mesh->faces[i].v[2];  

					SFace tFace;
					tFace.m_VertexIndex1 = tDestTexIndex1;
					tFace.m_VertexIndex2 = tDestTexIndex2;  
					tFace.m_VertexIndex3 = tDestTexIndex3;
					tMesh.m_vFace.push_back(tFace);
				}

				vector<SVertex> tVertexVec;  
				for (int i = 0; i < tVertexNum; i++)
				{  
					SVertex tVertex;  
					Point3 vert = mesh->verts[i];  
					tVertex.m_position.set(vert.x, vert.z, vert.y);  

					Point3 norm = mesh->getNormal(i);  
					tVertex.m_normal.set(norm.x, norm.z, norm.y);

					tVertex.m_color.r = 1.0f;  
					tVertex.m_color.g = 1.0f;  
					tVertex.m_color.b = 1.0f;  
					tVertex.m_color.a = 1.0f;

					tVertex.m_texCoord.set(0.0f, 0.0f);

					tVertexVec.push_back(tVertex);  
				}  

				if( mesh->numCVerts > 0)  
				{  
					for (int i = 0; i < tFaceNum; i++)  
					{  
						TVFace   tface = mesh->vcFace[i];  
						int tSrcColorIndex1 = tface.getTVert(0);  
						int tSrcColorIndex2 = tface.getTVert(1);  
						int tSrcColorIndex3 = tface.getTVert(2);  

						int tDestColorIndex1 = mesh->faces[i].v[0];  
						int tDestColorIndex2 = mesh->faces[i].v[1];  
						int tDestColorIndex3 = mesh->faces[i].v[2];  

						tVertexVec[tDestColorIndex1].m_color.r = mesh->vertCol[tSrcColorIndex1].x;  
						tVertexVec[tDestColorIndex1].m_color.g = mesh->vertCol[tSrcColorIndex1].y;  
						tVertexVec[tDestColorIndex1].m_color.b = mesh->vertCol[tSrcColorIndex1].z;  

						tVertexVec[tDestColorIndex2].m_color.r = mesh->vertCol[tSrcColorIndex2].x;  
						tVertexVec[tDestColorIndex2].m_color.g = mesh->vertCol[tSrcColorIndex2].y;  
						tVertexVec[tDestColorIndex2].m_color.b = mesh->vertCol[tSrcColorIndex2].z;  

						tVertexVec[tDestColorIndex3].m_color.r = mesh->vertCol[tSrcColorIndex3].x;  
						tVertexVec[tDestColorIndex3].m_color.g = mesh->vertCol[tSrcColorIndex3].y;  
						tVertexVec[tDestColorIndex3].m_color.b = mesh->vertCol[tSrcColorIndex3].z;
					}  
				}  

				if( mesh->numTVerts > 0)  
				{  
					for (int i = 0; i < tFaceNum; i++)  
					{  
						TVFace tface = mesh->tvFace[i];
						int tSrcTexIndex1 = tface.getTVert(0);  
						int tSrcTexIndex2 = tface.getTVert(1);  
						int tSrcTexIndex3 = tface.getTVert(2);  

						int tDestTexIndex1 = mesh->faces[i].v[0];  
						int tDestTexIndex2 = mesh->faces[i].v[1];  
						int tDestTexIndex3 = mesh->faces[i].v[2];  

						SVertex& tV1 = tVertexVec[tDestTexIndex1];  
						SVertex& tV2 = tVertexVec[tDestTexIndex2];  
						SVertex& tV3 = tVertexVec[tDestTexIndex3];  

						tV1.m_texCoord[0] = mesh->tVerts[tSrcTexIndex1].x;  
						tV1.m_texCoord[1] = 1.0 - mesh->tVerts[tSrcTexIndex1].y;  

						tV2.m_texCoord[0] = mesh->tVerts[tSrcTexIndex2].x;  
						tV2.m_texCoord[1] = 1.0 - mesh->tVerts[tSrcTexIndex2].y;  

						tV3.m_texCoord[0] = mesh->tVerts[tSrcTexIndex3].x;  
						tV3.m_texCoord[1] = 1.0 - mesh->tVerts[tSrcTexIndex3].y;
					}  
				}  
  
				vector<ISkin*> vSkinInfo = FindSkinModifier(pNode);
				if ( !vSkinInfo.empty() )
				{
					for ( auto& pSkin : vSkinInfo )
					{
						ISkinContextData* pSkinCtx = pSkin->GetContextInterface(pNode);
						for (int i = 0; i < tVertexNum/*pSkinCtx->GetNumPoints()*/; ++i)
						{
							int nBones = pSkinCtx->GetNumAssignedBones(i);	
							for (int j = 0; j < nBones; ++j)
							{
								INode* pBone = pSkin->GetBone(pSkinCtx->GetAssignedBone(i, j));
								char* bName = pBone->GetName();
								float weight = pSkinCtx->GetBoneWeight(i,j);
								SBoneData* pBoneData = FindBoneDataByName(bName);
								if ( pBoneData )
								{
									auto it = std::find(tMesh.m_skeleton.m_vSkinBone.begin(), tMesh.m_skeleton.m_vSkinBone.end(), pBoneData->m_iIndex);
									if ( it == tMesh.m_skeleton.m_vSkinBone.end())
									{
										tMesh.m_skeleton.m_vSkinBone.push_back(pBoneData->m_iIndex);
									}

									int iIndex = 0;
									for ( auto& rBoneDataIndex : tMesh.m_skeleton.m_vSkinBone )
									{
										if ( rBoneDataIndex == pBoneData->m_iIndex )
											break;

										++iIndex;
									}

									tVertexVec[i].m_boneIndex[j] = iIndex;
									tVertexVec[i].m_blendWeight[j] = weight;
								}
							}
						}
					}
				}
				
				for (auto& rBone : m_allBoneData)
					tMesh.m_skeleton.m_vBone.push_back(rBone.second);

				tMesh.m_skeleton.m_vFrame = m_allBoneFrames;
				tMesh.m_vVectex = tVertexVec;
				m_MeshNodeVec.push_back(tMesh);
			}  
		}  
	}
}  

void MeshExporter::ParseBones( INode* pNode )
{
	SBoneData newBone;
	newBone.m_iIndex = m_allBoneData.size() + 1;
	newBone.m_sName = pNode->GetName();
	GMatrix nodeTransform( pNode->GetNodeTM(0) );
	ConvertGMatrixToMat4(newBone.m_originalBindMat, nodeTransform);

	GMatrix inverseNodeTransform = nodeTransform.Inverse();
	ConvertGMatrixToMat4(newBone.m_inverseBindMat, inverseNodeTransform);

	INode* pParentNode = pNode->GetParentNode();
	if ( pParentNode && IsBone( pParentNode ) )
	{
		SBoneData* pParentBone = nullptr;
		for (auto& rBone : m_allBoneData)
		{
			if ( rBone.second.m_sName == pParentNode->GetName() )
			{
				pParentBone = &rBone.second;
				break;
			}
		}

		if ( pParentBone )
		{
			pParentBone->m_vChildIndex.push_back(newBone.m_iIndex);
			newBone.m_iParentIndex = pParentBone->m_iIndex;

			GMatrix parentTransform(pParentNode->GetNodeTM(0));
			GMatrix tempMat = nodeTransform * parentTransform.Inverse();
			ConvertGMatrixToMat4(newBone.m_originalBindMat, tempMat);
		}
	}

	m_allBoneData.insert(std::pair<INode*, SBoneData>(pNode, newBone));
}

bool MeshExporter::IsMesh( INode* node )
{
	if (NULL == node)
		return false;

	ObjectState os = node->EvalWorldState(0);
	if (NULL == os.obj)
		return false;

	if (os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID)
	{
		if (os.obj->ClassID() != Class_ID(TARGET_CLASS_ID, 0))
			return true;
	}

	return false;
}

bool MeshExporter::IsBone( INode* node )
{
	if (NULL == node)
	{
		return false;
	}

	ObjectState os = node->EvalWorldState(0);
	if (NULL == os.obj)
	{
		return false;
	}

	if (os.obj->SuperClassID() == HELPER_CLASS_ID)
	{
		return true;
	}

	if (os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID)
	{
		if (os.obj->ClassID() == BONE_OBJ_CLASSID)
		{
			return true;
		}
	}

	Control* ctl = node->GetTMController();
	if ((ctl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID)
		|| (ctl->ClassID() == BIPBODY_CONTROL_CLASS_ID))
	{
		return true;
	}

	return false;
}

void MeshExporter::ParseMaterials()
{
	MtlBaseLib* scenemats = m_pInterface->GetSceneMtls(); 
	if (scenemats)  
	{     
		char tText[200] = {0};  
		int tCount = scenemats->Count();  

		sprintf(tText, " Total Materials : %d ",tCount);  
		AddStrToOutPutListBox(tText);  

		if(tCount > 0)  
		{  
			m_AllMaterialVec.clear();
			for (int i = 0; i < tCount ; i++)  
			{   
				MtlBase * vMtl = (*scenemats)[i];  
				if (IsMtl(vMtl))  
				{         
					SMaterialData pParseMaterial;  
					pParseMaterial.m_MaterialName = vMtl->GetName();
					SubTextureEnum(vMtl, pParseMaterial.m_SubTextureVec);
					m_AllMaterialVec.push_back(pParseMaterial);
				}
			}  
		}  
	}  
}

void MeshExporter::ExportToFile( const char* szMeshName )
{
	int nMeshCount = m_MeshNodeVec.size();  
	for(int m = 0; m < nMeshCount; ++m)  
	{  
		SMeshData* pMesh = &m_MeshNodeVec[m];
		char szExportFileName[_MAX_PATH] = {0};
		if( 1 == nMeshCount )
		{  
			pMesh->m_MeshName = szMeshName;  
			strcpy(szExportFileName, m_szExportPath);
		}  
		else  
		{  
			_snprintf(szExportFileName, _MAX_PATH, "%s_%d", szMeshName, m);  
			pMesh->m_MeshName = szExportFileName;
			std::string strExportPath = m_szExportPath;
			std::string strEx;  
			std::string strName = strExportPath;  
			std::string::size_type pos = strExportPath.find_last_of(".");  
			if (pos != std::string::npos)  
			{  
				strEx = strExportPath.substr(pos+1);  
				strName = strExportPath.substr(0, pos);
				_snprintf( szExportFileName, _MAX_PATH, "%s_%d.%s", strName.c_str(), m, strEx.c_str());  
			}  
			else  
			{  
				_snprintf( szExportFileName, _MAX_PATH, "%s_%d", strName.c_str(), m);  
			}  
		}  

		FILE* hFile = fopen(szExportFileName, "wb");
		if ( hFile )
		{
			pMesh->WriteToFile(hFile);
			fclose(hFile);  
		}
	}
}

void MeshExporter::DoClean()
{
	m_AllMaterialVec.clear();
	m_MeshNodeVec.clear();  
}

void MeshExporter::ParseAllInfo()
{
	ParseMaterials();

	INode* pRootNode = m_pInterface->GetRootNode();
	EnumBones(pRootNode);
	ParseBoneAnimation();

	EnumGeomObjects(pRootNode);
}

vector<ISkin*> MeshExporter::FindSkinModifier( INode* pNode )
{
	Object* pObject = pNode->GetObjectRef(); 
	if(pObject == 0) 
		return vector<ISkin*>();

	vector<ISkin*> result;
	// 循环检测所有的DerivedObject 
	while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{ 
		IDerivedObject* pDerivedObject = static_cast<IDerivedObject*>(pObject); 
		for(int stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
		{ 
			Modifier * pModifier = pDerivedObject->GetModifier(stackId); 
			//检测ClassID是不是Skin修改器
			if(pModifier->ClassID() == SKIN_CLASSID)
			{ 
				result.push_back( (ISkin*)pModifier->GetInterface(I_SKIN) );
			}
		}

		pObject = pDerivedObject->GetObjRef();//下一个Derived Object 
	} 

	return result; 
}

SBoneData* MeshExporter::FindBoneDataByName( const char* pName )
{
	for (auto& pValue : m_allBoneData)
	{
		if ( pValue.second.m_sName == pName )
		{
			return &pValue.second;
		}
	}

	return nullptr;
}

void MeshExporter::ConvertGMatrixToMat4( Mat4& outMat, const GMatrix& inputMatrix )
{
	outMat.set(inputMatrix[0][0], inputMatrix[0][2], inputMatrix[0][1], inputMatrix[0][3]
			, inputMatrix[2][0], inputMatrix[2][2], inputMatrix[2][1], inputMatrix[2][3]
			, inputMatrix[1][0], inputMatrix[1][2], inputMatrix[1][1], inputMatrix[1][3]
			, inputMatrix[3][0], inputMatrix[3][2], inputMatrix[3][1], inputMatrix[3][3]
			);

	outMat.transpose();
}

void MeshExporter::ParseBoneAnimation()
{
	Interval ARange = m_pInterface->GetAnimRange();
	TimeValue tAniTime = ARange.End() - ARange.Start();
	TimeValue tTime = ARange.Start();
	int gpf = GetTicksPerFrame();
	int frameCount = tAniTime / gpf;
	for(int i = 0; i < frameCount; ++i)
	{
		if ( i > 0 && i < frameCount - 1 && i % 5 != 0)
			continue;

		SBoneFrame tempFrame;
		tempFrame.m_iIndex = i;
		for(auto& rBone : m_allBoneData)
		{
			SBoneKey bKey;
			GMatrix gm = rBone.first->GetNodeTM(i * gpf);
			INode* pParentNode = rBone.first->GetParentNode();
			if ( pParentNode && IsBone( pParentNode ) )
			{
				GMatrix nodeTransform( gm );
				GMatrix parentTransform(pParentNode->GetNodeTM(i * gpf));
				gm = nodeTransform * parentTransform.Inverse();
			}

			Point3 pos = gm.Translation();
			Quat dir = gm.Rotation();
			Point3 scale = gm.Scaling();

			bKey.m_translation.set(pos.x, pos.z, pos.y);

			bKey.m_rotation[0] = dir.w;
			bKey.m_rotation[1] = dir.x;
			bKey.m_rotation[2] = dir.z;
			bKey.m_rotation[3] = dir.y;
			bKey.m_scale.set(scale.x, scale.z, scale.y);
			
			tempFrame.m_fTime = i / 30.0f;
			tempFrame.m_vKey.push_back(bKey);
		}

		m_allBoneFrames.push_back(tempFrame);
	}
}
