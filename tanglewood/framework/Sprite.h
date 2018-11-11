///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Sprite.h
// Date:		11th November 2018
// Authors:		Matt Phillips
// Description:	Sprite sheet loader
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include <ion/renderer/Primitive.h>
#include <ion/renderer/Texture.h>
#include <ion/renderer/Material.h>
#include <ion/beehive/SpriteSheet.h>
#include <ion/beehive/SpriteAnimation.h>
#include <ion/beehive/Actor.h>

#include "Constants.h"

#include <map>

class Sprite
{
public:
	struct Sheet
	{
		struct Frame
		{
			ion::render::Texture* texture;
			ion::render::Material* material;
		};

		ion::render::Quad* m_primitive;
		std::vector<Frame> m_frames;
		std::map<std::string, const SpriteAnimation*> m_animations;
	};

	Sprite();
	Sprite(const Actor& actor);
	~Sprite();

	//Load actor from Beehive data
	void LoadActor(const Actor& actor);

	//Load sprite sheet from Beehive data
	void LoadSheet(const SpriteSheet& spriteSheet);

	//Draw sprite sheet to texture with specified palette
	void PaintSheet(const SpriteSheet& spriteSheet, const Palette& palette);

	//Find sprite sheet by name
	const Sheet* FindSheet(const std::string& name) const;

private:
	static const std::vector<ion::render::VertexBuffer::Element> s_vertexLayout;

	std::map<std::string, Sheet> m_sheets;
};