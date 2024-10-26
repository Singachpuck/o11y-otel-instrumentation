# OpenTelemetry instrumentation app 

## Agenda:

1. Project structure
    - User Manager Service (provides gRPC interface)
    - Gateway service (calls User Manager interface using gRPC, exposes REST Api)
    - User.proto
    - Observability config for each service.
    - OTEL Collector config to accept otlp/grpc and otlp/http protocols
2. Logging
    - Boost log library
    - Logging locally
    - File logging
    - 3 formats supported (text, json, json-formatted)
    - Possible to add events to active span
3. Tracing
    - Showcase parent-child relationship
    - Showcase context propagation
    - Local exporting (for debug)
    - Export to OTLP endpoint (Collector)
4. Metrics
    - Count total number of request to the service
    - Local exporting (for debug)
    - Export to OTLP endpoint (Collector)
    - Configurable interval

## Build guide

The application requires to have CMake with Conan installed and the venv activated.

On Linux make sure to have a gcc compiler ready and adapt Conan Profiles in `./config/conan_profiles`.

On Windows make sure to have *Visual Studio 2022* build tools installed.

If everything is ready to go, then in the project root run:

```
make -I config/make
```

Then to run *user-manager-service*:
```
make -I config/make run-user-manager
```

And to run *gateway-service*:
```
make -I config/make run-gateway
```