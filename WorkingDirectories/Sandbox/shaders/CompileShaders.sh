mkdir -p ./compiled
$VULKAN_SDK/Bin/glslc.exe shader.vert -o ./compiled/shader.vert.spv
$VULKAN_SDK/Bin/glslc.exe shader.frag -o ./compiled/shader.frag.spv

echo ""
read -p "Finished. Please check if there are any compilation errors and press Enter to close this window."