from pathlib import Path
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.build import can_run


class libxdaq(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    build_policy = "missing"

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.25.0 <3.30.0]")

    def requirements(self):
        self.requires("catch2/3.5.0")
        self.requires("fmt/10.2.1")
        self.requires("nlohmann_json/3.11.3")
        self.requires(self.tested_reference_str)

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if can_run(self):
            cmake = CMake(self)
            cmake.test()
