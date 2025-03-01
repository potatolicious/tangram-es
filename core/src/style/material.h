/* MATERIAL
-------------------------------
This openGL Material implementation follows from the WebGL version of Tangram
( https://github.com/tangrams/tangram/wiki/Materials-Overview )
*/

#pragma once

#include <memory>
#include <string>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace Tangram {

class Texture;
class ShaderProgram;

enum class MappingType {
    uv,
    planar,
    triplanar,
    spheremap
};

struct MaterialTexture {
    std::shared_ptr<Texture> tex = nullptr;
    MappingType mapping = MappingType::uv;
    glm::vec3 scale = glm::vec3(1.f);
    glm::vec3 amount = glm::vec3(1.f);
};

class Material {
public:

    Material();

    virtual ~Material(){};

    /*  Emission color is by default disabled and vec4(0.0).
     *  Setting this property enables it and changes require reloading the shader. */
    void setEmission(const glm::vec4 _emission);
    void setEmission(MaterialTexture _emissionTexture);

    /*  Ambient color is by default disabled and vec4(1.0).
     *  Setting this property enables it and changes require reloading the shader. */
    void setAmbient(const glm::vec4 _ambient);
    void setAmbient(MaterialTexture _ambientTexture);

    /*  Diffuse color is by default enabled and vec4(1.0).
     *  Changes require reloading the shader. */
    void setDiffuse(const glm::vec4 _diffuse);
    void setDiffuse(MaterialTexture _diffuseTexture);

    /*  Specular color is by default disabled and vec4(0.2) with a shininess factor of 0.2.
     *  Setting this property enables it and changes require reloading the shader. */
    void setSpecular(const glm::vec4 _specular);
    void setSpecular(MaterialTexture _specularTexture);

    void setShininess(float _shiny);

    /* Enable or disable emission colors */
    void setEmissionEnabled(bool _enable);

    /* Enable or disable ambient colors */
    void setAmbientEnabled(bool _enable);

    /* Enable or disable diffuse colors */
    void setDiffuseEnabled(bool _enable);

    /* Enable or disable specular colors */
    void setSpecularEnabled(bool _enable);

    void setNormal(MaterialTexture _normalTexture);

    /*  Inject the needed lines of GLSL code on the shader to make this material work */
    virtual void injectOnProgram(ShaderProgram& _shader);

    /*  Method to pass it self as a uniform to the shader program */
    virtual void setupProgram(ShaderProgram& _shader);

private:

    /* Get defines that need to be injected on top of the shader */
    std::string getDefinesBlock();

    /* Get the GLSL struct and classes need to be injected */
    std::string getClassBlock();

    bool m_bEmission = false;
    glm::vec4 m_emission = glm::vec4(1.f);
    MaterialTexture m_emission_texture;

    bool m_bAmbient = true;
    glm::vec4 m_ambient = glm::vec4(1.f);
    MaterialTexture m_ambient_texture;

    bool m_bDiffuse = true;
    glm::vec4 m_diffuse = glm::vec4(1.f);
    MaterialTexture m_diffuse_texture;

    bool m_bSpecular = false;
    glm::vec4 m_specular = glm::vec4(.2f);
    MaterialTexture m_specular_texture;
    
    MaterialTexture m_normal_texture;

    float m_shininess = .2f;

};

}
