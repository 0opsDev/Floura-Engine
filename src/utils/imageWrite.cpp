#include "imageWrite.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <json/json.hpp>
#include <fstream>
using json = nlohmann::json;

bool FlouraImageWrite::writeImage2DToDisk(unsigned int ID, int width, int height, const char* filePath, GLenum format, GLenum type, int channels){
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    std::vector<unsigned char> pixels(width * height * channels);
    glGetTexImage(GL_TEXTURE_2D, 0, format, type, pixels.data()); // GL_RGBA //GL_UNSIGNED_BYTE
    
    stbi_flip_vertically_on_write(false);
    bool r = stbi_write_png(filePath, width, height, channels, pixels.data(), width * 4);
    return r;
}

bool FlouraImageWrite::writeImage3DToDiskPNG(unsigned int ID, int width, int height, int depth, const char* filePath,
    GLenum format, GLenum type, int channels){
    glBindTexture(GL_TEXTURE_3D, ID);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    std::vector<unsigned char> sourceVolume(width * height * depth * channels);
    glGetTexImage(GL_TEXTURE_3D, 0, format, type, sourceVolume.data()); // GL_RGBA //GL_UNSIGNED_BYTE
    
    int cols = std::ceil(std::sqrt(depth));
    int rows = std::ceil((double)depth / cols);
    
    int atlasWidth = width * cols;
    int atlasHeight = height * rows;
    
    std::vector<unsigned char> pixels(atlasWidth * atlasHeight * channels, 0);
    
    int sliceSize = width * height * channels;
    int rowSize = width * channels;
    int atlasRowStride = atlasWidth * channels;

    for (int z = 0; z < depth; ++z){
        int colIdx = z % cols;
        int rowIdx = z / cols;
        
        int startX = colIdx * width;
        int startY = rowIdx * height;

        for (int y = 0; y < height; ++y){
            unsigned char* srcRow = sourceVolume.data() + (z * sliceSize) + (y * rowSize);
            
            int destY = startY + y;
            unsigned char* destRow = pixels.data() + (destY * atlasRowStride) + (startX * channels);
            
            std::memcpy(destRow, srcRow, rowSize);
        }
    }
    
    stbi_flip_vertically_on_write(false);
    bool r = stbi_write_png(filePath, atlasWidth, atlasHeight, channels, pixels.data(), atlasRowStride);
    
    return r;
}

bool FlouraImageWrite::writeImage3DToDiskJSON(unsigned int ID, int width, int height, int depth, const char* filePath,
    GLenum format, GLenum type, int channels){
    glBindTexture(GL_TEXTURE_3D, ID);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    std::vector<float> sdfData(width * height * depth * channels);
    glGetTexImage(GL_TEXTURE_3D, 0, format, type, sdfData.data()); // GL_RGBA //GL_UNSIGNED_BYTE
    
    try {
        json imageData = json::array();
        json headerJson;
        headerJson["type"] = type;
        headerJson["format"] = format;
        headerJson["channels"] = channels;
        headerJson["width"] = width;
        headerJson["height"] = height;
        headerJson["depth"] = depth;
        
        // I prefer this info at the top
        imageData.push_back(headerJson);
        
        json dataArray = json::array();
        for (size_t x = 0; x < sdfData.size(); x++){
            json pixel; pixel["value"] = sdfData[x];
            dataArray.push_back(pixel);
        }
        headerJson["DATA"] = dataArray;
        
        imageData.push_back(headerJson);
        
        std::ofstream outFile(filePath, std::ios::out);
        if (!outFile.is_open()) {
            std::cout << "Failed to write to " << filePath << std::endl;
            return false;
        }

        outFile << imageData.dump(4);  
        outFile.close();

        //std::cout << "Successfully updated " << filePath << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return false;
    }
    
    return true;
}
