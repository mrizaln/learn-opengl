from conan import ConanFile


class Recipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps", "VirtualRunEnv"

    def layout(self):
        self.folders.generators = "conan"

    def requirements(self):
        deps = [
            "glfw/3.3.8",
            "glbinding/3.3.0",
            "stb/cci.20230920",
            "glm/0.9.9.8",
            "imgui/1.90",
            "assimp/5.3.1",
        ]

        for dep in deps:
            self.requires(dep)
