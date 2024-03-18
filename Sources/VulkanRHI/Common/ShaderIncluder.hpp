#pragma once
#include <glslang/Public/ShaderLang.h>

namespace Sandbox
{
    /**
     * \brief 着色器头文件导入类
     */
    class ShaderIncluder : public glslang::TShader::Includer
    {
    public:
        /**
         * \brief 导入相对路径
         */
        IncludeResult *includeLocal(const char*, const char*, size_t) override;

        /**
         * \brief 导入系统路径
         */
        IncludeResult *includeSystem(const char*, const char*, size_t) override;

        /**
         * \brief 释放缓存内容
         */
        void releaseInclude(IncludeResult*) override;
    };
}
