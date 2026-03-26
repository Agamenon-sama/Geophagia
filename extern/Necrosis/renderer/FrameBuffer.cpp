#include "FrameBuffer.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <slog/slog.h>

#include "../Window.h"

namespace Necrosis {

Framebuffer::Framebuffer(const glm::ivec4 &viewport, FramebufferType type) : _viewport(viewport) {
    // int viewport[4];
    glGetIntegerv(GL_VIEWPORT, glm::value_ptr(_defaultViewport));

    glGenFramebuffers(1, &_id);
    glBindFramebuffer(GL_FRAMEBUFFER, _id);

    glGenTextures(1, &_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);


    if (type == FramebufferType::Color) {
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB, _viewport[2], _viewport[3],
            0, GL_RGB, GL_UNSIGNED_BYTE, nullptr
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

        uint32_t rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _viewport[2], _viewport[3]);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }
    else if (type == FramebufferType::Depth) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _viewport[2], _viewport[3],
            0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr
        );

        glCreateSamplers(1, &_sampler);
        glObjectLabel(GL_SAMPLER, _sampler, -1, "Shadow map sampler");

        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glSamplerParameteri(_sampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _texture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // Window::showErrorMessageBox("Failed to create framebuffer");
        slog::error("Framebuffer could not be created");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glViewport(_defaultViewport[0], _defaultViewport[1], _defaultViewport[2], _defaultViewport[3]);
}

void Framebuffer::bindTexture(const u8 unit) const {
    glBindTexture(GL_TEXTURE_2D, _texture);
    glBindTextureUnit(unit, _texture);
    glBindSampler(unit, _sampler);
}

i32 Framebuffer::getWidth() const { return _viewport[2]; }
i32 Framebuffer::getHeight() const { return _viewport[3]; }
glm::ivec4 Framebuffer::getViewport() const { return _viewport; }
u32 Framebuffer::getTextureID() const { return _texture; }

}
