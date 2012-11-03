#include "Base.h"
#include "Image.h"
#include "Texture.h"
#include "FileSystem.h"

// PVRTC (GL_IMG_texture_compression_pvrtc) : Imagination based gpus
#ifndef GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#endif
#ifndef GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03
#endif
#ifndef GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#endif
#ifndef GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#endif

// S3TC/DXT (GL_EXT_texture_compression_s3tc) : Most desktop/console gpus
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif

// ATC (GL_AMD_compressed_ATC_texture) : Qualcomm/Adreno based gpus
#ifndef ATC_RGB_AMD
#define ATC_RGB_AMD 0x8C92
#endif
#ifndef ATC_RGBA_EXPLICIT_ALPHA_AMD
#define ATC_RGBA_EXPLICIT_ALPHA_AMD 0x8C93
#endif
#ifndef ATC_RGBA_INTERPOLATED_ALPHA_AMD
#define ATC_RGBA_INTERPOLATED_ALPHA_AMD 0x87EE
#endif

namespace gameplay
{

static std::vector<Texture*> __textureCache;
static TextureHandle __currentTextureId;

Texture::Texture() : _handle(0), _format(UNKNOWN), _width(0), _height(0), _mipmapped(false), _cached(false), _compressed(false)
{
}

Texture::~Texture()
{
    if (_handle)
    {
        GL_ASSERT( glDeleteTextures(1, &_handle) );
        _handle = 0;
    }

    // Remove ourself from the texture cache.
    if (_cached)
    {
        std::vector<Texture*>::iterator itr = std::find(__textureCache.begin(), __textureCache.end(), this);
        if (itr != __textureCache.end())
        {
            __textureCache.erase(itr);
        }
    }
}

Texture* Texture::create(const char* path, bool generateMipmaps)
{
    GP_ASSERT(path);

    // Search texture cache first.
    for (size_t i = 0, count = __textureCache.size(); i < count; ++i)
    {
        Texture* t = __textureCache[i];
        GP_ASSERT(t);
        if (t->_path == path)
        {
            // If 'generateMipmaps' is true, call Texture::generateMipamps() to force the 
            // texture to generate its mipmap chain if it hasn't already done so.
            if (generateMipmaps)
            {
                t->generateMipmaps();
            }

            // Found a match.
            t->addRef();

            return t;
        }
    }

    Texture* texture = NULL;

    // Filter loading based on file extension.
    const char* ext = strrchr(FileSystem::resolvePath(path), '.');
    if (ext)
    {
        switch (strlen(ext))
        {
        case 4:
            if (tolower(ext[1]) == 'p' && tolower(ext[2]) == 'n' && tolower(ext[3]) == 'g')
            {
                Image* image = Image::create(path);
                if (image)
                    texture = create(image, generateMipmaps);
                SAFE_RELEASE(image);
            }
            else if (tolower(ext[1]) == 'p' && tolower(ext[2]) == 'v' && tolower(ext[3]) == 'r')
            {
                // PowerVR Compressed Texture RGBA.
                texture = createCompressedPVRTC(path);
            }
            else if (tolower(ext[1]) == 'd' && tolower(ext[2]) == 'd' && tolower(ext[3]) == 's')
            {
                // DDS file format (DXT/S3TC) compressed textures
                texture = createCompressedDDS(path);
            }
            break;
        }
    }

    if (texture)
    {
        texture->_path = path;
        texture->_cached = true;

        // Add to texture cache.
        __textureCache.push_back(texture);

        return texture;
    }

    GP_ERROR("Failed to load texture from file '%s'.", path);
    return NULL;
}

Texture* Texture::create(Image* image, bool generateMipmaps)
{
    GP_ASSERT(image);

    switch (image->getFormat())
    {
    case Image::RGB:
        return create(Texture::RGB, image->getWidth(), image->getHeight(), image->getData(), generateMipmaps);
    case Image::RGBA:
        return create(Texture::RGBA, image->getWidth(), image->getHeight(), image->getData(), generateMipmaps);
    default:
        GP_ERROR("Unsupported image format (%d).", image->getFormat());
        return NULL;
    }
}

Texture* Texture::create(Format format, unsigned int width, unsigned int height, unsigned char* data, bool generateMipmaps)
{
    // Create and load the texture.
    GLuint textureId;
    GL_ASSERT( glGenTextures(1, &textureId) );
    GL_ASSERT( glBindTexture(GL_TEXTURE_2D, textureId) );
    GL_ASSERT( glPixelStorei(GL_UNPACK_ALIGNMENT, 1) );
    GL_ASSERT( glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)format, width, height, 0, (GLenum)format, GL_UNSIGNED_BYTE, data) );

    // Set initial minification filter based on whether or not mipmaping was enabled.
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR) );

    Texture* texture = new Texture();
    texture->_handle = textureId;
    texture->_format = format;
    texture->_width = width;
    texture->_height = height;
    if (generateMipmaps)
    {
        texture->generateMipmaps();
    }

    // Restore the texture id
    GL_ASSERT( glBindTexture(GL_TEXTURE_2D, __currentTextureId) );

    return texture;
}

