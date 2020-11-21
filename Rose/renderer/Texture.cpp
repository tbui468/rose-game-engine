#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "renderer/Texture.h"


namespace rose {

Texture::Texture(int texSlot) {
    glGenTextures(1, &m_RendererID);
    m_TextureSlot = texSlot;
    Bind();
}


void Texture::Bind() {
    //glActiveTexture(GL_TEXTURE0 + m_TextureSlot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
/*
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cout << err;
    }*/
}


void Texture::LoadTexture(const std::string& path) {
    
    stbi_set_flip_vertically_on_load(true);
    int32_t channels;
    uint8_t *data = stbi_load(path.c_str(), &m_Width, &m_Height, &channels, 0);

//    if(stbi_failure_reason()) std::cout << stbi_failure_reason() << std::endl;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);

}



}
