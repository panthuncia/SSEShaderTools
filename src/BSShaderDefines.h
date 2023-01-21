#include "LoaderTypes.h" 
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

//using namespace std;
#pragma once
#pragma push_macro("TEST_BIT")
#undef TEST_BIT
#define TEST_BIT(index) (Technique & (1u << (index)))

namespace BSShaderInfo::Defines
{

	static std::vector<D3D_SHADER_MACRO> GetArray( ShaderLoaderType loaderType, uint32_t Technique)
	{
		std::vector<D3D_SHADER_MACRO> defines;
		uint32_t subType = (Technique >> 24) & 0x3F;
		switch (loaderType) {
		case Lighting:

			if (TEST_BIT(0)) defines.emplace_back("VC", "");
			if (TEST_BIT(1)) defines.emplace_back("SKINNED", "");
			if (TEST_BIT(2)) defines.emplace_back("MODELSPACENORMALS", "");
			if (TEST_BIT(9)) defines.emplace_back("SPECULAR", "");
			if (TEST_BIT(10)) defines.emplace_back("SOFT_LIGHTING", "");
			if (TEST_BIT(13)) defines.emplace_back("SHADOW_DIR", "");
			if (TEST_BIT(14)) defines.emplace_back("DEFSHADOW", "");
			if (TEST_BIT(11)) defines.emplace_back("RIM_LIGHTING", "");
			if (TEST_BIT(12)) defines.emplace_back("BACK_LIGHTING", "");
			if (TEST_BIT(15) && subType != 6) defines.emplace_back("PROJECTED_UV", "");
			if (TEST_BIT(16)) defines.emplace_back("ANISO_LIGHTING", "");
			if (TEST_BIT(17)) defines.emplace_back("AMBIENT_SPECULAR", "");
			if (TEST_BIT(18)) defines.emplace_back("WORLD_MAP", "");
			if (TEST_BIT(20)) defines.emplace_back("DO_ALPHA_TEST", "");
			if (TEST_BIT(21)) defines.emplace_back("SNOW", "");
			if (TEST_BIT(19)) defines.emplace_back("BASE_OBJECT_IS_SNOW", "");
			if (TEST_BIT(22)) defines.emplace_back("CHARACTER_LIGHT", "");
			if (TEST_BIT(15) && subType == 6) defines.emplace_back("DEPTH_WRITE_DECALS", "");
			if (TEST_BIT(23)) defines.emplace_back("ADDITIONAL_ALPHA_MASK", "");

			switch (subType)
			{
			case 0:break;
			case 1:defines.emplace_back("ENVMAP", ""); break;
			case 2:defines.emplace_back("GLOWMAP", ""); break;
			case 3:defines.emplace_back("PARALLAX", ""); break;
			case 4:defines.emplace_back("FACEGEN", ""); break;
			case 5:defines.emplace_back("FACEGEN_RGB_TINT", ""); break;
			case 6:defines.emplace_back("HAIR", ""); break;
			case 7:defines.emplace_back("PARALLAX_OCC", ""); break;
			case 8:defines.emplace_back("MULTI_TEXTURE", ""); defines.emplace_back("LANDSCAPE", ""); break;
			case 9:defines.emplace_back("LODLANDSCAPE", ""); break;
			case 10:/* I have no idea what this does */break;
			case 11:defines.emplace_back("MULTI_LAYER_PARALLAX", ""); defines.emplace_back("ENVMAP", ""); break;
			case 12:defines.emplace_back("TREE_ANIM", ""); break;
			case 13:defines.emplace_back("LODOBJECTS", ""); break;
			case 14:defines.emplace_back("MULTI_INDEX", "SPARKLE"); defines.emplace_back("SPARKLE", ""); break;
			case 15:defines.emplace_back("LODOBJECTSHD", ""); break;
			case 16:defines.emplace_back("EYE", ""); break;
			case 17:defines.emplace_back("CLOUD", ""); defines.emplace_back("INSTANCED", ""); break;
			case 18:defines.emplace_back("LODLANDSCAPE", ""); defines.emplace_back("LODLANDNOISE", ""); break;
			case 19:defines.emplace_back("MULTI_TEXTURE", ""); defines.emplace_back("LANDSCAPE", "LOD_LAND_BLEND"); break;
			default:__debugbreak(); break;
			}
			break;
		case RunGrass:
			if (TEST_BIT(0)) defines.emplace_back("DO_ALPHA", "");

			switch (subType)
			{
			case 0:defines.emplace_back("VERTEXL", ""); break;
			case 1:defines.emplace_back("FLATL", ""); break;
			case 2:defines.emplace_back("FLATL_SLOPE", ""); break;
			case 3:defines.emplace_back("VERTEXL_SLOPE", ""); break;
			case 4:defines.emplace_back("VERTEXL_BILLBOARD", ""); break;
			case 5:defines.emplace_back("FLATL_BILLBOARD", ""); break;
			case 6:defines.emplace_back("FLATL_SLOPE_BILLBOARD", ""); break;
			case 7:defines.emplace_back("VERTEXL_SLOPE_BILLBOARD", ""); break;
			case 8:defines.emplace_back("RENDERDEPTH", ""); break;
			default:__debugbreak(); break;
			}
			break;
		}
		return defines;
	}
	static std::vector<D3D_SHADER_MACRO> loadExternalDefines(){
		std::vector<D3D_SHADER_MACRO> result;
		const auto defines_file = std::filesystem::current_path() /= std::format("Data\\SKSE\\ShaderInjector\\additional_defines.txt"sv);
		std::fstream file(defines_file, std::ios::in);
		std::string line, define;
		if (file.is_open())
		{
			//should be just one line, but just in case
			while (getline(file, line))
			{
				//gtfo random null terminators
				char* line_copy = (char*)malloc(line.length() + 1);
				for (int i = 0; i < line.length(); i++) {
					if (line.c_str()[i] == '\0') {
						line_copy[i] = ' ';
					}
					else {
						line_copy[i] = line.c_str()[i];
					}
				}
				line = line_copy;
				free(line_copy);
				//cut whitespace
				line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
				//pull out pairs
				std::stringstream str(line);
				while (getline(str, define, ';')) {
					int i = 0;
					D3D_SHADER_MACRO macro;
					std::stringstream define_str(define);
					std::string field;
					while (getline(define_str, field, ',')) {
						if (i %2 == 0) {
							char* copy = (char*)malloc(field.size()+1);
							strncpy(copy, field.c_str(), field.size() + 1);
							macro.Name = copy;
						}
						else {
							char* copy = (char*)malloc(field.size()+1);
							strcpy(copy, field.c_str());
							macro.Definition = copy;
						}
						i++;
					}
					result.push_back(macro);
				}
			}
		}
		/*for (auto s : result) {
			logger::info("adding external define: {}", s.Name);
		}*/
		return result;
	}
}

#undef TEST_BIT
#pragma pop_macro("TEST_BIT")
