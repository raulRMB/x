set sourceDir=.\assets\shaders
set validatorDir=C:\VulkanSDK\1.3.236.0\Bin

%validatorDir%\glslangValidator.exe -V %sourceDir%\shader.vert -o %sourceDir%\vert.spv
%validatorDir%\glslangValidator.exe -V %sourceDir%\shader.frag -o %sourceDir%\frag.spv

%validatorDir%\glslangValidator.exe -V %sourceDir%\skeletal.vert -o %sourceDir%\skeletalVert.spv
%validatorDir%\glslangValidator.exe -V %sourceDir%\skeletal.frag -o %sourceDir%\skeletalFrag.spv