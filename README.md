# orc-server-storage
WIP: Small client/server to store data in orc file

## Dependencies

We use [vcpkg](https://github.com/Microsoft/vcpkg) to manage dependencies

Stryke depend on:
* [Apache-Orc](https://orc.apache.org/)
* [uWebsockets](https://github.com/uNetworking/uWebSockets) (v0.14)
* [nlohmann/json](https://github.com/nlohmann/json)
* [Catch2](https://github.com/catchorg/Catch2)

```
./vcpkg install orc uwebsockets nlohmann-json catch2 openssl
```

### Compile

There is 1 options :
* BUILD_UNIT_TESTS (default: OFF)

```bash
mkdir build
cd build
# configure make with vcpkg toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake -DBUILD_UNIT_TESTS=ON
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