Texture* Texture::create(TextureHandle handle, int width, int height, Format format)
{
    GP_ASSERT(handle);

    Texture* texture = new Texture();
    texture->_handle = handle;
    texture->_format = format;
    texture->_width = width;
    texture->_height = height;

    return texture;
}

// Computes the size of a PVRTC data chunk for a mipmap level of the given size.
static unsigned int computePVRTCDataSize(int width, int height, int bpp)
{
    int blockSize;
    int widthBlocks;
    int heightBlocks;

    if (bpp == 4)
    {
        blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
        widthBlocks = std::max(width >> 2, 2);
        heightBlocks = std::max(height >> 2, 2);
    }
    else
    {
        blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
        widthBlocks = std::max(width >> 3, 2);
        heightBlocks = std::max(height >> 2, 2);
    }

    return widthBlocks * heightBlocks * ((blockSize  * bpp) >> 3);
}

Texture* Texture::createCompressedPVRTC(const char* path)
{
    FILE* file = FileSystem::openFile(path, "rb");
    if (file == NULL)
    {
        GP_ERROR("Failed to load file '%s'.", path);
        return NULL;
    }

    // Read first 4 bytes to determine PVRTC format.
    size_t read;
    unsigned int version;
    read = fread(&version, sizeof(unsigned int), 1, file);
    if (read != 1)
    {
        GP_ERROR("Failed to read PVR version.");
        if (fclose(file) != 0)
        {
            GP_ERROR("Failed to close PVR file '%s'.", path);
            return NULL;
        }
        return NULL;
    }

    // Rewind to start of header.
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        GP_ERROR("Failed to seek backwards to beginning of file after reading PVR version.");
        if (fclose(file) != 0)
        {
            GP_ERROR("Failed to close PVR file '%s'.", path);
            return NULL;
        }
        return NULL;
    }

    // Read texture data.
    GLsizei width, height;
    GLenum format;
    GLubyte* data = NULL;
    unsigned int mipMapCount;

    if (version == 0x03525650)
    {
        // Modern PVR file format.
        data = readCompressedPVRTC(path, file, &width, &height, &format, &mipMapCount);
    }
    else
    {
        // Legacy PVR file format.
        data = readCompressedPVRTCLegacy(path, file, &width, &height, &format, &mipMapCount);
    }
    if (data == NULL)
    {
        GP_ERROR("Failed to read texture data from PVR file '%s'.", path);
        if (fclose(file) != 0)
        {
            GP_ERROR("Failed to close PVR file '%s'.", path);
            return NULL;
        }
        return NULL;
    }

    if (fclose(file) != 0)
    {
        GP_ERROR("Failed to close PVR file '%s'.", path);
        return NULL;
    }

    int bpp = (format == GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG || format == GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG) ? 2 : 4;

    // Generate our texture.
    GLuint textureId;
    GL_ASSERT( glGenTextures(1, &textureId) );
    GL_ASSERT( glBindTexture(GL_TEXTURE_2D, textureId) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipMapCount > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) );

    Texture* texture = new Texture();
    texture->_handle = textureId;
    texture->_width = width;
    texture->_height = height;
    texture->_mipmapped = mipMapCount > 1;
    texture->_compressed = true;

    // Load the data for each level.
    GLubyte* ptr = data;
    for (unsigned int level = 0; level < mipMapCount; ++level)
    {
        unsigned int dataSize = computePVRTCDataSize(width, height, bpp);

        // Upload data to GL.
        GL_ASSERT( glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, dataSize, ptr) );

        width = std::max(width >> 1, 1);
        height = std::max(height >> 1, 1);
        ptr += dataSize;
    }

    // Free data.
    SAFE_DELETE_ARRAY(data);

    return texture;
}

