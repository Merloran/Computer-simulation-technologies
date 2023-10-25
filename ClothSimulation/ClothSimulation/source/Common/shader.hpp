#pragma once

enum class EShaderType : Int8
{
    None,
    Vertex,
    Geometry,
    Fragment,
    Compute
};

class Shader
{
public:
    // Read shaders from disk and create them
    void create(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");
    void create(const std::string& computePath);
    void reload();

    void use();

    // Setters for uniforms
    void set_bool (const std::string& name, bool value);
    void set_int  (const std::string& name, Int32 value);
    void set_float(const std::string& name, Float32 value);
    void set_vec2 (const std::string& name, Float32 x, Float32 y);
    void set_vec2 (const std::string& name, const glm::vec2& vector);
    void set_vec3 (const std::string& name, Float32 x, Float32 y, Float32 z);
    void set_vec3 (const std::string& name, const glm::vec3& vector);
    void set_vec4 (const std::string& name, Float32 x, Float32 y, Float32 z, Float32 w);
    void set_vec4 (const std::string& name, const glm::vec4& vector);
    void set_mat4 (const std::string& name, const glm::mat4& value);
    void set_block(const std::string& name, UInt32 number);

    static void s_bind_uniform_buffer(UInt32 uniformBufferObject, UInt32 offset, UInt32 size, Float32* data);
    void shutdown();

private:
    std::string vertexPath, geometryPath, fragmentPath, computePath;
    UInt32 id = 0U;
    void check_compile_errors(UInt32 shaderId, EShaderType shaderType);

    static inline UInt32 sActiveShaderId = 0U;
};

