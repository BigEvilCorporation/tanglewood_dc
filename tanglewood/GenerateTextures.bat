pushd cd\shaders
..\..\..\ion\tools\buildresource\_Build\win32\_intermediates_\win32-debug\bigevilcorp\ion\tools\buildresource\buildresource\buildresource.exe ^
vshader flattextured_v ..\..\..\ion\shaders\cg\flattextured.cgfx VertexProgram
..\..\..\ion\tools\buildresource\_Build\win32\_intermediates_\win32-debug\bigevilcorp\ion\tools\buildresource\buildresource\buildresource.exe ^
pshader flattextured_p ..\..\..\ion\shaders\cg\flattextured.cgfx FragmentProgram
popd