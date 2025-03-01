#pragma once

#include "style.h"
#include "text/fontContext.h"
#include "text/textBuffer.h"

#include <memory>

namespace Tangram {

class TextStyle : public Style {

protected:

    virtual void constructVertexLayout() override;
    virtual void constructShaderProgram() override;

    virtual void buildPoint(const Point& _point, const DrawRule& _rule, const Properties& _props, VboMesh& _mesh, Tile& _tile) const override;
    virtual void buildLine(const Line& _line, const DrawRule& _rule, const Properties& _props, VboMesh& _mesh, Tile& _tile) const override;
    virtual void buildPolygon(const Polygon& _polygon, const DrawRule& _rule, const Properties& _props, VboMesh& _mesh, Tile& _tile) const override;
    virtual void onBeginBuildTile(Tile& _tile) const override;
    virtual void onEndBuildTile(Tile& _tile) const override;

    virtual VboMesh* newMesh() const override {
        return new TextBuffer(m_vertexLayout);
    };

    /*
     * Creates a text label and add it to the processed <TextBuffer>.
     */
    void addTextLabel(TextBuffer& _buffer, Label::Transform _transform, std::string _text, Label::Type _type) const;

    std::string m_fontName;
    float m_fontSize;
    int m_color;
    bool m_sdf;
    bool m_sdfMultisampling = true;
    bool m_dirtyColor = true;

public:

    bool isOpaque() const override { return false; }

    TextStyle(const std::string& _fontName, std::string _name, float _fontSize, unsigned int _color = 0xffffff,
              bool _sdf = false, bool _sdfMultisampling = false, GLenum _drawMode = GL_TRIANGLES);

    virtual void onBeginDrawFrame(const View& _view, const Scene& _scene) override;
    void setColor(unsigned int _color);

    virtual ~TextStyle();

};

}
