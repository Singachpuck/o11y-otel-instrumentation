from conan import ConanFile
from conan.tools.cmake import cmake_layout


class ContextPropagationApp(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    _otel_package_name = "opentelemetry-cpp"
    _otel_version = "1.14.2"

    @property
    def _otel_package(self): return "{}/{}".format(self._otel_package_name, self._otel_version)

    _boost_package_name = "boost"
    _boost_version = "1.85.0"

    @property
    def _boost_package(self): return "{}/{}".format(self._boost_package_name, self._boost_version)

    _nlohmann_package_name = "nlohmann_json"
    _nlohmann_version = "3.11.3"

    @property
    def _nlohmann_package(self): return "{}/{}".format(self._nlohmann_package_name, self._nlohmann_version)

    _restbed_package_name = "restbed"
    _restbed_version = "4.8"

    @property
    def _restbed_package(self): return "{}/{}".format(self._restbed_package_name, self._restbed_version)

    _fmt_package_name = "fmt"
    _fmt_version = "11.0.2"

    @property
    def _fmt_package(self): return "{}/{}".format(self._fmt_package_name, self._fmt_version)

    _yaml_package_name = "yaml-cpp"
    _yaml_version = "0.8.0"

    @property
    def _yaml_package(self): return "{}/{}".format(self._yaml_package_name, self._yaml_version)

    _cxxopts_package_name = "cxxopts"
    _cxxopts_version = "3.2.0"

    @property
    def _cxxopts_package(self): return "{}/{}".format(self._cxxopts_package_name, self._cxxopts_version)

    _cmake_package_name = "cmake"
    _cmake_version = "3.30.0"

    @property
    def _cmake_package(self): return "{}/{}".format(self._cmake_package_name, self._cmake_version)

    _grpc_package_name = "grpc"
    _grpc_version = "1.46.7.9"
    _grpc_plugins_package_name = "grpc_plugins"

    @property
    def _grpc_plugins_package(self):  return "{}/{}".format(self._grpc_plugins_package_name, self._grpc_version)

    _protobuf_package_name = "protobuf"
    _protobuf_version = "3.19.6.1"
    _protoc_package_name = "protoc"

    @property
    def _protoc_package(self):  return "{}/{}".format(self._protoc_package_name, self._protobuf_version)


    def requirements(self):
        self.requires(self._otel_package)
        self.requires(self._fmt_package)
        self.requires(self._nlohmann_package)
        self.requires(self._boost_package)
        self.requires(self._restbed_package)
        self.requires(self._cxxopts_package)
        self.requires(self._yaml_package)

    def build_requirements(self):
        self.build_requires(self._cmake_package)