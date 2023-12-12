#pragma once
#include <filesystem>

class Texture2D
{
public:
	Texture2D(const std::filesystem::path& path, bool flip = true);
	~Texture2D();

public:
	uint32_t* pixels = nullptr;
	uint32_t Width = 0;
	uint32_t Height = 0;
};
