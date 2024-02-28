#pragma once


#include "Subpass.hpp"
#include "Swapchain.hpp"


// enum RenderPassType
// {
// 	GAME_RENDER_PASS = 0,
// 	EDITOR_RENDER_PASS = 1
// };

class RenderPass
{
private:
    std::shared_ptr<Device> m_device;
    std::shared_ptr<Swapchain> m_swapchain;
    bool m_cleaned = false;

public:
    VkRenderPass vkRenderPass;
    bool enableMSAA = false;
    bool enableDepth = false;
    std::vector<VkAttachmentDescription> attachments;
    // std::vector<bool> isAttachmentClearDepthStencil;
    std::shared_ptr<Subpass> subpass;

    RenderPass(const std::shared_ptr<Device>& device, const std::shared_ptr<Subpass>& inSubpass);

    ~RenderPass();

    void CreateEditorRenderPass();

    void CreateGameRenderPass();

    VkRenderPass CreateCubeMapRenderPass();

    // VkFormat FindDepthFormat();
    void Cleanup();

    void BeginRenderPass(VkCommandBuffer vkCommandBuffer, std::shared_ptr<Framebuffer> framebuffer, VkExtent2D vkExtent2D,
                         VkClearColorValue clearColorValue, VkClearDepthStencilValue clearDepthStencilValue);
};
