#pragma once

#include <string>

#include "Szczur/Utility/SFML3D/Sprite.hpp"
#include "Szczur/Utility/SFML3D/Texture.hpp"
#include "Szczur/Utility/SFML3D/Drawable.hpp"
#include "Szczur/Utility/SFML3D/RenderTarget.hpp"
#include "Szczur/Utility/SFML3D/RenderStates.hpp"

namespace rat
{

    class SpriteDisplayData : public sf3d::Drawable
    {
    public:
        SpriteDisplayData(const std::string& name);

        const sf3d::Texture& getTexture() const;

        const std::string& getName() const;

        virtual void draw(sf3d::RenderTarget& target, sf3d::RenderStates states) const override;

    protected:
        constexpr static auto _assetsFolderPath = "";
        constexpr static auto _textureFilePath = "";
    private:
        std::string _name;
        sf3d::Sprite _sprite;
        sf3d::Texture _texture;

    };

}