#pragma once

/**
 * \brief 视图模式
 */
enum EViewMode
{
    /**
     * \brief 着色
     */
    Lit = 0,
    /**
     * \brief 无着色
     */
    Unlit,
    /**
     * \brief 线框
     */
    Wireframe,
    /**
     * \brief 深度缓冲区
     */
    DepthBuffer
};

/**
 * \brief 视图模式名称
 */
inline const char* VIEW_MODE_NAMES[] = {"Lit", "Unlit", "Wireframe", "DepthBuffer"};