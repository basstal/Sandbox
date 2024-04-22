#pragma once
#include <memory>
#include <vector>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    class Device;
    struct Attachment;
    struct LoadStoreInfo;
    struct SubpassInfo;

    class RenderPass
    {
    public:
        RenderPass(const std::shared_ptr<Device>& device, const std::vector<Attachment>& inAttachments, const std::vector<LoadStoreInfo>& inLoadStoreInfos,
                   const std::vector<SubpassInfo>& inSubpassInfos);

        RenderPass(const std::shared_ptr<Device>& device, const std::vector<Attachment>& inAttachments, const std::vector<LoadStoreInfo>& inLoadStoreInfos,
                        const std::vector<SubpassInfo>& inSubpassInfos, VkSubpassDependency inSubpassDependency);
        void CreateRenderPass(VkSubpassDependency inSubpassDependency);

        VkSubpassDependency CreateDefaultDependency();

        std::vector<VkAttachmentDescription> GetAttachmentDescriptions(const std::vector<Attachment>& inAttachments, const std::vector<LoadStoreInfo>& inLoadStoreInfos);

        ~RenderPass();

        void Cleanup();

        VkRenderPass               vkRenderPass;
        std::vector<Attachment>    attachments;
        std::vector<LoadStoreInfo> loadStoreInfo;
        std::vector<SubpassInfo>   subpasses;

        std::shared_ptr<Device> GetDevice() const;

    private:
        std::shared_ptr<Device> m_device;
        bool                    m_cleaned = false;
    };
}  // namespace Sandbox
