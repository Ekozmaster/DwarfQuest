#include <string>
#include <fstream>
#include <sstream>

#include <src/ResourceManagement/ShaderLoader.h>
#include <src/Utils/Logger.h>

namespace DwarfQuest {
    namespace Core {

        Shader* LoadShader(const char* shaderPath) {
            std::string vCode;
            std::string fCode;
            try {
                std::ifstream stream(shaderPath);
                std::string line;
                // 0 = vertex's source, 1 = fragment's source.
                std::stringstream sourceStreams[2];
                unsigned int sourceIndex = 0;
                while (std::getline(stream, line)) {
                    if (line.find("#VERTEX") != std::string::npos) {
                        sourceIndex = 0;
                    } else if (line.find("#FRAGMENT") != std::string::npos) {
                        sourceIndex = 1;
                    } else {
                        sourceStreams[sourceIndex] << line << "\n";
                    }
                }

                vCode = sourceStreams[0].str();
                fCode = sourceStreams[1].str();
            } catch (const std::exception & e) {
                Logger::Error("ShaderLoader.LoadShader - Got exception while reading the file: '" + std::string(shaderPath) + "':\n" + e.what());
                return NULL;
            }

            const char* vertexSource = vCode.c_str();
            const char* fragmentSource = fCode.c_str();

            Shader* shader = new Shader();
            bool compiledSuccessfully = shader->CompileShaders(shaderPath, vertexSource, fragmentSource);
            if (compiledSuccessfully) return shader;
            else delete shader;

            return NULL;
        }

    }
}