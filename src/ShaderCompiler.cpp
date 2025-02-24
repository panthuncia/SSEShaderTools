#include "d3d11.h"
#include "d3dcompiler.h"

namespace ShaderCompiler
{
	REL::Relocation<ID3D11Device**> g_ID3D11Device{ RELOCATION_ID(524729, 411348) };

	ID3D11PixelShader* RegisterPixelShader(const std::wstring a_filePath)
	{
		ID3DBlob* shaderBlob = nullptr;

		if (FAILED(D3DReadFileToBlob(a_filePath.c_str(), &shaderBlob))) {
			logger::error("Pixel shader load failed:\n{}", "File does not exist or is invalid");

			if (shaderBlob)
				shaderBlob->Release();

			return nullptr;
		}

		logger::debug("shader load succeeded");

		logger::debug("registering shader");

		ID3D11PixelShader* regShader;

		if (FAILED((*g_ID3D11Device)->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &regShader))) {
			logger::error("pixel shader registration failed");

			if (shaderBlob)
				shaderBlob->Release();

			return nullptr;
		}

		logger::debug("shader registration succeeded");

		return regShader;
	}

	ID3D11PixelShader* CompileAndRegisterPixelShader(const std::wstring a_filePath, std::vector<D3D_SHADER_MACRO> defines = std::vector<D3D_SHADER_MACRO>())
	{
		D3D_SHADER_MACRO* macros = (D3D_SHADER_MACRO*)calloc(defines.size() + 4, sizeof(D3D_SHADER_MACRO));
		macros[defines.size()].Name = "WINPC";
		macros[defines.size()].Definition = "";
		macros[defines.size() + 1].Name = "DX11";
		macros[defines.size() + 1].Definition = "";
		macros[defines.size() + 2].Name = "PSHADER";
		macros[defines.size() + 2].Definition = "";
		macros[defines.size() + 3].Name = NULL;
		macros[defines.size() + 3].Definition = NULL;
		for (int i = 0; i < defines.size(); i++) {
			macros[i] = defines[i];
		}

		UINT compilerFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* shaderErrors = nullptr;

		if (FAILED(D3DCompileFromFile(a_filePath.c_str(), macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", compilerFlags, 0, &shaderBlob, &shaderErrors))) {
			free(macros);
			logger::error("Pixel shader compilation failed:\n{}", shaderErrors ? (const char*)shaderErrors->GetBufferPointer() : "Unknown error");

			if (shaderBlob)
				shaderBlob->Release();

			if (shaderErrors)
				shaderErrors->Release();

			return nullptr;
		}
		free(macros);
		if (shaderErrors)
			shaderErrors->Release();

		logger::debug("shader compilation succeeded");

		logger::debug("registering shader");

		ID3D11PixelShader* regShader;

		if (FAILED((*g_ID3D11Device)->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &regShader))) {
			logger::error("pixel shader registration failed");

			if (shaderBlob)
				shaderBlob->Release();

			return nullptr;
		}

		logger::debug("shader registration succeeded");

		return regShader;
	}

	ID3D11VertexShader* RegisterVertexShader(const std::wstring a_filePath)
	{
		ID3DBlob* shaderBlob = nullptr;

		if (FAILED(D3DReadFileToBlob(a_filePath.c_str(), &shaderBlob))) {
			logger::error("Vertex shader load failed:\n{}", "File does not exist or is invalid");

			if (shaderBlob)
				shaderBlob->Release();

			return nullptr;
		}

		logger::debug("shader load succeeded");

		logger::debug("registering shader");

		ID3D11VertexShader* regShader;

		if (FAILED((*g_ID3D11Device)->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &regShader))) {
			logger::error("vertex shader registration failed");

			if (shaderBlob)
				shaderBlob->Release();

			return nullptr;
		}

		logger::debug("shader registration succeeded");

		return regShader;
	}

	ID3D11VertexShader* CompileAndRegisterVertexShader(const std::wstring a_filePath, std::vector<D3D_SHADER_MACRO> defines = std::vector<D3D_SHADER_MACRO>())
	{
		D3D_SHADER_MACRO* macros = (D3D_SHADER_MACRO*)calloc(defines.size() + 4, sizeof(D3D_SHADER_MACRO));
		macros[defines.size()].Name = "WINPC";
		macros[defines.size()].Definition = "";
		macros[defines.size()+1].Name = "DX11";
		macros[defines.size()+1].Definition = "";
		macros[defines.size()+2].Name = "VSHADER";
		macros[defines.size()+2].Definition = "";
		macros[defines.size() + 3].Name = NULL;
		macros[defines.size() + 3].Definition = NULL;
		for (int i = 0; i < defines.size(); i++) {
			macros[i] = defines[i];
		}
		UINT compilerFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* shaderErrors = nullptr;
		if (FAILED(D3DCompileFromFile(a_filePath.c_str(), macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", compilerFlags, 0, &shaderBlob, &shaderErrors))) {
			free(macros);
			logger::error("vertex shader compilation failed:\n{}", shaderErrors ? (const char*)shaderErrors->GetBufferPointer() : "Unknown error");

			if (shaderBlob)
				shaderBlob->Release();

			if (shaderErrors)
				shaderErrors->Release();

			return nullptr;
		}
		free(macros);
		if (shaderErrors)
			shaderErrors->Release();

		logger::debug("shader compilation succeeded");

		logger::debug("registering shader");
		ID3D11VertexShader* regShader;

		if (FAILED((*g_ID3D11Device)->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &regShader))) {
			logger::error("vertex shader registration failed");

			if (shaderBlob)
				shaderBlob->Release();

			return nullptr;
		}
		logger::debug("shader registration succeeded");

		return regShader;
	}

	void* CompileAndRegisterEffectShader(const std::wstring a_filePath, std::vector<D3D_SHADER_MACRO> defines = std::vector<D3D_SHADER_MACRO>())
	{
		D3D_SHADER_MACRO* macros = (D3D_SHADER_MACRO*)calloc(defines.size() + 4, sizeof(D3D_SHADER_MACRO));
		macros[defines.size()].Name = "WINPC";
		macros[defines.size()].Definition = "";
		macros[defines.size() + 1].Name = "DX11";
		macros[defines.size() + 1].Definition = "";
		macros[defines.size() + 2].Name = "PSHADER";
		macros[defines.size() + 2].Definition = "";
		macros[defines.size() + 3].Name = NULL;
		macros[defines.size() + 3].Definition = NULL;
		for (int i = 0; i < defines.size(); i++) {
			macros[i] = defines[i];
		}

		UINT compilerFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* shaderErrors = nullptr;

		if (FAILED(D3DCompileFromFile(a_filePath.c_str(), macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "Draw", "fx_5_0", compilerFlags, 0, &shaderBlob, &shaderErrors))) {
			free(macros);
			logger::error("Pixel shader compilation failed:\n{}", shaderErrors ? (const char*)shaderErrors->GetBufferPointer() : "Unknown error");

			if (shaderBlob)
				shaderBlob->Release();

			if (shaderErrors)
				shaderErrors->Release();

			return nullptr;
		}
		free(macros);
		if (shaderErrors)
			shaderErrors->Release();

		logger::debug("shader compilation succeeded");

		logger::debug("registering shader");

		ID3D11PixelShader* regShader;

		if (FAILED((*g_ID3D11Device)->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &regShader))) {
			logger::error("pixel shader registration failed");

			if (shaderBlob)
				shaderBlob->Release();

			return nullptr;
		}

		logger::debug("shader registration succeeded");

		return regShader;
	}
}
