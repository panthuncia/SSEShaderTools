#include "BSShader.h"
#include "ShaderCompiler.h"
#include "LoaderTypes.h"
#include "BSShaderDefines.h"
#include "API/ENBSeriesAPI.h"

namespace BSShaderHooks
{
	typedef void (*LoadShaders_t)(REX::BSShader* shader, std::uintptr_t stream);
	REL::Relocation<LoadShaders_t*> LoadShaders{ RELOCATION_ID(101339, 108326) };
	LoadShaders_t oLoadShaders;

	void loadIndividualShaders(REX::BSShader* bsShader, std::uintptr_t stream) {
		if (strcmp("Lighting", bsShader->m_LoaderType) == 0) {
			std::unordered_map<REX::TechniqueID, std::wstring> techniqueFileMap;

			const auto shaderDir = std::filesystem::current_path() /= "Data/SKSE/plugins/shaders/"sv;

			if (std::filesystem::exists(shaderDir)) {

				std::size_t foundCount = 0;
				std::size_t successCount = 0;
				std::size_t failedCount = 0;

				for (const auto& entry : std::filesystem::directory_iterator(shaderDir)) {
					if (entry.path().extension().generic_string() != ".hlsl"sv)
						continue;

					auto filenameStr = entry.path().filename().string();
					auto techniqueIdStr = filenameStr.substr(0, filenameStr.find('_'));
					const REX::TechniqueID techniqueId = std::strtoul(techniqueIdStr.c_str(), nullptr, 16);
					logger::info("found shader technique id {:08x} with path {}", techniqueId, entry.path().generic_string());
					foundCount++;
					techniqueFileMap.insert(std::make_pair(techniqueId, absolute(entry.path()).wstring()));
				}

				for (const auto& entry : bsShader->m_PixelShaderTable) {
					auto tFileIt = techniqueFileMap.find(entry->m_TechniqueID);
					if (tFileIt != techniqueFileMap.end()) {
						if (const auto shader = ShaderCompiler::CompileAndRegisterPixelShader(tFileIt->second)) {
							logger::info("shader compiled successfully, replacing old shader");
							successCount++;
							entry->m_Shader = shader;
						}
						else {
							failedCount++;
						}
					}
				}
			}
		}

		std::unordered_map<REX::TechniqueID, std::wstring> techniqueFileMap;
		std::unordered_map<REX::TechniqueID, std::wstring> vsTechniqueFileMap;

		const auto shaderDir = std::filesystem::current_path() /= std::format("Data\\Shaders\\{}\\"sv, bsShader->m_LoaderType);
		if (std::filesystem::exists(shaderDir)) {
			logger::info("{}", shaderDir.generic_string());

			std::size_t foundCount = 0;
			std::size_t successCount = 0;
			std::size_t failedCount = 0;

			for (const auto& entry : std::filesystem::directory_iterator(shaderDir)) {
				bool isVs = false;
				std::string fileStr = entry.path().filename().generic_string();

				std::string techniqueIDStr;
				REX::TechniqueID techniqueId;
				if (fileStr.ends_with(".ps.hlsl")) {
					techniqueIDStr = fileStr.substr(0, fileStr.length() - 8);
					techniqueId = std::strtoul(techniqueIDStr.c_str(), nullptr, 16);
					auto tFileIt = techniqueFileMap.find(techniqueId);
					if (tFileIt != techniqueFileMap.end() && !tFileIt->second.ends_with(L".hlsl"))
						continue;  // favour compiled binary blobs
				}
				else if (fileStr.ends_with(".ps")) {
					techniqueIDStr = fileStr.substr(0, fileStr.length() - 3);
					techniqueId = std::strtoul(techniqueIDStr.c_str(), nullptr, 16);
					continue;
				}
				else if (fileStr.ends_with(".vs.hlsl")) {
					isVs = true;
					techniqueIDStr = fileStr.substr(0, fileStr.length() - 8);
					techniqueId = std::strtoul(techniqueIDStr.c_str(), nullptr, 16);
					auto tFileIt = vsTechniqueFileMap.find(techniqueId);
					if (tFileIt != vsTechniqueFileMap.end() && !tFileIt->second.ends_with(L".hlsl"))
						continue;  // favour compiled binary blobs
				}
				else if (fileStr.ends_with(".vs")) {
					isVs = true;
					techniqueIDStr = fileStr.substr(0, fileStr.length() - 3);
					techniqueId = std::strtoul(techniqueIDStr.c_str(), nullptr, 16);
					continue;
				}
				else {
					continue;
				}

				logger::info("found shader technique id {:08x} with path {}", techniqueId, entry.path().generic_string());
				foundCount++;
				if (isVs) {
					vsTechniqueFileMap.insert(std::make_pair(techniqueId, absolute(entry.path()).wstring()));
				}
				else {
					techniqueFileMap.insert(std::make_pair(techniqueId, absolute(entry.path()).wstring()));
				}
			}

			for (const auto& entry : bsShader->m_PixelShaderTable) {
				auto tFileIt = techniqueFileMap.find(entry->m_TechniqueID);
				if (tFileIt != techniqueFileMap.end()) {
					bool compile = tFileIt->second.ends_with(L".hlsl");
					if (const auto psShader = compile ? ShaderCompiler::CompileAndRegisterPixelShader(tFileIt->second) : ShaderCompiler::RegisterPixelShader(tFileIt->second)) {
						logger::info("shader compiled successfully, replacing old shader");
						successCount++;
						entry->m_Shader = psShader;
					}
					else {
						failedCount++;
					}
				}
			}

			for (const auto& entry : bsShader->m_VertexShaderTable) {
				auto tFileIt = vsTechniqueFileMap.find(entry->m_TechniqueID);
				if (tFileIt != vsTechniqueFileMap.end()) {
					bool compile = tFileIt->second.ends_with(L".hlsl");
					if (const auto vsShader = compile ? ShaderCompiler::CompileAndRegisterVertexShader(tFileIt->second) : ShaderCompiler::RegisterVertexShader(tFileIt->second)) {
						logger::info("shader compiled successfully, replacing old shader");
						successCount++;
						entry->m_Shader = vsShader;
					}
					else {
						failedCount++;
					}
				}
			}

			logger::info("found shaders: {} successfully replaced: {} failed to replace: {}", foundCount, successCount, failedCount);
		}
	}

