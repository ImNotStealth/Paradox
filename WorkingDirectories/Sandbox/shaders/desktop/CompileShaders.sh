mkdir -p ./compiled

GLSLC="${VULKAN_SDK:-glslc}"

$GLSLC shader.vert -o ./compiled/shader.vert.spv
$GLSLC shader.frag -o ./compiled/shader.frag.spv

echo ""
read -p "Finished. Please check if there are any compilation errors and press Enter to close this window."
