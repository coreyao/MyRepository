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
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdmat.h>
#include <IGame/IGameType.h>

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

struct Vec2
{
	Vec2() : x(0.0f), y(0.0f)
	{
	}

	float x;
	float y;
};

struct Vec3
{
	Vec3() : x(0.0f), y(0.0f), z(0.0f)
	{
	}

	float x;
	float y;
	float z;
};

struct Color4F
{
	Color4F() : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
	{
	}

	float r;
	float g;
	float b;
	float a;
};

struct SFace
{
	SFace()
		: m_VertexIndex1(0)
		, m_VertexIndex2(0)
		, m_VertexIndex3(0)
	{
	}

	int m_VertexIndex1;
	int m_VertexIndex2;
	int m_VertexIndex3;
};

struct STexture
{
	STexture()
		: m_Index(0)
	{
	}

	int m_Index;
	string m_sFileName;
};

struct SMaterial 
{
	SMaterial()
		: m_MaterialID(0)
	{
	}

	int m_MaterialID;
	std::string m_MaterialName;
	vector<STexture> m_SubTextureVec;
};

struct SSubMesh
{
	SSubMesh()
		: m_MaterialID(-1)
	{
		memset(m_SubMeshMatrix, 0, sizeof(m_SubMeshMatrix));
	}

	int m_MaterialID;
	string m_SubMeshName;
	float  m_SubMeshMatrix[16];
	vector<SFace> m_FaceVec;
};

struct SMesh
{
	string m_MeshName;
	vector<SSubMesh> m_SubMeshVec;
};

struct SVertex
{
	Vec3 m_position;
	Vec3 m_normal;
	Vec2 m_texCoord;
	Color4F m_color;
};

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
		BOOL			SubTextureEnum(MtlBase* vMtl, vector<STexture>& vTextureVec, int& vMaterialSize);
		BOOL			NodeEnum(INode* node, SMesh* pMeshNode);
		BOOL			NodeEnum_Child(INode* node, SMesh*  pMeshNode);
		void			ParseGeomObject(INode* node, SMesh* pMeshNode);

		//Constructor/Destructor
		MeshExporter();
		~MeshExporter();		

		Interface*			m_pInterface;   
		ExpInterface*		m_pExpInterface;
		BOOL				m_exportSelected;
		char				m_szExportPath[_MAX_PATH];

		vector<SMaterial*> m_AllMaterialVec;
		int					m_AllMaterialSize;

		vector<SMesh>  m_MeshNodeVec;
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
	return _T("");
}

const TCHAR *MeshExporter::LongDesc()
{
	#pragma message(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\")"))
	return _T("");
}
	
const TCHAR *MeshExporter::ShortDesc() 
{			
	#pragma message(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("");
}

const TCHAR *MeshExporter::AuthorName()
{			
	#pragma message(TODO("Return ASCII Author name"))
	return _T("");
}