	void loadCompleteShaders(REX::BSShader* bsShader, std::uintptr_t stream){
		const auto lightingVSpath = std::filesystem::current_path() /= std::format("Data\\Shaders\\{}\\{}.hlsl"sv, bsShader->m_LoaderType, bsShader->m_LoaderType);
		ShaderLoaderType loaderType = getShaderLoaderType(bsShader->m_LoaderType);
		if (std::filesystem::exists(lightingVSpath))
		{
			_MESSAGE("replacing vertex shaders");
			size_t replaceCount = 0;
			std::size_t foundCount = 0;
			std::size_t successCount = 0;
			std::size_t failedCount = 0;
			for (const auto & entry : bsShader->m_VertexShaderTable)
			{
				_MESSAGE("replacing vertex shader technique {}", entry->m_TechniqueID);

				auto defines = BSShaderInfo::Defines::GetArray(loaderType, entry->m_TechniqueID);
				auto newShader = ShaderCompiler::CompileAndRegisterVertexShader(lightingVSpath.wstring(), defines);
				if (newShader)
				{
					entry->m_Shader = newShader;
					/*newShader->m_TechniqueID = entry->m_TechniqueID;
					for (int i = 0; i < ::BSShader::CONSTANT_GROUP_LEVEL_COUNT; i++)
					{
						newShader->m_ConstantGroups[i].m_Buffer = entry->m_ConstantGroups[i].m_Buffer;
						newShader->m_ConstantGroups[i].m_Data = entry->m_ConstantGroups[i].m_Data;
					}
					for (int i = 0; i < ::BSShader::MAX_VS_CONSTANTS; i++)
						newShader->m_ConstantOffsets[i] = entry->m_ConstantOffsets[i];
					newShader->m_VertexDescription = entry->m_VertexDescription;
					bsShader->m_VertexShaderTable.insert_or_assign(newShader);
					replaceCount++;*/
				}
				else
				{
					failedCount++;
				}
			}
			_MESSAGE("replaced {} shaders, map size {}", replaceCount, bsShader->m_VertexShaderTable.size());
		}

		const auto lightingPSpath = std::filesystem::current_path() /= std::format("Data\\Shaders\\{}\\{}.hlsl"sv, bsShader->m_LoaderType, bsShader->m_LoaderType);
		if (std::filesystem::exists(lightingPSpath))
		{
			_MESSAGE("replacing pixel shaders");
			size_t replaceCount = 0;
			size_t skipCount = 0;
			size_t failedCount = 0;
			for (const auto& entry : bsShader->m_PixelShaderTable)
			{
				const uint32_t baseTechniqueID = (entry->m_TechniqueID >> 24) & 0x3F;
					
				_MESSAGE("replacing pixel shader technique {} ({})", entry->m_TechniqueID, baseTechniqueID);
				if (baseTechniqueID == 14)
				{
					_MESSAGE("8, 14, 19 unavailable, skipping");
					skipCount++;
					continue;
				}

				std::vector<D3D_SHADER_MACRO> defines = BSShaderInfo::Defines::GetArray(loaderType, entry->m_TechniqueID);
				auto newShader = ShaderCompiler::CompileAndRegisterPixelShader(lightingPSpath.wstring(), defines);
				if (newShader)
				{
					entry->m_Shader = newShader;
					/*newShader->m_TechniqueID = entry->m_TechniqueID;
					for (int i = 0; i < ::BSShader::CONSTANT_GROUP_LEVEL_COUNT; i++)
					{
						newShader->m_ConstantGroups[i].m_Buffer = entry->m_ConstantGroups[i].m_Buffer;
						newShader->m_ConstantGroups[i].m_Data = entry->m_ConstantGroups[i].m_Data;
					}
					for (int i = 0; i < ::BSShader::MAX_PS_CONSTANTS; i++)
						newShader->m_ConstantOffsets[i] = entry->m_ConstantOffsets[i];
					bsShader->m_PixelShaderTable.insert_or_assign(newShader);
					replaceCount++;*/
				}
				else
				{
					failedCount++;
				}
			}
			_MESSAGE("replaced {} shaders, skipped {} shaders, failed compile {} shaders, map size {}", replaceCount, skipCount, failedCount, bsShader->m_PixelShaderTable.size());
		}
	}

