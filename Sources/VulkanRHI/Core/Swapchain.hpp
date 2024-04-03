#pragma once
#include <memory>
#include <vector>

#include "Misc/Event.hpp"
#include "vulkan/vulkan_core.h"

namespace Sandbox {
class Semaphore;
class Surface;
class Device;
class Window;
class ImageView;

enum ESwapchainStatus
{
    Continue,
    Recreate,
    Failure
};

class Swapchain
{
  public:
    Swapchain(const std::shared_ptr<Device>& device, const std::shared_ptr<Surface>& surface);

    ~Swapchain();

    void Create(const std::shared_ptr<Device>& device, const std::shared_ptr<Surface>& surface);

    std::vector<std::shared_ptr<ImageView>> CreateImageViews(VkFormat format);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    void ParseCapabilities(const std::shared_ptr<Window>& window, const VkSurfaceCapabilitiesKHR& capabilities, uint32_t& imageCount, VkExtent2D& actualExtent);

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    void Cleanup();

    ESwapchainStatus AcquireNextImageIndex(const std::shared_ptr<Semaphore>& semaphore);

    void Recreate();

    ESwapchainStatus Preset(const std::vector<std::shared_ptr<Semaphore>>& inPresentWaitSemaphores);

    VkSwapchainKHR vkSwapchainKhr;

    std::vector<VkImage> vkImages;

    std::vector<std::shared_ptr<ImageView>> imageViews;

    VkExtent2D imageExtent;

    uint32_t acquiredNextImageIndex;

    Event<void> onBeforeRecreateSwapchain;
    Event<void> onAfterRecreateSwapchain;

  private:
    bool m_cleaned = false;
    std::shared_ptr<Surface> m_surface;
    std::shared_ptr<Device> m_device;
};
} // namespace Sandbox