const TCHAR *MeshExporter::CopyrightMessage() 
{	
	#pragma message(TODO("Return ASCII Copyright message"))
	return _T("");
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

	strcpy(m_szExportPath,name);  
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
			m_AllMaterialSize = 0;  
			for (int i = 0; i < tCount ; i++)  
			{   
				MtlBase * vMtl = (*scenemats)[i];  
				if (IsMtl(vMtl))  
				{         
					SMaterial* pParseMaterial = new SMaterial;  
					pParseMaterial->m_MaterialID = m_AllMaterialSize++;  
					pParseMaterial->m_MaterialName = vMtl->GetName();  
					SubTextureEnum(vMtl, pParseMaterial->m_SubTextureVec, m_AllMaterialSize);
					m_AllMaterialVec.push_back(pParseMaterial);
				}
			}  
		}  
	}  

	int numChildren = m_pInterface->GetRootNode()->NumberOfChildren();  
	if(numChildren > 0)  
	{  
		for (int idx = 0; idx < numChildren; idx++)  
		{  
			NodeEnum(m_pInterface->GetRootNode()->GetChildNode(idx), NULL);
		}  
	}  

	int nMeshCount = m_MeshNodeVec.size();  
	for(int m = 0; m < nMeshCount; ++m)  
	{  
		char szExportFileName[_MAX_PATH] = {0};  
		if( 1 == nMeshCount )
		{  
			m_MeshNodeVec[m].m_MeshName = szMeshName;  
			strcpy(szExportFileName, m_szExportPath);
		}  
		else  
		{  
			ostringstream oss;
			oss << szMeshName << "_" << m;
			
			m_MeshNodeVec[m].m_MeshName = oss.str();  
			std::string strExportPath = m_szExportPath;  

			// 得到扩展名  
			std::string strEx   = "";  
			std::string strName = strExportPath;  
			std::string::size_type pos = strExportPath.find_last_of(".");  
			if (pos != std::string::npos)  
			{  
				strEx = strExportPath.substr(pos+1);  
				strName = strExportPath.substr(0, pos);  
				_snprintf(  szExportFileName, _MAX_PATH, "%s_%d.%s", strName.c_str(),m,strEx);  
			}  
			else  
			{  
				_snprintf(  szExportFileName, _MAX_PATH, "%s_%d", strName.c_str(),m);  
			}  

		}  
		//进行二进制文件的写入。  
		FILE*   hFile = fopen(m_szExportPath,"wb");  
		fwrite(m_MeshNodeVec[m].m_MeshName,sizeof(m_MeshNodeVec[m].m_MeshName),1,hFile);  
		int nSubNum = m_MeshNodeVec[m].m_SubMeshVec.size();  
		fwrite(&nSubNum,sizeof(int),1,hFile);  

		for( int s = 0 ; s < nSubNum ; s++)  
		{  
			SSubMeshHeader  tSubMeshHeader;  
			strcpy(tSubMeshHeader.m_SubMeshName,m_MeshNodeVec[m].m_SubMeshVec[s].m_SubMeshName);  
			int nMaterialID = m_MeshNodeVec[m].m_SubMeshVec[s].m_MaterialID ;  
			SParseMaterial* tpParseMaterial = GetMaterial(nMaterialID);  
			if(tpParseMaterial && false == tpParseMaterial->m_SubTextureVec.empty())  
			{  
				strcpy(tSubMeshHeader.m_Texture,tpParseMaterial->m_SubTextureVec[0].m_FileName);  
			}  
			else  
			{  
				tSubMeshHeader.m_Texture[0]='\0';  
			}  
			tSubMeshHeader.m_VertexCount = m_MeshNodeVec[m].m_SubMeshVec[s].m_VertexVec.size();  
			tSubMeshHeader.m_IndexCount = m_MeshNodeVec[m].m_SubMeshVec[s].m_FaceVec.size() * 3;      
			tSubMeshHeader.m_PrimitiveType = PT_TRIANGLES ;  
			tSubMeshHeader.m_IndexFormat = INDEX16 ;  
			fwrite(&tSubMeshHeader,sizeof(SSubMeshHeader),1,hFile);  
			if(tSubMeshHeader.m_VertexCount > 0 )  
			{  
				fwrite(&m_MeshNodeVec[m].m_SubMeshVec[s].m_VertexVec.front(),sizeof(SVertex),tSubMeshHeader.m_VertexCount,hFile);  
			}  
			if(tSubMeshHeader.m_IndexCount > 0 )  
			{  
				fwrite(&m_MeshNodeVec[m].m_SubMeshVec[s].m_FaceVec.front(),sizeof(SFace),m_MeshNodeVec[m].m_SubMeshVec[s].m_FaceVec.size(),hFile);  
			}  
			fwrite(&m_MeshNodeVec[m].m_SubMeshVec[s].m_SubMeshMatrix,sizeof(SSubMeshMatrix),1,hFile);  
		}  
		fclose(hFile);  
	}  


	//释放材质  
	vector<SParseMaterial*>::iterator Iter;  
	for(Iter = m_AllMaterialVec.begin(); Iter != m_AllMaterialVec.end(); Iter++)  
	{  
		delete (*Iter);  
	}  
	m_AllMaterialVec.clear();  
	//释放模型  
	m_MeshNodeVec.clear();  
	AddStrToOutPutListBox("导出完毕!");

	return 0;  
}  

