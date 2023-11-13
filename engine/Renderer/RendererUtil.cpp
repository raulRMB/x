#include "RendererUtil.h"
#include <stb_image.h>

namespace xUtil {

void FreeImage(stbi_uc* imageData) {
    stbi_image_free(imageData);
}

stbi_uc* LoadTextureFile(const std::string& fileName, i32* width, i32* height, VkDeviceSize* imageSize)
{
    i32 channels;

    std::string fileLoc = "../assets/textures/" + fileName;
    stbi_uc* image;
    if(image = stbi_load(fileLoc.c_str(), width, height, &channels, STBI_rgb_alpha); !image)
    {
        throw std::runtime_error("Failed to load a texture file! (" + fileName + ")");
    }

    *imageSize = *width * *height * 4;

    return image;
}

}

