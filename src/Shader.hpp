#pragma once

#include <string>

class Shader {
public:
  Shader(const fspath& vsPath, const fspath& fsPath, const fspath& gsPath = "");
  Shader(const fspath& compPath);

  static void setDirectoryLocation(const fspath& path);

  GLint getUniformLoc(const std::string& name) const;

  void use() const;

  void setUniform1f(const GLint& loc, const float& n)          const;
  void setUniform3f(const GLint& loc, const vec3& v)           const;
  void setUniform4f(const GLint& loc, const vec4& v)           const;
  void setUniform2i(const GLint& loc, const ivec2& v)          const;
  void setUniformMatrix4f(const GLint& loc, const mat4& m)     const;
  void setUniformTexture(const GLint& loc, const GLuint& unit) const;

  void setUniform1f(const std::string& name, const float& n)          const;
  void setUniform3f(const std::string& name, const vec3& v)           const;
  void setUniform4f(const std::string& name, const vec4& v)           const;
  void setUniform2i(const std::string& name, const ivec2& v)          const;
  void setUniformMatrix4f(const std::string& name, const mat4& m)     const;
  void setUniformTexture(const std::string& name, const GLuint& unit) const;

private:
  static fspath directory;
  GLuint program;

private:
  static GLuint load(fspath path, int type);
  static GLuint compile(const fspath& path, int type);
  static void link(GLuint program);
};
