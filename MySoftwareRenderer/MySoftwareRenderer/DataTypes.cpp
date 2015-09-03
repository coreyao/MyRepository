#include "DataTypes.h"

Mat4 STransform::GetRotationMat()
{
	return Mat4::CreateRotationMat(m_rotation.x, m_rotation.y, m_rotation.z);
}

void STransform::Reset()
{
	m_pos = Vec3(0, 0, 0);
	m_rotation = Vec3(0, 0, 0);
	m_scale = Vec3(1.0f, 1.0f, 1.0f);
}

Mat4 STransform::GetTransformMat()
{
	if (m_mat != Mat4::IDENTITY)
	{
		return m_mat;
	}

	return Mat4::CreateTranslationMat(m_pos.x, m_pos.y, m_pos.z)
		* GetRotationMat()
		* Mat4::CreateScaleMat(m_scale.x, m_scale.y, m_scale.z);
}

void STransform::SetMat(const Mat4& mat)
{
	m_mat = mat;
}

const Color4F Color4F::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const Color4F Color4F::GREEN(0.0f, 1.0f, 0.0f, 1.0f);

const Color3B Color3B::WHITE(255, 255, 255);
const Color3B Color3B::GREEN(0, 255, 0);

Color3B Color3B::operator+(const Color3B& rh)
{
	return Color3B(r + rh.r, g + rh.g, b + rh.b);
}

Color3B Color3B::operator-(const Color3B& rh)
{
	return Color3B(r - rh.r, g - rh.g, b - rh.b);
}

Color3B Color3B::operator*(float fScale)
{
	return Color3B(r * fScale, g * fScale, b * fScale);
}

Color3B Color3B::operator*(const Color3B& rh)
{
	return Color3B(r * rh.r, g * rh.g, b * rh.b);
}

Color3B Color3B::operator/(float fScale)
{
	return Color3B(r / fScale, g / fScale, b / fScale);
}

Color3B Color3B::operator*=(float fScale)
{
	*this = *this * fScale;
	return *this;
}

Color3B Color3B::operator*=(const Color3B& rh)
{
	*this = Color3B(r * rh.r, g * rh.g, b * rh.b);
	return *this;
}

Color4F Color4F::operator*(const Color4F& rh)
{
	return Color4F(r * rh.r, g * rh.g, b * rh.b, a * rh.a);
}

SVertex::SVertex(const Vec3& pos, const Color4F& color, const Vec2& uv)
: m_pos(pos)
, m_color(color)
, m_UV(uv)
{

}

void CMaterial::SetBaseColorTexture(const std::string& sFileName)
{

}
