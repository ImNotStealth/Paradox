cd ..
cmake -B build-vita -DCMAKE_TOOLCHAIN_FILE=scripts/psvita.toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build build-vita
echo ""
read -p "Finished. Please check if there are any errors and press Enter to close this window."
