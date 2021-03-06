#include "DataTypes.h"

void SSubMeshData::WriteToFile(FILE* hFile)
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
		fwrite(&m_vVectex.front(), sizeof(SSkinMeshVertex), m_vVectex.size(), hFile);  
	}  

	if(iIndexCount > 0 )  
	{  
		fwrite(&m_vFace.front(), sizeof(SFace), m_vFace.size(), hFile);  
	}

	fwrite(&m_MeshMatrix, sizeof(m_MeshMatrix), 1, hFile);
}

void SSubMeshData::ReadFromFile( FILE* hFile )
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
		fread(&m_vVectex.front(), sizeof(SSkinMeshVertex), iVertexCount, hFile);
	}

	if(iIndexCount > 0)  
	{  
		m_vFace.resize(iIndexCount);
		fread(&m_vFace.front(), sizeof(SFace), iIndexCount, hFile);  
	}

	fread(&m_MeshMatrix, sizeof(m_MeshMatrix), 1, hFile);
}

void SBoneData::WriteToFile( FILE* hFile )
{
	fwrite(&m_iParentIndex, sizeof(int), 1, hFile);
	fwrite(&m_iIndex, sizeof(int), 1, hFile);

	int iNameSize = m_sName.size();
	fwrite(&iNameSize, sizeof(int), 1, hFile);
	fwrite(m_sName.c_str(), iNameSize, 1, hFile);

	fwrite(&m_originalBindMat, sizeof(m_originalBindMat), 1, hFile);
	fwrite(&m_inverseBindMat, sizeof(m_inverseBindMat), 1, hFile);

	int iChildNum = m_vChildIndex.size();
	fwrite(&iChildNum, sizeof(int), 1, hFile);
	if ( iChildNum > 0 )
	{
		fwrite(&m_vChildIndex.front(), sizeof(int), m_vChildIndex.size(), hFile);
	}
}

void SBoneData::ReadFromFile( FILE* hFile )
{
	char tempBuf[_MAX_PATH] = {0};

	fread(&m_iParentIndex, sizeof(int), 1, hFile);
	fread(&m_iIndex, sizeof(int), 1, hFile);

	int iNameSize = 0;
	fread(&iNameSize, sizeof(int), 1, hFile);
	fread(tempBuf, iNameSize, 1, hFile);
	m_sName = tempBuf;

	fread(&m_originalBindMat, sizeof(m_originalBindMat), 1, hFile);
	fread(&m_inverseBindMat, sizeof(m_inverseBindMat), 1, hFile);

	int iChildNum = m_vChildIndex.size();
	fread(&iChildNum, sizeof(int), 1, hFile);

	if ( iChildNum > 0 )
	{
		m_vChildIndex.resize(iChildNum);
		fread(&m_vChildIndex.front(), sizeof(int), iChildNum, hFile);
	}
}

void SBoneFrame::WriteToFile( FILE* hFile )
{
	fwrite(&m_iIndex, sizeof(int), 1, hFile);
	fwrite(&m_fTime, sizeof(float), 1, hFile);

	int iKeyCount = m_vKey.size();
	fwrite(&iKeyCount, sizeof(int), 1, hFile);
	if ( iKeyCount > 0 )
		fwrite(&m_vKey.front(), sizeof(SBoneKey), iKeyCount, hFile);
}

void SBoneFrame::ReadFromFile( FILE* hFile )
{
	fread(&m_iIndex, sizeof(int), 1, hFile);
	fread(&m_fTime, sizeof(float), 1, hFile);

	int iKeyCount = 0;
	fread(&iKeyCount, sizeof(int), 1, hFile);
	m_vKey.resize(iKeyCount);
	if ( iKeyCount > 0 )
		fread(&m_vKey.front(), sizeof(SBoneKey), iKeyCount, hFile);
}

void SMeshData::WriteToFile( FILE* hFile )
{
	int iSubMeshNum = m_vSubMesh.size();
	fwrite(&iSubMeshNum, sizeof(int), 1, hFile);

	for (int i = 0; i < iSubMeshNum; ++i)
	{
		m_vSubMesh[i].WriteToFile(hFile);
	}

	int iBoneNum = m_skeleton.m_vBone.size();
	fwrite(&iBoneNum, sizeof(int), 1, hFile);
	for (int i = 0; i < iBoneNum; ++i)
	{
		m_skeleton.m_vBone[i].WriteToFile(hFile);
	}

	int iSkinBoneNum = m_skeleton.m_vSkinBone.size();
	fwrite(&iSkinBoneNum, sizeof(int), 1, hFile);
	for (auto& rBoneIndex : m_skeleton.m_vSkinBone)
	{
		fwrite(&rBoneIndex, sizeof(int), 1, hFile);  
	}

	int iFrameNum = m_skeleton.m_vFrame.size();
	fwrite(&iFrameNum, sizeof(int), 1, hFile);
	for (int i = 0; i < iFrameNum; ++i)
	{
		m_skeleton.m_vFrame[i].WriteToFile(hFile);
	}
}

