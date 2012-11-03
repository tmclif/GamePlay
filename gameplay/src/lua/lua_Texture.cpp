#include "Base.h"
#include "ScriptController.h"
#include "lua_Texture.h"
#include "Base.h"
#include "FileSystem.h"
#include "Game.h"
#include "Image.h"
#include "Ref.h"
#include "Texture.h"
#include "lua_TextureFilter.h"
#include "lua_TextureFormat.h"
#include "lua_TextureWrap.h"

namespace gameplay
{

void luaRegister_Texture()
{
    const luaL_Reg lua_members[] = 
    {
        {"addRef", lua_Texture_addRef},
        {"generateMipmaps", lua_Texture_generateMipmaps},
        {"getFormat", lua_Texture_getFormat},
        {"getHandle", lua_Texture_getHandle},
        {"getHeight", lua_Texture_getHeight},
        {"getRefCount", lua_Texture_getRefCount},
        {"getWidth", lua_Texture_getWidth},
        {"isCompressed", lua_Texture_isCompressed},
        {"isMipmapped", lua_Texture_isMipmapped},
        {"release", lua_Texture_release},
        {"setFilterMode", lua_Texture_setFilterMode},
        {"setWrapMode", lua_Texture_setWrapMode},
        {NULL, NULL}
    };
    const luaL_Reg lua_statics[] = 
    {
        {"create", lua_Texture_static_create},
        {NULL, NULL}
    };
    std::vector<std::string> scopePath;

    ScriptUtil::registerClass("Texture", lua_members, NULL, lua_Texture__gc, lua_statics, scopePath);
}

static Texture* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "Texture");
    luaL_argcheck(state, userdata != NULL, 1, "'Texture' expected.");
    return (Texture*)((ScriptUtil::LuaObject*)userdata)->instance;
}

