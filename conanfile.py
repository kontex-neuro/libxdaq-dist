from pathlib import Path
from conan import ConanFile
from conan.tools.files import get, copy


class libxdaq(ConanFile):
    name = "libxdaq"
    version = "0.2.8"
    settings = "os", "compiler", "build_type", "arch"
    build_policy = "missing"

    def build_requirements(self):
        self.requires("catch2/3.5.0")
        self.tool_requires("cmake/[>=3.25.0 <3.30.0]")

    def requirements(self):
        self.requires("boost/1.81.0")
        self.requires("zlib/1.3.1")
        self.requires("fmt/10.2.1")
        self.requires("nlohmann_json/3.11.3")
        self.requires("spdlog/1.13.0")

    def configure(self):
        # Enable required Boost modules
        self.options["boost/*"].with_atomic = True
        self.options["boost/*"].with_system = True
        self.options["boost/*"].with_filesystem = True
        self.options["boost/*"].with_program_options = True
        # Configure Boost options for GitHub Actions build
        self.options["boost/*"].with_stacktrace_backtrace = False
        self.options["boost/*"].without_stacktrace = True
        self.options["boost/*"].without_locale = True
        # List of Boost modules to disable for faster build speed
        disable_for_build_speed = (
            "charconv", "chrono", "cobalt", "container", "context", "contract", "coroutine",
            "date_time", "exception", "fiber", "graph", "graph_parallel", "iostreams", "json",
            "log", "math", "mpi", "nowide", "python", "random", "regex", "serialization", "test",
            "thread", "timer", "type_erasure", "url", "wave"
        )
        for opt in disable_for_build_speed:
            setattr(self.options["boost/*"], f"without_{opt}", True)

    def build(self):
        base_url = "https://github.com/kontex-neuro/libxdaq-dist/releases/download"

        _os = str(self.settings.os).lower()
        _arch = str(self.settings.arch).lower()
        url = f"{base_url}/{self.version}/{_os}-{_arch}.zip"
        get(self, url, strip_root=True)

    def package(self):
        local_bin_folder = Path(self.build_folder, "bin")
        local_include_folder = Path(self.build_folder, "include")
        local_lib_folder = Path(self.build_folder, "lib")
        copy(self, "*", local_bin_folder, Path(self.package_folder, "bin"))
        copy(self, "*", local_include_folder, Path(self.package_folder, "include"))
        copy(self, "*", local_lib_folder, Path(self.package_folder, "lib"))

    def package_info(self):
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.libs = ["xdaq_device"]
        self.cpp_info.includedirs = ["include", "include/xdaq"]
        self.cpp_info.libdirs = ["lib/xdaq", "lib/xdaq/devices"]
        self.cpp_info.builddirs.append(str(Path("lib", "cmake", "xdaq")))
        self.cpp_info.builddirs.append(str(Path("lib", "cmake", "nontype_functional")))
        self.cpp_info.builddirs.append(str(Path("lib", "cmake", "readerwriterqueue")))
