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
        ]
        for dep in deps:
            self.requires(dep)