void SMeshData::ReadFromFile( FILE* hFile )
{
	int iSubMeshNum = 0;
	fread(&iSubMeshNum, sizeof(int), 1, hFile);
	m_vSubMesh.resize(iSubMeshNum);

	for (int i = 0; i < iSubMeshNum; ++i)
	{
		m_vSubMesh[i].ReadFromFile(hFile);
	}

	int iBoneNum = 0;
	fread(&iBoneNum, sizeof(int), 1, hFile);
	if ( iBoneNum > 0 )
	{
		m_skeleton.m_vBone.resize(iBoneNum);
		for (int i = 0; i < iBoneNum; ++i)
		{
			m_skeleton.m_vBone[i].ReadFromFile(hFile);
		}
	}

	int iSkinBoneNum = 0;
	fread(&iSkinBoneNum, sizeof(int), 1, hFile);
	for ( int i = 0; i < iSkinBoneNum; ++i)
	{
		int iBoneIndex = 0;
		fread(&iBoneIndex, sizeof(int), 1, hFile);  
		m_skeleton.m_vSkinBone.push_back(iBoneIndex);
	}

	int iFrameNum = 0;
	fread(&iFrameNum, sizeof(int), 1, hFile);
	if ( iFrameNum > 0 )
	{
		m_skeleton.m_vFrame.resize(iFrameNum);
		for (int i = 0; i < iFrameNum; ++i)
		{
			m_skeleton.m_vFrame[i].ReadFromFile(hFile);
		}
	}
}

Mat4 STransform::GetRotationMat()
{
	if (m_bUseQuaternion)
	{
		return Mat4::CreateRotationMat(m_quat);
	}
	else
	{
		return Mat4::CreateRotationMat(m_rotation.x, m_rotation.y, m_rotation.z);
	}
}

void STransform::Reset()
{
	m_pos = Vec3(0, 0, 0);
	m_rotation = Vec3(0, 0, 0);
	m_scale = Vec3(1.0f, 1.0f, 1.0f);
	m_bTransformDirty = true;
}

Mat4 STransform::GetTransformMat()
{
	if (m_bTransformDirty)
	{
		Mat4 scaleMat = Mat4::CreateScaleMat(m_scale.x, m_scale.y, m_scale.z);
		Mat4 rotationMat = GetRotationMat();
		Mat4 translationMat = Mat4::CreateTranslationMat(m_pos.x, m_pos.y, m_pos.z);

		m_mat = translationMat * rotationMat * scaleMat;

		m_bTransformDirty = false;
	}

	return m_mat;
}

void STransform::SetPosition(const Vec3& pos)
{
	if ( m_pos != pos )
	{
		m_pos = pos;
		m_bTransformDirty = true;
	}
}

void STransform::SetScale(const Vec3& scale)
{
	m_scale = scale;
	m_bTransformDirty = true;
}

void STransform::SetRotation(const Vec3& rot)
{
	m_rotation = rot;
	m_bTransformDirty = true;
}

void STransform::SetRotation(const Quaternion& rot)
{
	m_quat = rot;
	m_bTransformDirty = true;
}

bool STransform::IsTransformDirty()
{
	return m_bTransformDirty;
}

const Vec3& STransform::GetPosition() const
{
	return m_pos;
}

const Vec3& STransform::GetRotation() const
{
	return m_rotation;
}

const Vec3& STransform::GetScale() const
{
	return m_scale;
}

void STransform::SetUseQuaternion(bool bVal)
{
	m_bUseQuaternion = bVal;
}

void STransform::SetPositionX(float val)
{
	m_pos.x = val;
}

void STransform::SetPositionY(float val)
{
	m_pos.y = val;
}

void STransform::SetPositionZ(float val)
{
	m_pos.z = val;
}

const Color3B Color3B::WHITE (255, 255, 255);
const Color3B Color3B::GREEN (0, 255, 0);

Color3B Color3B::operator+( const Color3B& rh )
{
	return Color3B( r + rh.r, g + rh.g, b + rh.b );
}

Color3B Color3B::operator-( const Color3B& rh )
{
	return Color3B( r - rh.r, g - rh.g, b - rh.b );
}

Color3B Color3B::operator*( float fScale )
{
	return Color3B( r * fScale, g * fScale, b * fScale);
}

Color3B Color3B::operator*( const Color3B& rh )
{
	return Color3B( r * rh.r, g * rh.g, b * rh.b );
}

Color3B Color3B::operator/( float fScale )
{
	return Color3B( r / fScale, g / fScale, b / fScale);
}

Color3B Color3B::operator*=( float fScale )
{
	*this = *this * fScale;
	return *this;
}

Color3B Color3B::operator*=( const Color3B& rh )
{
	*this = Color3B( r * rh.r, g * rh.g, b * rh.b);
	return *this;
}

SCommonVertex::SCommonVertex(const Vec3& pos, const Color4F& color, const Vec2& uv)
	: m_pos(pos)
	, m_color(color)
	, m_UV(uv)
{

}

Color4F Color4F::operator*(const Color4F& rh) const
{
	return Color4F(r * rh.r, g * rh.g, b * rh.b, a * rh.a);
}

void Color4F::operator+=(const Color4F& rh)
{
	this->a += rh.a;
	this->r += rh.r;
	this->g += rh.g;
	this->b += rh.b;
}

Color4F Color4F::operator-(const Color4F& rh) const
{
	return Color4F(r - rh.r, g - rh.g, b - rh.b, a - rh.a);
}

Color4F Color4F::operator-(float fScalar) const
{
	return Color4F(r - fScalar, g - fScalar, b - fScalar, a - fScalar);
}

Color4F Color4F::operator/(float fScalar) const
{
	return Color4F(r / fScalar, g / fScalar, b / fScalar, a / fScalar);
}

Color4F Color4F::operator*(float fScalar) const
{
	return Color4F(r * fScalar, g * fScalar, b * fScalar, a * fScalar);
}

void Color4F::Set(float _r, float _g, float _b, float _a)
{
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

Color4F Color4F::operator+(const Color4F& rh) const
{
	return Color4F(this->r + rh.r, this->g + rh.g, this->b + rh.b, this->a + rh.a);
}

void Color4F::operator*=(float fScalar)
{
	this->a *= fScalar;
	this->r *= fScalar;
	this->g *= fScalar;
	this->b *= fScalar;
}

const Color4F Color4F::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const Color4F Color4F::GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const Color4F Color4F::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
