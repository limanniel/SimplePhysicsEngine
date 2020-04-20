#include "Appearance.h"

Appearance::Appearance(const Geometry& geometry,
                       const Material& material)
    : _geometry(geometry),
      _material(material)
{
}

Appearance::~Appearance()
{
    _textureRV.Reset();
}
