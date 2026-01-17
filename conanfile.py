from conan import ConanFile
from conan.tools.cmake import cmake_layout


class Recipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps", "VirtualRunEnv"
    requires = [
        "glbinding/3.5.0",
        "glm/1.0.1",
        "imgui/1.92.2b-docking",
        "stb/cci.20240531",
        "spdlog/1.17.0",
    ]

    default_options = {"spdlog/*:use_std_fmt": True}

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        if self.settings.os == "Windows":
            self.requires("glfw/3.4")  # linux: use system
            self.requires("khrplatform/cci.20200529")
            self.requires("assimp/6.0.2")   # linux: use system
