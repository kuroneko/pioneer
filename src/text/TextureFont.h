// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _TEXT_TEXTUREFONT_H
#define _TEXT_TEXTUREFONT_H

#include "libs.h"
#include "FontDescriptor.h"
#include "RefCounted.h"
#include "graphics/Texture.h"
#include "graphics/Material.h"
#include "graphics/VertexArray.h"
#include "graphics/RenderState.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

namespace FileSystem { class FileData; }

namespace Text {

class TextureFont : public RefCounted {

public:
	TextureFont(const FontDescriptor &descriptor, Graphics::Renderer *renderer);
	~TextureFont();

	void RenderString(const char *str, float x, float y, const Color &color = Color::WHITE);
	Color RenderMarkup(const char *str, float x, float y, const Color &color = Color::WHITE);
	void MeasureString(const char *str, float &w, float &h);
	void MeasureCharacterPos(const char *str, int charIndex, float &x, float &y) const;
	int PickCharacter(const char *str, float mouseX, float mouseY) const;

	// general baseline-to-baseline height
	float GetHeight() const { return m_height; }
	// general descender height
	float GetDescender() const { return m_descender; }

	enum { MAX_FAST_GLYPHS = 256 };

	struct Glyph {
		Glyph() : advX(0), advY(0), width(0), height(0), texWidth(0), texHeight(0), offX(0), offY(0), offU(0), offV(0), ftIndex(0) {}
		float advX, advY;
		float width, height;
		float texWidth, texHeight;
		int offX, offY;
		float offU, offV; //atlas UV offset
		Uint32 ftIndex;
	};
	const Glyph &GetGlyph(Uint32 ch) const { return ch < MAX_FAST_GLYPHS ? m_glyphsFast[ch] : m_glyphs.find(ch)->second; }

	static int GetGlyphCount() { return s_glyphCount; }
	static void ClearGlyphCount() { s_glyphCount = 0; }

	// fill a vertex array with single-colored text
	void CreateGeometry(Graphics::VertexArray &, const char *str, float x, float y, const Color &color = Color::WHITE);
	RefCountedPtr<Graphics::Texture> GetTexture() { return m_texture; }

private:
	TextureFont(const TextureFont &);
	TextureFont &operator=(const TextureFont &);

	FontDescriptor m_descriptor;
	Graphics::Renderer *m_renderer;

	FT_Library m_freeTypeLibrary;
	FT_Face m_face;
	FT_Stroker m_stroker;

	RefCountedPtr<FileSystem::FileData> m_fontFileData;

	int BakeGlyph(Uint32 chr);

	void AddGlyphGeometry(Graphics::VertexArray *va, const Glyph &glyph, float x, float y, const Color &color);
	float m_height;
	float m_descender;
	std::unique_ptr<Graphics::Material> m_mat;
	Graphics::VertexArray m_vertices;
	Graphics::RenderState *m_renderState;

	static int s_glyphCount;

	std::vector<Glyph> m_glyphsFast; // for fast lookup of low-index glyphs
	std::map<Uint32,Glyph> m_glyphs;

	static const Uint32 CHARACTER_RANGES[];

	// UV offsets for glyphs
	int m_atlasU;
	int m_atlasV;
	int m_atlasVIncrement;

	RefCountedPtr<Graphics::Texture> m_texture;
	Graphics::TextureFormat m_texFormat;

	std::vector<unsigned char> m_buf;
	int m_bufWidth, m_bufHeight;
	int m_bpp;
};

}

#endif