GLubyte* Texture::readCompressedPVRTC(const char* path, FILE* file, GLsizei* width, GLsizei* height, GLenum* format, unsigned int* mipMapCount)
{
    GP_ASSERT(file);
    GP_ASSERT(path);
    GP_ASSERT(width);
    GP_ASSERT(height);
    GP_ASSERT(format);
    GP_ASSERT(mipMapCount);

    struct pvrtc_file_header
    {
        unsigned int version;
        unsigned int flags;
        unsigned int pixelFormat[2];
        unsigned int colorSpace;
        unsigned int channelType;
        unsigned int height;
        unsigned int width;
        unsigned int depth;
        unsigned int surfaceCount;
        unsigned int faceCount;
        unsigned int mipMapCount;
        unsigned int metaDataSize;
    };

    size_t read;

    // Read header data.
    pvrtc_file_header header;
    read = fread(&header, sizeof(pvrtc_file_header), 1, file);
    if (read != 1)
    {
        GP_ERROR("Failed to read PVR header data for file '%s'.", path);
        return NULL;
    }

    if (header.pixelFormat[1] != 0)
    {
        // Unsupported pixel format.
        GP_ERROR("Unsupported pixel format in PVR file '%s'. (MSB == %d != 0)", path, header.pixelFormat[1]);
        return NULL;
    }

    int bpp;
    switch (header.pixelFormat[0])
    {
    case 0:
        *format = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
        bpp = 2;
        break;
    case 1:
        *format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
        bpp = 2;
        break;
    case 2:
        *format = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
        bpp = 4;
        break;
    case 3:
        *format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
        bpp = 4;
        break;
    default:
        // Unsupported format.
        GP_ERROR("Unsupported pixel format value (%d) in PVR file '%s'.", header.pixelFormat[0], path);
        return NULL;
    }

    *width = (GLsizei)header.width;
    *height = (GLsizei)header.height;
    *mipMapCount = header.mipMapCount;

    // Skip meta-data.
    if (fseek(file, header.metaDataSize, SEEK_CUR) != 0)
    {
        GP_ERROR("Failed to seek past header meta data in PVR file '%s'.", path);
        return NULL;
    }

    // Compute total size of data to be read.
    int w = *width;
    int h = *height;
    size_t dataSize = 0;
    for (unsigned int level = 0; level < header.mipMapCount; ++level)
    {
        dataSize += computePVRTCDataSize(w, h, bpp);
        w = std::max(w>>1, 1);
        h = std::max(h>>1, 1);
    }

    // Read data.
    GLubyte* data = new GLubyte[dataSize];
    read = fread(data, 1, dataSize, file);
    if (read != dataSize)
    {
        SAFE_DELETE_ARRAY(data);
        GP_ERROR("Failed to read texture data from PVR file '%s'.", path);
        return NULL;
    }

    return data;
}

