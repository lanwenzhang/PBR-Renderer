#pragma once
#include "core.h"
#include <string>


class Texture {
public:
		// Model texture
		static Texture* createTexture(const std::string& path, unsigned int unit);
		static Texture* createTextureFromMemory(const std::string& path, unsigned int unit, unsigned char* dataIn, uint32_t widthIn,uint32_t heightIn);
		Texture();
		Texture(const std::string& path, unsigned int unit, unsigned int internalFormat = GL_RGBA);
		Texture(unsigned int unit, unsigned char* dataIn, uint32_t widthIn, uint32_t heightIn, unsigned int internalFormat = GL_RGBA);
		Texture(unsigned int width, unsigned int height, unsigned int unit, unsigned int internalFormat = GL_RGBA);

		// Fbo texture
		static Texture* createColorAttachment(unsigned int width, unsigned int height, unsigned int unit);
		static Texture* createDepthStencilAttachment(unsigned int width, unsigned int height,unsigned int unit);
		static Texture* createDepthAttachment(unsigned int width, unsigned int height,unsigned int unit);

		// Post-process texture
		static Texture* createMultiSampleTexture(unsigned int width, unsigned int height, unsigned int samples, unsigned int format, unsigned int unit);
		static Texture* createHDRTexture(unsigned int width, unsigned int height, unsigned int unit);

		// Pbr texture
		static Texture* createNearestTexture(std::string path);
		static Texture* createExrTexture(std::string path);
		static Texture* createExrCubeMap(std::vector<std::string> paths);

		// Cube map texture
		Texture(const std::vector<std::string>& paths, unsigned int unit, unsigned int internalFormat = GL_RGBA);

		~Texture();

		void bind();
		void setUnit(unsigned int unit) { mUnit = unit; }

		int getWidth()const { return mWidth; }
		int getHeight() const { return mHeight; }
		GLuint getTexture()const { return mTexture; }

private:
	GLuint mTexture{ 0 };
	int mWidth{ 0 };
	int mHeight{ 0 };
	unsigned int mUnit{ 0 };
	unsigned int mTextureTarget{ GL_TEXTURE_2D };

	static std::map<std::string, Texture*> mTextureCache;
};