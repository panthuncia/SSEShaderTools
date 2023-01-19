#pragma once

#include "d3d11.h"

namespace ShaderCompiler
{
	ID3D11PixelShader* RegisterPixelShader(const std::wstring a_filePath);
	ID3D11PixelShader* CompileAndRegisterPixelShader(const std::wstring a_filePath, std::vector<D3D_SHADER_MACRO> defines = std::vector<D3D_SHADER_MACRO>());
	ID3D11VertexShader* RegisterVertexShader(const std::wstring a_filePath);
	ID3D11VertexShader* CompileAndRegisterVertexShader(const std::wstring a_filePath, std::vector<D3D_SHADER_MACRO> defines = std::vector<D3D_SHADER_MACRO>());
}
