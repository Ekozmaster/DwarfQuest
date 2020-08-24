#pragma once

#include <src/Graphics/OpenGL/Shader.h>

namespace DwarfQuest {
    namespace Core {
        // ### Scalars
        // bool
        // int
        // uint
        // float
        // double

        // ### Vectors
        // bvecn
        // ivecn
        // uvecn
        // vecn
        // dvecn

        // ### Matrices
        // matnxm
        // matn
        // dmatnxm
        // dmatn

        // ### Opaque
        // sampler2D

        class Material {
            Shader* shader;

        public:
            // Don't use these methods directly unless you know exactly what you're doing.
            int Use();
        };
    }
}
