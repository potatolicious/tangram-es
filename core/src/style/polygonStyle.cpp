#include "polygonStyle.h"

#include "tangram.h"
#include "util/builders.h"
#include "gl/shaderProgram.h"
#include "tile/tile.h"
#include "glm/gtx/normal.hpp"

namespace Tangram {

PolygonStyle::PolygonStyle(std::string _name, GLenum _drawMode) : Style(_name, _drawMode) {
}

void PolygonStyle::constructVertexLayout() {

    // TODO: Ideally this would be in the same location as the struct that it basically describes
    m_vertexLayout = std::shared_ptr<VertexLayout>(new VertexLayout({
        {"a_position", 3, GL_FLOAT, false, 0},
        {"a_normal", 3, GL_FLOAT, false, 0},
        {"a_texcoord", 2, GL_FLOAT, false, 0},
        {"a_color", 4, GL_UNSIGNED_BYTE, true, 0},
        {"a_layer", 1, GL_FLOAT, false, 0}
    }));

}

void PolygonStyle::constructShaderProgram() {

    std::string vertShaderSrcStr = stringFromResource("polygon.vs");
    std::string fragShaderSrcStr = stringFromResource("polygon.fs");

    m_shaderProgram->setSourceStrings(fragShaderSrcStr, vertShaderSrcStr);
}

PolygonStyle::Parameters PolygonStyle::parseRule(const DrawRule& _rule) const {
    Parameters p;
    _rule.getColor(StyleParamKey::color, p.color);
    _rule.getValue(StyleParamKey::order, p.order);

    return p;
}

void PolygonStyle::buildLine(const Line& _line, const DrawRule& _rule, const Properties& _props, VboMesh& _mesh, Tile& _tile) const {
    std::vector<PolygonVertex> vertices;

    Parameters params = parseRule(_rule);

    GLuint abgr = params.color;
    GLfloat layer = params.order;

    PolyLineBuilder builder = {
        [&](const glm::vec3& coord, const glm::vec2& normal, const glm::vec2& uv) {
            float halfWidth =  0.2f;

            glm::vec3 point(coord.x + normal.x * halfWidth, coord.y + normal.y * halfWidth, coord.z);
            vertices.push_back({ point, glm::vec3(0.0f, 0.0f, 1.0f), uv, abgr, layer });
        },
        [&](size_t sizeHint){ vertices.reserve(sizeHint); }
    };

    Builders::buildPolyLine(_line, builder);

    auto& mesh = static_cast<PolygonStyle::Mesh&>(_mesh);
    mesh.addVertices(std::move(vertices), std::move(builder.indices));
}

void PolygonStyle::buildPolygon(const Polygon& _polygon, const DrawRule& _rule, const Properties& _props, VboMesh& _mesh, Tile& _tile) const {

    std::vector<PolygonVertex> vertices;

    Parameters params = parseRule(_rule);

    GLuint abgr = params.color;
    GLfloat layer = params.order;

    if (Tangram::getDebugFlag(Tangram::DebugFlags::proxy_colors)) {
        abgr = abgr << (_tile.getID().z % 6);
    }

    const static std::string key_height("height");
    const static std::string key_min_height("min_height");

    float height = _props.getNumeric(key_height) * _tile.getInverseScale();
    float minHeight = _props.getNumeric(key_min_height) * _tile.getInverseScale();

    PolygonBuilder builder = {
        [&](const glm::vec3& coord, const glm::vec3& normal, const glm::vec2& uv){
            vertices.push_back({ coord, normal, uv, abgr, layer });
        },
        [&](size_t sizeHint){ vertices.reserve(sizeHint); }
    };

    if (minHeight != height) {
        Builders::buildPolygonExtrusion(_polygon, minHeight, height, builder);
    }

    Builders::buildPolygon(_polygon, height, builder);

    auto& mesh = static_cast<PolygonStyle::Mesh&>(_mesh);
    mesh.addVertices(std::move(vertices), std::move(builder.indices));
}


void PolygonStyle::addVertex(glm::vec3 p, glm::vec3 n, GLuint abgr, float layer,
                   std::vector<int>& indices,
                   std::vector<PolygonVertex>& vertices) const {
  auto id = vertices.size();
  vertices.push_back({ p, n, glm::vec2(0), abgr, layer });
  indices.push_back(id);
}

void PolygonStyle::buildMesh(const std::vector<uint16_t>& indices,
                             const std::vector<Point>& points,
                             const DrawRule& _rule,
                             const Properties& _props,
                             VboMesh& _mesh, Tile& _tile) const {
    GLuint abgr = 0xffe6f0f2;
    std::vector<PolygonVertex> vertices;
    GLfloat layer = 1;

    std::vector<int> newIndices;
    vertices.reserve(indices.size() * 3);

    for (size_t i = 0; i < indices.size(); i += 3) {
        auto p1 = points[indices[i + 0]];
        auto p2 = points[indices[i + 1]];
        auto p3 = points[indices[i + 2]];

        auto a = p2 - p1;
        auto b = p3 - p1;

        auto c = glm::cross(a, b);
        auto n = glm::normalize(glm::vec3(0,0,0.25)) - glm::normalize(c);
        //auto n =  glm::normalize(c);

        addVertex(p1, n, abgr, layer, newIndices, vertices);
        addVertex(p3, n, abgr, layer, newIndices, vertices);
        addVertex(p2, n, abgr, layer, newIndices, vertices);
    }

    auto& mesh = static_cast<PolygonStyle::Mesh&>(_mesh);
    mesh.addVertices(std::move(vertices), std::move(newIndices));
}

}
