#pragma once
#include <memory>
#include <string>
#include <boost/hana.hpp>
#include <glm/vec4.hpp>

#include <Rendering/ViewMode.hpp>
#include <Supports/YamlGlmConverter.hpp>

struct CameraPersistence;

/**
 * \brief 渲染器持久化数据
 */
struct RendererPersistence
{
    /**
     * \brief 是否是窗口模式
     */
    bool isWindow = false;
    /**
     * \brief 窗口位置 X
     */
    int windowPositionX = 0;
    /**
     * \brief 窗口位置 Y
     */
    int windowPositionY = 0;
    /**
     * \brief 窗口宽度
     */
    int width = 1920;
    /**
     * \brief 窗口高度
     */
    int height = 1080;
    /**
     * \brief 编辑器相机
     */
    std::shared_ptr<CameraPersistence> editorCamera;
    /**
     * \brief 应用程序名称
     */
    std::string applicationName = "Sandbox";
    /**
     * \brief 视图模式
     */
    EViewMode viewMode = EViewMode::Lit;
    /**
     * \brief 清除颜色
     */
    glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
};


// 使 Settings 与 Boost.Hana 兼容
BOOST_HANA_ADAPT_STRUCT(RendererPersistence, isWindow, windowPositionX, windowPositionY, width, height, editorCamera, applicationName,
                        viewMode, clearColor);