GLubyte* Texture::readCompressedPVRTCLegacy(const char* path, FILE* file, GLsizei* width, GLsizei* height, GLenum* format, unsigned int* mipMapCount)
{
    char PVRTCIdentifier[] = "PVR!";

    struct pvrtc_file_header_legacy
    {
        unsigned int size;                  // size of the structure
        unsigned int height;                // height of surface to be created
        unsigned int width;                 // width of input surface
        unsigned int mipmapCount;           // number of mip-map levels requested
        unsigned int formatflags;           // pixel format flags
        unsigned int dataSize;              // total size in bytes
        unsigned int bpp;                   // number of bits per pixel
        unsigned int redBitMask;            // mask for red bit
        unsigned int greenBitMask;          // mask for green bits
        unsigned int blueBitMask;           // mask for blue bits
        unsigned int alphaBitMask;          // mask for alpha channel
        unsigned int pvrtcTag;              // magic number identifying pvrtc file
        unsigned int surfaceCount;          // number of surfaces present in the pvrtc
    };

    // Read the file header.
    unsigned int size = sizeof(pvrtc_file_header_legacy);
    pvrtc_file_header_legacy header;
    unsigned int read = (int)fread(&header, 1, size, file);
    if (read != size)
    {
        GP_ERROR("Failed to read file header for pvrtc file '%s'.", path);
        if (fclose(file) != 0)
        {
            GP_ERROR("Failed to close file '%s'.", path);
        }
        return NULL;
    }

    // Proper file header identifier.
    if (PVRTCIdentifier[0] != (char)((header.pvrtcTag >>  0) & 0xff) ||
        PVRTCIdentifier[1] != (char)((header.pvrtcTag >>  8) & 0xff) ||
        PVRTCIdentifier[2] != (char)((header.pvrtcTag >> 16) & 0xff) ||
        PVRTCIdentifier[3] != (char)((header.pvrtcTag >> 24) & 0xff))
     {
        GP_ERROR("Failed to load pvrtc file '%s': invalid header.", path);
        if (fclose(file) != 0)
        {
            GP_ERROR("Failed to close file '%s'.", path);
        }
        return NULL;
    }

    // Format flags for GLenum format.
    if (header.bpp == 4)
    {
        *format = header.alphaBitMask ? GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
    }
    else if (header.bpp == 2)
    {
        *format = header.alphaBitMask ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
    }
    else
    {
        GP_ERROR("Failed to load pvrtc file '%s': invalid pvrtc compressed texture format flags.", path);
        if (fclose(file) != 0)
        {
            GP_ERROR("Failed to close file '%s'.", path);
        }
        return NULL;
    }

    *width = (GLsizei)header.width;
    *height = (GLsizei)header.height;
    *mipMapCount = header.mipmapCount + 1; // +1 because mipmapCount does not include the base level

    GLubyte* data = new GLubyte[header.dataSize];
    read = (int)fread(data, 1, header.dataSize, file);
    if (read != header.dataSize)
    {
        GP_ERROR("Failed to load texture data for pvrtc file '%s'.", path);
        if (fclose(file) != 0)
        {
            GP_ERROR("Failed to close file '%s'.", path);
        }
        SAFE_DELETE_ARRAY(data);
        return NULL;
    }

    return data;
}

