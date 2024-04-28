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
                        const std::vector<SubpassInfo>& inSubpassInfos, const std::vector<VkSubpassDependency2KHR>& inSubpassDependency);
        void AttachmentReferenceMaker(std::vector<VkAttachmentReference2KHR>& result, const std::vector<uint32_t>& attachmentIndices,
                                      const std::vector<VkAttachmentDescription2KHR>& attachmentDescriptions);

        void CreateRenderPass(const std::vector<VkSubpassDependency2KHR>& inSubpassDependency);

        static VkSubpassDependency2KHR CreateDefaultDependency();

        std::vector<VkAttachmentDescription2KHR> GetAttachmentDescriptions(const std::vector<Attachment>& inAttachments, const std::vector<LoadStoreInfo>& inLoadStoreInfos);

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
