#pragma once
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <map>

class Device;
class Attachment;

/**
 * \brief 子通道类
 */
class Subpass
{
    /**
     * \brief 缓存子通道描述
     */
    struct SubpassDescriptionCache
    {
        /**
         * \brief 颜色附件引用
         */
        std::vector<VkAttachmentReference> colorAttachmentReferences;
        /**
         * \brief 颜色附件超采样引用
         */
        std::vector<VkAttachmentReference> colorAttachmentResolverReferences;
        /**
         * \brief 深度附件引用
         */
        VkAttachmentReference depthAttachmentReference;
    };

    /**
     * \brief 设备指针
     */
    std::shared_ptr<Device> m_device;
    /**
     * \brief 记录当前被添加的附件在 attachments 中的下标位置
     */
    std::vector<uint32_t> m_referenceAttachments;
    /**
     * \brief 记录当前被添加的附件的布局
     */
    std::vector<VkImageLayout> m_referenceLayouts;
    /**
     * \brief 记录当前被添加的附件是否是多重采样
     */
    std::vector<bool> m_referenceIsMSAASamples;
    /**
     * \brief 缓存子通道描述构造的结构体，避免子通道描述失效
     */
    std::vector<SubpassDescriptionCache> m_subpassDescriptionCaches;
    /**
     * \brief 多重采样样本数
     */
    VkSampleCountFlagBits m_samples;
    /**
     * \brief 缓存的名字到附件的映射，支持附件可重用
     */
    static std::map<std::string, std::shared_ptr<Attachment>> attachmentMap;

public:
    /**
     * \brief 是否是多重采样
     */
    bool isMSAASample;
    /**
     * \brief 是否有深度附件
     */
    bool hasDepthAttachment;
    /**
     * \brief 所有附件信息
     */
    std::vector<std::shared_ptr<Attachment>> attachments;
    /**
     * \brief 所有子通道描述
     */
    std::vector<VkSubpassDescription> subpassDescriptions;

    /**
     * \brief 构造函数
     * \param device 设备指针
     */
    Subpass(const std::shared_ptr<Device>& device);

    /**
     * \brief 析构函数
     */
    ~Subpass();

    /**
     * \brief 开始子通道附件添加
     * \param samples 多重采样样本数
     */
    void BeginSubpassAttachments(VkSampleCountFlagBits samples);

    /**
     * \brief 结束子通道附件添加，根据已添加的附件构造子通道描述符
     */
    void EndSubpassAttachments();

    /**
     * \brief 添加颜色附件
     * \param name 附件名字
     * \param format 附件格式
     * \param loadOp 附件加载操作
     * \param initialLayout 附件初始布局
     * \param finalLayout 附件最终布局
     */
    void AddColorAttachment(std::string name, VkFormat format, VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    /**
     * \brief 添加超采样颜色附件
     * \param name 附件名字
     */
    void AddColorAttachmentResolver(std::string name);

    /**
     * \brief 添加深度/模板附件
     * \param name 附件名字
     */
    void AssignDepthAttachment(std::string name);
};
