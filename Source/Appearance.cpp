#include "Appearance.h"

Appearance::Appearance(const Geometry& geometry,
                       const Material& material)
    : _geometry(geometry),
      _material(material)
{
}

Appearance::Appearance(const Geometry& geometry,
                       const Material& material,
                       ID3D11ShaderResourceView* texture)
    : _geometry(geometry),
      _material(material)
{
    _textureRV = texture;
}

Appearance::Appearance(const Appearance& appearance)
    : _geometry(appearance._geometry),
      _material(appearance._material)
{
    if (appearance._textureRV)
    {
        _textureRV = appearance._textureRV;
    }
}

Appearance::~Appearance()
{
    _textureRV.Reset();
}
