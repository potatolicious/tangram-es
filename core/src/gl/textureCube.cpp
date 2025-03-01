#include "textureCube.h"
#include "platform.h"
#include "stb_image.h"

#include <cstring> // for memcpy
#include <cstdlib>

namespace Tangram {

TextureCube::TextureCube(std::string _file, TextureOptions _options) : Texture(0, 0, _options) {

    m_target = GL_TEXTURE_CUBE_MAP;
    load(_file);
}

void TextureCube::load(const std::string& _file) {
    unsigned int size;
    unsigned char* data = bytesFromResource(_file.c_str(), &size);
    unsigned char* pixels;
    int width, height, comp;

    pixels = stbi_load_from_memory(data, size, &width, &height, &comp, STBI_rgb_alpha);

    size = width * height;

    m_width = width / 4;
    m_height = height / 3;

    for (int i = 0; i < 6; ++i) {
        GLenum face = CubeMapFace[i];
        m_faces.push_back({face, {}, 0});
        m_faces.back().m_data.resize(m_width * m_height);
    }

    for (int y = 0; y < height; ++y) {
        int jFace = (y - (y % m_height)) / m_height;

        for (int iFace = 0; iFace < 4; ++iFace) {
            Face* face = nullptr;

            if (iFace == 2 && jFace == 1) { face = &m_faces[0]; } // POS_X
            if (iFace == 0 && jFace == 1) { face = &m_faces[1]; } // NEG_X
            if (iFace == 1 && jFace == 0) { face = &m_faces[2]; } // POS_Y
            if (iFace == 1 && jFace == 2) { face = &m_faces[3]; } // NEG_Y
            if (iFace == 1 && jFace == 1) { face = &m_faces[4]; } // POS_Z
            if (iFace == 3 && jFace == 1) { face = &m_faces[5]; } // NEG_Z

            if (!face) { continue; }

            int offset = (m_width * iFace + y * width) * sizeof(unsigned int);
            std::memcpy(face->m_data.data() + face->m_offset, pixels + offset, m_width * sizeof(unsigned int));
            face->m_offset += m_width;
        }
    }

    free(data);
    stbi_image_free(pixels);

    update(0);
}

void TextureCube::update(GLuint _textureUnit) {

    checkValidity();

    if (m_glHandle != 0 || m_faces.size() == 0) { return; }

    generate(_textureUnit);

    for (int i = 0; i < 6; ++i) {
        Face& f = m_faces[i];
        glTexImage2D(CubeMapFace[i], 0, m_options.m_internalFormat, m_width, m_height, 0, m_options.m_format, GL_UNSIGNED_BYTE, f.m_data.data());
    }
}

}
