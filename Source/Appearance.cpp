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

Appearance::~Appearance()
{
    _textureRV.Reset();
}
