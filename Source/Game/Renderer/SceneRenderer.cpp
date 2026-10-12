#include "FunkinPCH.h"
#include "SceneRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

CSceneRenderer::CSceneRenderer(CRenderer& Renderer)
    : m_Renderer2D(Renderer) {}

void CSceneRenderer::Render()
{
    // The world is 1280 x 720 units, with the origin at the top-left and +Y pointing down, no matter the window's size.
    constexpr float ViewWidth = 1280.0f;
    constexpr float ViewHeight = 720.0f;
    
    // Bottom is 0 and top is ViewHeight (not flipped) on purpose: Vulkan's clip space has +Y pointing down, so this puts world y = 0 at the top of the screen.
    // The ZO suffix is Vulkan's [0, 1] depth range.
    m_Renderer2D.BeginFrame(glm::orthoRH_ZO(0.0f, ViewWidth, 0.0f, ViewHeight, -1.0f, 1.0f));
    
    // Blue, at the bottom.
    m_Renderer2D.DrawQuad(0, glm::translate(glm::mat4(1.0f), { 300.0f, 150.0f, 0.0f }) * glm::scale(glm::mat4(1.0f), { 500.0f, 300.0f, 1.0f }), { 0.2f, 0.3f, 0.9f, 1.0f });
    
    // Translucent green, rotated around its top-left corner.
    m_Renderer2D.DrawQuad(1, glm::translate(glm::mat4(1.0f), { 450.0f, 150.0f, 0.0f }) * glm::rotate(glm::mat4(1.0f), 0.5f, { 0.0f, 0.0f, 1.0f }) *
        glm::scale(glm::mat4(1.0f), { 300.0f, 300.0f, 1.0f }), { 0.2f, 0.8f, 0.3f, 0.75f });
    
    // Red, on top.
    m_Renderer2D.DrawQuad(2, glm::translate(glm::mat4(1.0f), { 490.0f, 210.0f, 0.0f }) * glm::scale(glm::mat4(1.0f), { 300.0f, 300.0f, 1.0f }), { 0.9f, 0.2f, 0.2f, 1.0f });
    
    m_Renderer2D.EndFrame();
}
