#pragma once
#include <glslang/Public/ShaderLang.h>

class ShaderIncluder : public glslang::TShader::Includer
{
public:
	IncludeResult *includeLocal(const char*, const char*, size_t) override;
	IncludeResult *includeSystem(const char*, const char*, size_t) override;
	void releaseInclude(IncludeResult*) override;
};