BOOL MeshExporter::NodeEnum(INode* node,SMesh* pMeshNode)   
{  
	if (!node)  
		return FALSE;  

	SMesh       tMeshNode;    
	TimeValue       tTime = 0;  
	ObjectState os = node->EvalWorldState(tTime);   

	if (os.obj)
	{  
		DWORD SuperclassID = os.obj->SuperClassID();  
		switch(SuperclassID)  
		{  
		case SHAPE_CLASS_ID:  
		case GEOMOBJECT_CLASS_ID:   
			ParseGeomObject(node, &tMeshNode); 
			break;  
		default:  
			break;  
		}  
	}  

	for (int c = 0; c < node->NumberOfChildren(); c++)  
	{  
		if (!NodeEnum_Child(node->GetChildNode(c), &tMeshNode))  
		{  
			break;  
		}  
	}  

	if(tMeshNode.m_SubMeshVec.size() > 0)  
	{  
		m_MeshNodeVec.push_back(tMeshNode);  
	}  
	return TRUE;  
}  

BOOL MeshExporter::NodeEnum_Child(INode* node, SMesh* pMeshNode)   
{  
	if (!node)  
		return FALSE;  

	TimeValue       tTime = 0;  
	ObjectState os = node->EvalWorldState(tTime);   

	if (os.obj)  
	{  
		char tText[200];  
		sprintf(tText,"Export<%s>----------------------<%d : %d>",node->GetName(),os.obj->SuperClassID(),os.obj->ClassID());  
		AddStrToOutPutListBox(tText);

		DWORD   SuperclassID = os.obj->SuperClassID();  
		switch(SuperclassID)  
		{  
		case SHAPE_CLASS_ID:  
		case GEOMOBJECT_CLASS_ID:   
			ParseGeomObject(node, pMeshNode);
			break;  
		default:  
			break;  
		}  
	}  

	for (int c = 0; c < node->NumberOfChildren(); c++)  
	{  
		if (!NodeEnum_Child(node->GetChildNode(c), pMeshNode))  
		{  
			break;  
		}  
	}  

	return TRUE;  
}  

BOOL MeshExporter::SubTextureEnum( MtlBase* vMtl, vector<STexture>& vTextureVec, int& vMaterialSize )
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

				STexture tParseTexture;
				tParseTexture.m_Index = j;  
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

class NullView : public View
{
public:
	Point2 ViewToScreen(Point3 p)
	{ 
		return Point2(p.x,p.y); 
	}

	NullView()
	{
		worldToView.IdentityMatrix();
		screenW=640.0f; screenH = 480.0f;
	}
};

