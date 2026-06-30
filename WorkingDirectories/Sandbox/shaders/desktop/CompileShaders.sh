mkdir -p ./compiled

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    GLSLC="$VULKAN_SDK/Bin/glslc.exe"
else
    GLSLC="glslc"
fi

$GLSLC shader.vert -o ./compiled/shader.vert.spv
$GLSLC shader.frag -o ./compiled/shader.frag.spv
$GLSLC presentShader.vert -o ./compiled/presentShader.vert.spv
$GLSLC presentShader.frag -o ./compiled/presentShader.frag.spv
$GLSLC uvShader.vert -o ./compiled/uvShader.vert.spv
$GLSLC uvShader.frag -o ./compiled/uvShader.frag.spv

echo ""
read -p "Finished. Please check if there are any compilation errors and press Enter to close this window."
