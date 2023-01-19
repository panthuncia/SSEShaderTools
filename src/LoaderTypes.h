#pragma once
enum ShaderLoaderType {UNKNOWN, Lighting, RunGrass};

ShaderLoaderType getShaderLoaderType(const char* typeString) {
	if (strcmp(typeString, "Lighting")) {
		return Lighting;
	}
	if (strcmp(typeString, "RunGrass")) {
		return RunGrass;
	}
	return UNKNOWN;
}
