#pragma once

#include "textStyle.h"
#include "tangram.h"

namespace Tangram {

class DebugTextStyle : public TextStyle {

protected:

    virtual void onBeginBuildTile(Tile& _tile) const override;

public:

    DebugTextStyle(const std::string& _fontName, std::string _name, float _fontSize, unsigned int _color, bool _sdf = false, GLenum _drawMode = GL_TRIANGLES);

    virtual bool isOpaque() const override { return false; };
};

}
