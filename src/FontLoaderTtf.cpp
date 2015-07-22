/*
 * The GammaEngine Library 2.0 is a multiplatform Vulkan-based game engine
 * Copyright (C) 2015  Adrien Aubry <dridri85@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "FontLoaderTtf.h"
#include "File.h"
#include "Image.h"
#include "Instance.h"
#include "Debug.h"

namespace GE {

FT_Library FontLoaderTtf::ft_library;


FontLoaderTtf::FontLoaderTtf()
	: FontLoader()
{
	FT_Init_FreeType( &ft_library );
}


std::vector< std::string > FontLoaderTtf::contentPatterns()
{
	std::vector< std::string > ret;
	return ret;
}


std::vector< std::string > FontLoaderTtf::extensions()
{
	std::vector< std::string > ret;
	ret.emplace_back( "ttf" );
	ret.emplace_back( "otf" );
	return ret;
}


FontLoader* FontLoaderTtf::NewInstance()
{
	return new FontLoaderTtf();
}


void FontLoaderTtf::Load( Instance* instance, File* file, uint32_t size )
{
	fDebug( instance, file, size );

	uint64_t file_size = file->Seek( 0, File::END );
	mData = (uint8_t*)instance->Malloc( file_size );
	file->Rewind();
	file->Read( mData, file_size );

	FT_Face face;
	int error = FT_New_Memory_Face( ft_library, mData, file_size, 0, &face );
	if(error){
		instance->Free( mData );
		return;
	}

	mFace = face;
	mTexture = nullptr;

	FT_Select_Charmap( ((FT_Face)mFace), FT_ENCODING_UNICODE );
}


void FontLoaderTtf::resize( Font* font, int size )
{
	fDebug( font, size );

	FT_Set_Pixel_Sizes( (FT_Face)font->face(), 0, size );

	FT_Face face = (FT_Face)font->face();
	FT_GlyphSlot slot = face->glyph;
	uint16_t n;
	uint16_t chr = 0;
	int x = 0;
	int y = 0;
	bool first_null_char = true;

	int total_width = 0;
	int advY = 0;
	int advX = 0;

	for ( n = 0; n < 256; n++ ) {
		chr = n;
		FT_UInt glyph_index = FT_Get_Char_Index( face, chr );
		int error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
		if ( error ) continue;
		error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
		if ( error ) continue;
		if ( !glyph_index && !first_null_char ) {
			continue;
		}
		total_width += slot->bitmap.width;
		advX = std::max( advX, std::max( (int)slot->advance.x >> 6, std::max( (int)slot->bitmap.width, mSize ) ) );
		advY = std::max( advY, (int)slot->bitmap.rows );
		first_null_char = false;
	}

//	int side = std::max( advX * 16, advY * 16 );
//	Image* texture = font->reallocTexture( side, side );
	Image* texture = font->reallocTexture( advX * 16, advY * 16 );

	first_null_char = true;
	y = advY;

	for ( n = 0; n < 256; n++ ) {
		chr = n;
		FT_UInt glyph_index = FT_Get_Char_Index( face, chr );
		int error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
		if ( error ) continue;
		error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
		if ( error ) continue;

		if ( (uint32_t)( x + advX ) > texture->width() ) {
			x = 0;
			y += advY;
		}

		font->glyphs()[n].x = std::max( x, 0 );
		font->glyphs()[n].y = y - slot->bitmap_top;
		font->glyphs()[n].w = slot->bitmap.width + slot->bitmap_left;
		font->glyphs()[n].h = slot->bitmap.rows;
		font->glyphs()[n].advX = slot->advance.x >> 6;
		font->glyphs()[n].posY = slot->bitmap_top;

		if ( !glyph_index && !first_null_char ) {
			continue;
		}

		fontPrintTextImpl2( &slot->bitmap, x + slot->bitmap_left, y - slot->bitmap_top, texture->data(), texture->width(), texture->height() );

		x += advX;
		first_null_char = false;
	}
}


void FontLoaderTtf::fontPrintTextImpl2( FT_Bitmap* bitmap, int xofs, int yofs, uint32_t* framebuffer, int width, int height )
{
// 	fDebug( bitmap, xofs, yofs, framebuffer, width, height );
	int x, y;

	uint8_t* line = bitmap->buffer;
	uint32_t* fbLine = framebuffer + xofs + yofs * width;
	for ( y = 0; y < bitmap->rows; y++ ) {
		uint8_t* column = line;
		uint32_t* fbColumn = fbLine;
		for ( x = 0; x < bitmap->width; x++ ) {
			if (x + xofs < width && x + xofs >= 0 && y + yofs < height && y + yofs >= 0){
				uint8_t val = *column;
				*fbColumn = ( val << 24 ) | 0x00FFFFFF;
			}
			column++;
			fbColumn++;
		}
		line += bitmap->pitch;
		fbLine += width;
	}
}


} // namespace GE
