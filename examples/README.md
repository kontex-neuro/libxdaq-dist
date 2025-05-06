## Build example

```bash
conan install . --build=missing -s build_type=Release
cmake -S . -B build/Release --preset conan-release
cmake --build build/Release --preset conan-release
```

## Run example

### set_ttl_out
```bash
cd build/Release
./set_ttl_out 0 rhd
./set_ttl_out 0 rhs
```