	void hk_LoadShaders(REX::BSShader* bsShader, std::uintptr_t stream)
	{
		oLoadShaders(bsShader, stream);

		logger::info("BSShader::LoadShaders called on {} - ps count {} -  vs count {}", bsShader->m_LoaderType, bsShader->m_PixelShaderTable.size(), bsShader->m_VertexShaderTable.size());

		loadIndividualShaders(bsShader, stream);

		loadCompleteShaders(bsShader, stream);
	}

	void Install()
	{
		ENB_API::ENBSDK1001* g_ENB = reinterpret_cast<ENB_API::ENBSDK1001*>(ENB_API::RequestENBAPI(ENB_API::SDKVersion::V1001));
		if (!g_ENB) {
			logger::info("Installing BSShader::LoadShaders hook");
			{
				struct Patch : Xbyak::CodeGenerator
				{
					Patch()
					{
						Xbyak::Label origFuncJzLabel;

						test(rdx, rdx);
						jz(origFuncJzLabel);
						jmp(ptr[rip]);
						dq(LoadShaders.address() + 0x9);

						L(origFuncJzLabel);
						jmp(ptr[rip]);
						dq(LoadShaders.address() + 0xF0);
					}
				};

				Patch p;
				p.ready();

				auto& trampoline = SKSE::GetTrampoline();
				oLoadShaders = static_cast<LoadShaders_t>(trampoline.allocate(p));
				trampoline.write_branch<6>(
					LoadShaders.address(),
					hk_LoadShaders);
			}
			logger::info("Installed");
		}
		else {
			logger::info("ENB detected, not installing hooks");
		}
	}
}