Texture* Texture::createCompressedDDS(const char* path)
{
    GP_ASSERT(path);

    // DDS file structures.
    struct dds_pixel_format
    {
        unsigned int dwSize;
        unsigned int dwFlags;
        unsigned int dwFourCC;
        unsigned int dwRGBBitCount;
        unsigned int dwRBitMask;
        unsigned int dwGBitMask;
        unsigned int dwBBitMask;
        unsigned int dwABitMask;
    };

    struct dds_header
    {
        unsigned int     dwSize;
        unsigned int     dwFlags;
        unsigned int     dwHeight;
        unsigned int     dwWidth;
        unsigned int     dwPitchOrLinearSize;
        unsigned int     dwDepth;
        unsigned int     dwMipMapCount;
        unsigned int     dwReserved1[11];
        dds_pixel_format ddspf;
        unsigned int     dwCaps;
        unsigned int     dwCaps2;
        unsigned int     dwCaps3;
        unsigned int     dwCaps4;
        unsigned int     dwReserved2;
    };

    struct dds_mip_level
    {
        GLubyte* data;
        GLsizei width;
        GLsizei height;
        GLsizei size;
    };

    Texture* texture = NULL;

    // Read DDS file.
    FILE* fp = FileSystem::openFile(path, "rb");
    if (fp == NULL)
    {
        GP_ERROR("Failed to open file '%s'.", path);
        return NULL;
    }

    // Validate DDS magic number.
    char code[4];
    if (fread(code, 1, 4, fp) != 4 || strncmp(code, "DDS ", 4) != 0)
    {
        GP_ERROR("Failed to read DDS file '%s': invalid DDS magic number.", path);
        if (fclose(fp) != 0)
        {
            GP_ERROR("Failed to close file '%s'.", path);
        }
        return NULL;
    }

    // Read DDS header.
    dds_header header;
    if (fread(&header, sizeof(dds_header), 1, fp) != 1)
    {
        GP_ERROR("Failed to read header for DDS file '%s'.", path);
        if (fclose(fp) != 0)
        {
            GP_ERROR("Failed to close file '%s'.", path);
        }
        return NULL;
    }

    if ((header.dwFlags & 0x20000/*DDSD_MIPMAPCOUNT*/) == 0)
    {
        // Mipmap count not specified (non-mipmapped texture).
        header.dwMipMapCount = 1;
    }

    // Allocate mip level structures.
    dds_mip_level* mipLevels = new dds_mip_level[header.dwMipMapCount];
    memset(mipLevels, 0, sizeof(dds_mip_level) * header.dwMipMapCount);

    GLenum format, internalFormat;
    bool compressed = false;
    GLsizei width = header.dwWidth;
    GLsizei height = header.dwHeight;
    int bytesPerBlock;

    if (header.ddspf.dwFlags & 0x4/*DDPF_FOURCC*/)
    {
        compressed = true;

        // Compressed.
        switch (header.ddspf.dwFourCC)
        {
        case ('D'|('X'<<8)|('T'<<16)|('1'<<24)):
            format = internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            bytesPerBlock = 8;
            break;
        case ('D'|('X'<<8)|('T'<<16)|('3'<<24)):
            format = internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            bytesPerBlock = 16;
            break;
        case ('D'|('X'<<8)|('T'<<16)|('5'<<24)):
            format = internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            bytesPerBlock = 16;
            break;
        case ('A'|('T'<<8)|('C'<<16)|(' '<<24)):
            format = internalFormat = ATC_RGB_AMD;
            bytesPerBlock = 8;
            break;
        case ('A'|('T'<<8)|('C'<<16)|('A'<<24)):
            format = internalFormat = ATC_RGBA_EXPLICIT_ALPHA_AMD;
            bytesPerBlock = 16;
            break;
        case ('A'|('T'<<8)|('C'<<16)|('I'<<24)):
            format = internalFormat = ATC_RGBA_INTERPOLATED_ALPHA_AMD;
            bytesPerBlock = 16;
            break;
        default:
            GP_ERROR("Unsupported compressed texture format (%d) for DDS file '%s'.", header.ddspf.dwFourCC, path);
            if (fclose(fp) != 0)
            {
                GP_ERROR("Failed to close file '%s'.", path);
            }
            SAFE_DELETE_ARRAY(mipLevels);
            return NULL;
        }

        for (unsigned int i = 0; i < header.dwMipMapCount; ++i)
        {
            mipLevels[i].width = width;
            mipLevels[i].height = height;
            mipLevels[i].size =  std::max(1, (width+3) >> 2) * std::max(1, (height+3) >> 2) * bytesPerBlock;
            mipLevels[i].data = new GLubyte[mipLevels[i].size];

            if (fread(mipLevels[i].data, 1, mipLevels[i].size, fp) != (unsigned int)mipLevels[i].size)
            {
                GP_ERROR("Failed to load dds compressed texture bytes for texture: %s", path);
                
                // Cleanup mip data.
                for (unsigned int i = 0; i < header.dwMipMapCount; ++i)
                    SAFE_DELETE_ARRAY(mipLevels[i].data);
                SAFE_DELETE_ARRAY(mipLevels);

                if (fclose(fp) != 0)
                {
                    GP_ERROR("Failed to close file '%s'.", path);
                }
                return texture;
            }

            width  = std::max(1, width >> 1);
            height = std::max(1, height >> 1);
        }
    }
    else if (header.ddspf.dwFlags == 0x40/*DDPF_RGB*/)
    {
        // RGB (uncompressed)
        // Note: Use GL_BGR as internal format to flip bytes.
        GP_ERROR("Failed to create texture from DDS file '%s': uncompressed RGB format is not supported.", path);
        if (fclose(fp) != 0)
        {
            GP_ERROR("Failed to close file '%s'.", path);
        }
        SAFE_DELETE_ARRAY(mipLevels);
        return NULL;
    }
    else if (header.ddspf.dwFlags == 0x41/*DDPF_RGB|DDPF_ALPHAPIXELS*/)
    {
        // RGBA (uncompressed)
        // Note: Use GL_BGRA as internal format to flip bytes.
        GP_ERROR("Failed to create texture from DDS file '%s': uncompressed RGBA format is not supported.", path);
        if (fclose(fp) != 0)
        {
            GP_ERROR("Failed to close file '%s'.", path);
        }
        SAFE_DELETE_ARRAY(mipLevels);
        return NULL;
    }
    else
    {
        // Unsupported.
        GP_ERROR("Failed to create texture from DDS file '%s': unsupported flags (%d).", path, header.ddspf.dwFlags);
        if (fclose(fp) != 0)
        {
            GP_ERROR("Failed to close file '%s'.", path);
        }
        SAFE_DELETE_ARRAY(mipLevels);
        return NULL;
    }
    
    // Close file.
    if (fclose(fp) != 0)
    {
        GP_ERROR("Failed to close file '%s'.", path);
    }

    // Generate GL texture.
    GLuint textureId;
    GL_ASSERT( glGenTextures(1, &textureId) );
    GL_ASSERT( glBindTexture(GL_TEXTURE_2D, textureId) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, header.dwMipMapCount > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR ) );

    // Create gameplay texture.
    texture = new Texture();
    texture->_handle = textureId;
    texture->_width = header.dwWidth;
    texture->_height = header.dwHeight;
    texture->_compressed = compressed;
    texture->_mipmapped = header.dwMipMapCount > 1;

    // Load texture data.
    for (unsigned int i = 0; i < header.dwMipMapCount; ++i)
    {
        if (compressed)
        {
            GL_ASSERT( glCompressedTexImage2D(GL_TEXTURE_2D, i, format, mipLevels[i].width, mipLevels[i].height, 0, mipLevels[i].size, mipLevels[i].data) );
        }
        else
        {
            // TODO: For uncompressed formats, set GL_UNPACK_ALIGNMENT based on stride
            GL_ASSERT( glTexImage2D(GL_TEXTURE_2D, i, internalFormat, mipLevels[i].width, mipLevels[i].height, 0, format, GL_UNSIGNED_INT, mipLevels[i].data) );
        }

        // Clean up the texture data.
        SAFE_DELETE_ARRAY(mipLevels[i].data);
    }

    // Clean up mip levels structure.
    SAFE_DELETE_ARRAY(mipLevels);

    return texture;
}

