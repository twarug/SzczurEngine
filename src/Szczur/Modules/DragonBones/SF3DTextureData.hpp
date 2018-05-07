/** @file SF3DTextureAtlasData.hpp
** @author Piotr Krupa (piotrkrupa06@gmail.com)
** @author Patryk (PsychoX) Ludwikowski <psychoxivi@gmail.com>
**/

#pragma once

#include <memory>

#include <dragonBones/DragonBonesHeaders.h>

#include <glm/glm.hpp> // vec2

#include "Szczur/Utility/SFML3D/Texture.hpp"

DRAGONBONES_NAMESPACE_BEGIN

class SF3DTextureData : public TextureData
{
	BIND_CLASS_TYPE_B(SF3DTextureData);

public:
	sf3d::Texture*			texture;

public:
	SF3DTextureData()
	{
		_onClear();
	}

	virtual ~SF3DTextureData()
	{
		_onClear();
	}

	void _onClear() override
	{
		texture = nullptr;

		TextureData::_onClear();
	}

	void setTexture(sf3d::Texture *value, sf::FloatRect& rect)
	{
		region.x = rect.left;
		region.y = rect.top;
		region.width = rect.width;
		region.height = rect.height;

		texture = value;
	}

	const glm::vec2& getSize() const
	{
		return this->texture->getSize();
	}
};

DRAGONBONES_NAMESPACE_END