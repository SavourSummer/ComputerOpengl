#pragma once
#include "Renderer.h"

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;//实际纹理的每像素位

public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0)const;//slot表示纹理插槽
	void Unbind();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};