int lua_Texture__gc(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                void* userdata = luaL_checkudata(state, 1, "Texture");
                luaL_argcheck(state, userdata != NULL, 1, "'Texture' expected.");
                ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)userdata;
                if (object->owns)
                {
                    Texture* instance = (Texture*)object->instance;
                    SAFE_RELEASE(instance);
                }
                
                return 0;
            }

            lua_pushstring(state, "lua_Texture__gc - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_addRef(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                instance->addRef();
                
                return 0;
            }

            lua_pushstring(state, "lua_Texture_addRef - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_generateMipmaps(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                instance->generateMipmaps();
                
                return 0;
            }

            lua_pushstring(state, "lua_Texture_generateMipmaps - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_getFormat(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                Texture::Format result = instance->getFormat();

                // Push the return value onto the stack.
                lua_pushstring(state, lua_stringFromEnum_TextureFormat(result));

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getFormat - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_getHandle(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                void* returnPtr = (void*)new GLuint(instance->getHandle());
                if (returnPtr)
                {
                    ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                    object->instance = returnPtr;
                    object->owns = true;
                    luaL_getmetatable(state, "GLuint");
                    lua_setmetatable(state, -2);
                }
                else
                {
                    lua_pushnil(state);
                }

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getHandle - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_getHeight(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                unsigned int result = instance->getHeight();

                // Push the return value onto the stack.
                lua_pushunsigned(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getHeight - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_getRefCount(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                unsigned int result = instance->getRefCount();

                // Push the return value onto the stack.
                lua_pushunsigned(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getRefCount - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_getWidth(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                unsigned int result = instance->getWidth();

                // Push the return value onto the stack.
                lua_pushunsigned(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getWidth - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_isCompressed(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                bool result = instance->isCompressed();

                // Push the return value onto the stack.
                lua_pushboolean(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_isCompressed - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_isMipmapped(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                bool result = instance->isMipmapped();

                // Push the return value onto the stack.
                lua_pushboolean(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_isMipmapped - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_release(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                instance->release();
                
                return 0;
            }

            lua_pushstring(state, "lua_Texture_release - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_setFilterMode(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 3:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TSTRING || lua_type(state, 2) == LUA_TNIL) &&
                (lua_type(state, 3) == LUA_TSTRING || lua_type(state, 3) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                Texture::Filter param1 = (Texture::Filter)lua_enumFromString_TextureFilter(luaL_checkstring(state, 2));

                // Get parameter 2 off the stack.
                Texture::Filter param2 = (Texture::Filter)lua_enumFromString_TextureFilter(luaL_checkstring(state, 3));

                Texture* instance = getInstance(state);
                instance->setFilterMode(param1, param2);
                
                return 0;
            }

            lua_pushstring(state, "lua_Texture_setFilterMode - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_setWrapMode(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 3:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TSTRING || lua_type(state, 2) == LUA_TNIL) &&
                (lua_type(state, 3) == LUA_TSTRING || lua_type(state, 3) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                Texture::Wrap param1 = (Texture::Wrap)lua_enumFromString_TextureWrap(luaL_checkstring(state, 2));

                // Get parameter 2 off the stack.
                Texture::Wrap param2 = (Texture::Wrap)lua_enumFromString_TextureWrap(luaL_checkstring(state, 3));

                Texture* instance = getInstance(state);
                instance->setWrapMode(param1, param2);
                
                return 0;
            }

            lua_pushstring(state, "lua_Texture_setWrapMode - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 3).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

int lua_Texture_static_create(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TSTRING || lua_type(state, 1) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    ScriptUtil::LuaArray<const char> param1 = ScriptUtil::getString(1, false);

                    void* returnPtr = (void*)Texture::create(param1);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TTABLE || lua_type(state, 1) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Image> param1 = ScriptUtil::getObjectPointer<Image>(1, "Image", false, &param1Valid);
                    if (!param1Valid)
                        break;

                    void* returnPtr = (void*)Texture::create(param1);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Texture_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TSTRING || lua_type(state, 1) == LUA_TNIL) &&
                    lua_type(state, 2) == LUA_TBOOLEAN)
                {
                    // Get parameter 1 off the stack.
                    ScriptUtil::LuaArray<const char> param1 = ScriptUtil::getString(1, false);

                    // Get parameter 2 off the stack.
                    bool param2 = ScriptUtil::luaCheckBool(state, 2);

                    void* returnPtr = (void*)Texture::create(param1, param2);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TTABLE || lua_type(state, 1) == LUA_TNIL) &&
                    lua_type(state, 2) == LUA_TBOOLEAN)
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<Image> param1 = ScriptUtil::getObjectPointer<Image>(1, "Image", false, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    bool param2 = ScriptUtil::luaCheckBool(state, 2);

                    void* returnPtr = (void*)Texture::create(param1, param2);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Texture_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 3:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TNIL) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<GLuint> param1 = ScriptUtil::getObjectPointer<GLuint>(1, "GLuint", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    int param2 = (int)luaL_checkint(state, 2);

                    // Get parameter 3 off the stack.
                    int param3 = (int)luaL_checkint(state, 3);

                    void* returnPtr = (void*)Texture::create(*param1, param2, param3);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Texture_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 4:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TSTRING || lua_type(state, 1) == LUA_TNIL) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    (lua_type(state, 4) == LUA_TTABLE || lua_type(state, 4) == LUA_TLIGHTUSERDATA))
                {
                    // Get parameter 1 off the stack.
                    Texture::Format param1 = (Texture::Format)lua_enumFromString_TextureFormat(luaL_checkstring(state, 1));

                    // Get parameter 2 off the stack.
                    unsigned int param2 = (unsigned int)luaL_checkunsigned(state, 2);

                    // Get parameter 3 off the stack.
                    unsigned int param3 = (unsigned int)luaL_checkunsigned(state, 3);

                    // Get parameter 4 off the stack.
                    ScriptUtil::LuaArray<unsigned char> param4 = ScriptUtil::getUnsignedCharPointer(4);

                    void* returnPtr = (void*)Texture::create(param1, param2, param3, param4);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TNIL) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    (lua_type(state, 4) == LUA_TSTRING || lua_type(state, 4) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    ScriptUtil::LuaArray<GLuint> param1 = ScriptUtil::getObjectPointer<GLuint>(1, "GLuint", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    int param2 = (int)luaL_checkint(state, 2);

                    // Get parameter 3 off the stack.
                    int param3 = (int)luaL_checkint(state, 3);

                    // Get parameter 4 off the stack.
                    Texture::Format param4 = (Texture::Format)lua_enumFromString_TextureFormat(luaL_checkstring(state, 4));

                    void* returnPtr = (void*)Texture::create(*param1, param2, param3, param4);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Texture_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 5:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TSTRING || lua_type(state, 1) == LUA_TNIL) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    (lua_type(state, 4) == LUA_TTABLE || lua_type(state, 4) == LUA_TLIGHTUSERDATA) &&
                    lua_type(state, 5) == LUA_TBOOLEAN)
                {
                    // Get parameter 1 off the stack.
                    Texture::Format param1 = (Texture::Format)lua_enumFromString_TextureFormat(luaL_checkstring(state, 1));

                    // Get parameter 2 off the stack.
                    unsigned int param2 = (unsigned int)luaL_checkunsigned(state, 2);

                    // Get parameter 3 off the stack.
                    unsigned int param3 = (unsigned int)luaL_checkunsigned(state, 3);

                    // Get parameter 4 off the stack.
                    ScriptUtil::LuaArray<unsigned char> param4 = ScriptUtil::getUnsignedCharPointer(4);

                    // Get parameter 5 off the stack.
                    bool param5 = ScriptUtil::luaCheckBool(state, 5);

                    void* returnPtr = (void*)Texture::create(param1, param2, param3, param4, param5);
                    if (returnPtr)
                    {
                        ScriptUtil::LuaObject* object = (ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Texture_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1, 2, 3, 4 or 5).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

}
