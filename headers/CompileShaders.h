#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
    std::string GeometrySource;
};

//static ShaderProgramSource ParseShader(const std::string& filepath) {
//    std::ifstream stream(filepath);
//
//    enum class ShaderType {
//        NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
//    };
//
//    std::string line;
//    std::stringstream ss[3];
//    ShaderType type = ShaderType::NONE;
//    while (getline(stream, line)) {
//        if (line.find("#shader") != std::string::npos) {
//            if (line.find("vertex") != std::string::npos) {
//                type = ShaderType::VERTEX;
//            }
//            else if (line.find("fragment") != std::string::npos) {
//                type = ShaderType::FRAGMENT;
//            }
//            else if (line.find("geometry") != std::string::npos) {
//                type = ShaderType::GEOMETRY;
//            }
//        }
//        else {
//            ss[(int)type] << line << '\n';
//        }
//    }
//
//    return { ss[0].str(), ss[1].str(), ss[2].str() };
//}

static ShaderProgramSource ParseShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    ShaderProgramSource source;
    std::ifstream vertexFile(vertexShaderPath);
    std::ifstream fragmentFile(fragmentShaderPath);

    std::stringstream vertexStream, fragmentStream;

    if (!vertexFile.is_open() || !fragmentFile.is_open()) {
        std::cerr << "Failed to open shader files!" << std::endl;
        return source;
    }

    vertexStream << vertexFile.rdbuf();
    fragmentStream << fragmentFile.rdbuf();

    source.VertexSource = vertexStream.str();
    source.FragmentSource = fragmentStream.str();

    return source;
}

//static unsigned int CompileShader(unsigned int type, const std::string& source) {
//    unsigned int id = glCreateShader(type);
//    const char* src = source.c_str();
//    glShaderSource(id, 1, &src, nullptr);
//    glCompileShader(id);
//
//    int result;
//    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
//    if (result == GL_FALSE) {
//        int length;
//        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
//        char* message = (char*)alloca(length * sizeof(char));
//        glGetShaderInfoLog(id, length, &length, message);
//        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? type == GL_FRAGMENT_SHADER ?
//            "vertex" : "fragment" : "geometry") << " shader" << std::endl;
//        std::cout << message << std::endl;
//        glDeleteShader(id);
//        return 0;
//    }
//
//    return id;
//}
//
//static unsigned int CreateShader(const std::string& VertexShader, const std::string& FragmentShader,
//    const std::string& GeometryShader) {
//    unsigned int program = glCreateProgram();
//    unsigned int vs = CompileShader(GL_VERTEX_SHADER, VertexShader);
//    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);
//    unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, GeometryShader);
//
//    glAttachShader(program, vs);
//    glAttachShader(program, fs);
//    glAttachShader(program, gs);
//
//    glLinkProgram(program);
//    glValidateProgram(program);
//
//    glDeleteShader(vs);
//    glDeleteShader(fs);
//    glDeleteShader(gs);
//
//    return program;
//}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// Function to compile and link a shader program
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
