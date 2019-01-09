# Stryke Server
Small client/server to store data in [Orc](https://orc.apache.org/) file
[Stryke Server](https://github.com/edmBernard/stryke-server) depend on [Stryke](https://github.com/edmBernard/stryke). Stryke is include as a submodule.

## Dependencies

We use [vcpkg](https://github.com/Microsoft/vcpkg) to manage dependencies

stryke-server depend on:
* [Apache-Orc](https://orc.apache.org/)
* [uWebsockets](https://github.com/uNetworking/uWebSockets) (v0.14)
* [nlohmann/json](https://github.com/nlohmann/json)
* [Catch2](https://github.com/catchorg/Catch2)
* [Cpr](https://github.com/whoshuu/cpr)

```
./vcpkg install orc uwebsockets nlohmann-json catch2 openssl cpr
```

### Compile

```bash
mkdir build
cd build
# configure make with vcpkg toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake
make
```

### Run Tests

```bash
make test
```

### Generate Doxygen documentation

You need to have doxygen and graphviz installed on your computer.

```bash
make docs
```