void MeshExporter::ParseGeomObject(INode* node, SMesh* pMeshNode)
{  
	char            tText[200] = {0};
	TimeValue       tTime = 0;  
	ObjectState os = node->EvalWorldState(tTime);   
	if (!os.obj)  
		return;  

	if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))  
		return;  

	sprintf(tText,"Export Object:<%s>.............", node->GetName());  
	AddStrToOutPutListBox(tText);  

	SSubMesh tSubMesh;
	tSubMesh.m_SubMeshName = node->GetName();  

	Mtl* nodemtl = node->GetMtl();
	if (nodemtl)  
	{  
		MtlBaseLib* scenemats = m_pInterface->GetSceneMtls();  
		int tCount = scenemats->Count();  
		for(int i = 0 ; i < tCount ; i++)  
		{  
			MtlBase* mtl = (*scenemats)[i];  
			if(strcmp(mtl->GetName(),nodemtl->GetName()) == 0)  
			{  
				tSubMesh.m_MaterialID = i;  
				break;  
			}  
		}  
		sprintf(tText,"Relating Material:<%s>", nodemtl->GetName());  
		AddStrToOutPutListBox(tText);
	}  

	bool delMesh = false;  
	Object *obj = os.obj;  
	if ( obj )  
	{  
		if(obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))  
		{  
			TriObject* tri = (TriObject *) obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));  
			if (obj != tri)
				delMesh = true;   

			if (tri)  
			{  
				NullView maxView;
				BOOL bDelete = TRUE;  
				Mesh * mesh = tri->GetRenderMesh(tTime, node, maxView, bDelete);  
				mesh->buildNormals();  
				mesh->checkNormals(TRUE);  

				sprintf(tText,"Mesh:<%s> VertexCount:<%d>FaceCount<%d>",node->GetName(),mesh->getNumVerts(),mesh->getNumFaces());  
				AddStrToOutPutListBox(tText);  

				int tVertexNum = mesh->getNumVerts();   
				int tFaceNum   = mesh->getNumFaces();  

				Matrix3 tTMAfterWSMM = node->GetNodeTM(tTime);  
				GMatrix tGMeshTM(tTMAfterWSMM);  
				for(int m = 0 ; m < 4 ; m++)  
				{  
					for(int n = 0 ; n < 4 ; n++)  
					{  
						tSubMesh.m_SubMeshMatrix[m*4+n] = tGMeshTM[m][n]; 
					}  
				}  

				for (int i = 0; i < tFaceNum; i++)  
				{  
					int     tDestTexIndex1 = mesh->faces[i].v[0];  
					int     tDestTexIndex2 = mesh->faces[i].v[1];  
					int     tDestTexIndex3 = mesh->faces[i].v[2];  

					SFace tFace;
					tFace.m_VertexIndex1 = tDestTexIndex1;
					tFace.m_VertexIndex2 = tDestTexIndex2;  
					tFace.m_VertexIndex3 = tDestTexIndex3;
					tSubMesh.m_FaceVec.push_back(tFace); 
				}  

				vector<SVertex> tVertexVec;  
				for (int i = 0; i < tVertexNum; i++)
				{  
					SVertex tVertex;  
					Point3 vert = mesh->verts[i];  
					tVertex.m_position.x = vert.x;  
					tVertex.m_position.y = vert.z;  
					tVertex.m_position.z = vert.y;  

					Point3 norm = mesh->getNormal(i);  
					tVertex.m_normal.x = norm.x;  
					tVertex.m_normal.y = norm.z;  
					tVertex.m_normal.z = norm.y;  

					tVertex.m_color.r = 1.0f;  
					tVertex.m_color.g = 1.0f;  
					tVertex.m_color.b = 1.0f;  
					tVertex.m_color.a = 1.0f;

					tVertex.m_texCoord.x = 0.0f;
					tVertex.m_texCoord.y = 0.0f;  

					tVertexVec.push_back(tVertex);  
				}  

				if( mesh->numCVerts > 0)  
				{  
					for (int i = 0; i < tFaceNum; i++)  
					{  
						TVFace   tface = mesh->vcFace[i];  
						int     tSrcColorIndex1 = tface.getTVert(0);  
						int     tSrcColorIndex2 = tface.getTVert(1);  
						int     tSrcColorIndex3 = tface.getTVert(2);  

						int     tDestColorIndex1 = mesh->faces[i].v[0];  
						int     tDestColorIndex2 = mesh->faces[i].v[1];  
						int     tDestColorIndex3 = mesh->faces[i].v[2];  

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
						int     tSrcTexIndex1 = tface.getTVert(0);  
						int     tSrcTexIndex2 = tface.getTVert(1);  
						int     tSrcTexIndex3 = tface.getTVert(2);  

						int     tDestTexIndex1 = mesh->faces[i].v[0];  
						int     tDestTexIndex2 = mesh->faces[i].v[1];  
						int     tDestTexIndex3 = mesh->faces[i].v[2];  

						SVertex& tV1 = tVertexVec[tDestTexIndex1];  
						SVertex& tV2 = tVertexVec[tDestTexIndex2];  
						SVertex& tV3 = tVertexVec[tDestTexIndex3];  

						tV1.m_texCoord.x = mesh->tVerts[tSrcTexIndex1].x;  
						tV1.m_texCoord.y = 1.0 - mesh->tVerts[tSrcTexIndex1].y;  

						tV2.m_texCoord.x = mesh->tVerts[tSrcTexIndex2].x;  
						tV2.m_texCoord.y = 1.0 - mesh->tVerts[tSrcTexIndex2].y;  

						tV3.m_texCoord.x = mesh->tVerts[tSrcTexIndex3].x;  
						tV3.m_texCoord.y = 1.0 - mesh->tVerts[tSrcTexIndex3].y;  
					}  
				}  
  
				if (delMesh)
				{  
					delete tri;  
				}  
			}  
		}  
	}  

	pMeshNode->m_SubMeshVec.push_back(tSubMesh);  
}  