Texture::Format Texture::getFormat() const
{
    return _format;
}

unsigned int Texture::getWidth() const
{
    return _width;
}

unsigned int Texture::getHeight() const
{
    return _height;
}

TextureHandle Texture::getHandle() const
{
    return _handle;
}

void Texture::setWrapMode(Wrap wrapS, Wrap wrapT)
{
    GL_ASSERT( glBindTexture(GL_TEXTURE_2D, _handle) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)wrapS) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)wrapT) );
}

void Texture::setFilterMode(Filter minificationFilter, Filter magnificationFilter)
{
    GL_ASSERT( glBindTexture(GL_TEXTURE_2D, _handle) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)minificationFilter) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)magnificationFilter) );
}

void Texture::generateMipmaps()
{
    if (!_mipmapped)
    {
        GL_ASSERT( glBindTexture(GL_TEXTURE_2D, _handle) );
        GL_ASSERT( glGenerateMipmap(GL_TEXTURE_2D) );

        _mipmapped = true;
    }
}

bool Texture::isMipmapped() const
{
    return _mipmapped;
}

bool Texture::isCompressed() const
{
    return _compressed;
}

Texture::Sampler::Sampler(Texture* texture)
    : _texture(texture), _wrapS(Texture::REPEAT), _wrapT(Texture::REPEAT), _magFilter(Texture::LINEAR)
{
    GP_ASSERT(texture);
    _minFilter = texture->isMipmapped() ? Texture::NEAREST_MIPMAP_LINEAR : Texture::LINEAR;
}

Texture::Sampler::~Sampler()
{
    SAFE_RELEASE(_texture);
}

Texture::Sampler* Texture::Sampler::create(Texture* texture)
{
    GP_ASSERT(texture);
    texture->addRef();
    return new Sampler(texture);
}

Texture::Sampler* Texture::Sampler::create(const char* path, bool generateMipmaps)
{
    Texture* texture = Texture::create(path, generateMipmaps);
    return texture ? new Sampler(texture) : NULL;
}

void Texture::Sampler::setWrapMode(Wrap wrapS, Wrap wrapT)
{
    _wrapS = wrapS;
    _wrapT = wrapT;
}

void Texture::Sampler::setFilterMode(Filter minificationFilter, Filter magnificationFilter)
{
    _minFilter = minificationFilter;
    _magFilter = magnificationFilter;
}

Texture* Texture::Sampler::getTexture() const
{
    return _texture;
}

void Texture::Sampler::bind()
{
    GP_ASSERT(_texture);

    GL_ASSERT( glBindTexture(GL_TEXTURE_2D, _texture->_handle) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)_wrapS) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)_wrapT) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)_minFilter) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)_magFilter) );
}

}
