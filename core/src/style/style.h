#pragma once

#include "data/tileData.h"
#include "material.h"
#include "gl.h"
#include "gl/shaderProgram.h"
#include "gl/renderState.h"
#include "scene/sceneLayer.h"

#include <memory>
#include <string>
#include <vector>

namespace Tangram {

class Light;
class Tile;
class MapProjection;
class VboMesh;
class VertexLayout;
class View;
class Scene;
using Context = std::unordered_map<std::string, Value>;

enum class LightingType : char {
    none,
    vertex,
    fragment
};

/* Means of constructing and rendering map geometry
 *
 * A Style defines a way to
 *   1. Construct map geometry into a mesh for drawing and
 *   2. Render the resulting mesh in a scene
 * Style implementations must provide functions to construct
 * a <VertexLayout> for their geometry, construct a <ShaderProgram>
 * for rendering meshes, and build point, line, and polygon
 * geometry into meshes. See <PolygonStyle> for a basic implementation.
 */
class Style {

protected:

    /* The platform pixel scale */
    float m_pixelScale = 1.0;

    /* Unique name for a style instance */
    std::string m_name;

    /* <ShaderProgram> used to draw meshes using this style */
    std::unique_ptr<ShaderProgram> m_shaderProgram = std::make_unique<ShaderProgram>();

    /* <VertexLayout> shared between meshes using this style */
    std::shared_ptr<VertexLayout> m_vertexLayout;

    /* <Material> used for drawing meshes that use this style */
    std::shared_ptr<Material> m_material = std::make_shared<Material>();

    /* <LightingType> to determine how lighting will be calculated for this style */
    LightingType m_lightingType = LightingType::fragment;

    /* Draw mode to pass into <VboMesh>es created with this style */
    GLenum m_drawMode;

    /* Whether the viewport has changed size */
    bool m_dirtyViewport = true;

    /* Create <VertexLayout> corresponding to this style; subclasses must implement this and call it on construction */
    virtual void constructVertexLayout() = 0;

    /* Create <ShaderProgram> for this style; subclasses must implement this and call it on construction */
    virtual void constructShaderProgram() = 0;

    /* Build styled vertex data for point geometry and add it to the given <VboMesh> */
    virtual void buildPoint(const Point& _point, const DrawRule& _rule, const Properties& _props, VboMesh& _mesh, Tile& _tile) const;

    /* Build styled vertex data for line geometry and add it to the given <VboMesh> */
    virtual void buildLine(const Line& _line, const DrawRule& _rule, const Properties& _props, VboMesh& _mesh, Tile& _tile) const;

    /* Build styled vertex data for polygon geometry and add it to the given <VboMesh> */
    virtual void buildPolygon(const Polygon& _polygon, const DrawRule& _rule, const Properties& _props, VboMesh& _mesh, Tile& _tile) const;

    /* Create a new mesh object using the vertex layout corresponding to this style */
    virtual VboMesh* newMesh() const = 0;
    
    /* Toggle on read if true, checks whether the context has been lost on last frame */
    bool glContextLost();

private:
    
    /* Whether the context has been lost on last frame */
    bool m_contextLost;

public:

    Style(std::string _name, GLenum _drawMode);

    virtual ~Style();
    
    void notifyGLContextLost() { m_contextLost = true; }

    void viewportHasChanged() { m_dirtyViewport = true; }

    /* Whether or not the style uses blending operation for drawing */
    virtual bool isOpaque() const { return true; };

    /* Make this style ready to be used (call after all needed properties are set) */
    virtual void build(const std::vector<std::unique_ptr<Light>>& _lights);

    void buildFeature(Tile& _tile, const Feature& _feat, const DrawRule& _rule) const;

    /* Perform any needed setup to process the data for a tile */
    virtual void onBeginBuildTile(Tile& _tile) const;

    /* Perform any needed teardown after processing data for a tile */
    virtual void onEndBuildTile(Tile& _tile) const;

    /* Perform any setup needed before drawing each frame */
    virtual void onBeginDrawFrame(const View& _view, const Scene& _scene);

    /* Perform any unsetup needed after drawing each frame */
    virtual void onEndDrawFrame() {}

    virtual void setLightingType(LightingType _lType);

    void setMaterial(const std::shared_ptr<Material>& _material);

    void setPixelScale(float _pixelScale) { m_pixelScale = _pixelScale; }

    std::shared_ptr<Material> getMaterial() { return m_material; }

    const std::unique_ptr<ShaderProgram>& getShaderProgram() const { return m_shaderProgram; }

    const std::string& getName() const { return m_name; }

};

}
