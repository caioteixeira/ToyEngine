-----------------------------------------------
DirectXTK - the DirectX Tool Kit for DirectX 12
-----------------------------------------------

Copyright (c) Microsoft Corporation. All rights reserved.

April 24, 2017

This package contains the "DirectX Tool Kit", a collection of helper classes for 
writing Direct3D 12 C++ code for Universal Windows Platform (UWP) apps, Win32 desktop
applications for Windows 10, and Xbox One exclusive apps.

This code is designed to build with Visual Studio 2015. It is recommended that you
make use of VS 2015 Update 3, Windows Tools 1.4.1, and the
Windows 10 Anniversary Update SDK (14393).

Inc\
    Public Header Files (in the DirectX C++ namespace):

    Audio.h - low-level audio API using XAudio2 (DirectXTK for Audio public header)
    CommonStates.h - common D3D state combinations
    DDSTextureLoader.h - light-weight DDS file texture loader
    DescriptorHeap.h - helper for managing DX12 descriptor heaps
    DirectXHelpers.h - misc C++ helpers for D3D programming
    Effects.h - set of built-in shaders for common rendering tasks
    EffectPipelineStateDescription.h - helper for creating PSOs
    GamePad.h - gamepad controller helper using XInput
    GeometricPrimitive.h - draws basic shapes such as cubes and spheres
    GraphicsMemory.h - helper for managing dynamic graphics memory allocation
    Keyboard.h - keyboard state tracking helper
    Model.h - draws meshes loaded from .SDKMESH or .VBO files
    Mouse.h - mouse helper
    PrimitiveBatch.h - simple and efficient way to draw user primitives
    RenderTargetState.h - helper for communicating render target requirements when creating PSOs
    ResourceUploadBatch.h - helper for managing texture resource upload to the GPU
    ScreenGrab.h - light-weight screen shot saver
    SimpleMath.h - simplified C++ wrapper for DirectXMath
    SpriteBatch.h - simple & efficient 2D sprite rendering
    SpriteFont.h - bitmap based text rendering
    VertexTypes.h - structures for commonly used vertex data formats
    WICTextureLoader.h - WIC-based image file texture loader
    XboxDDSTextureLoader.h - Xbox One exclusive apps variant of DDSTextureLoader

Src\
    DirectXTK source files and internal implementation headers

Audio\
    DirectXTK for Audio source files and internal implementation headers

NOTE: MakeSpriteFont and XWBTool can be found in the DirectX Tool Kit for DirectX 11 package.

All content and source code for this package are subject to the terms of the MIT License.
<http://opensource.org/licenses/MIT>.

Documentation is available at <https://github.com/Microsoft/DirectXTK12/wiki>.

For the latest version of DirectX Tool Kit, bug reports, etc. please visit the project site.

http://go.microsoft.com/fwlink/?LinkID=615561

Note: Xbox One exclusive apps developers using the Xbox One XDK need to generate the
      Src\Shaders\Compiled\XboxOne*.inc files to build the library as they are not
      included in the distribution package. They are built by running the script
      in Src\Shaders - "CompileShaders xbox", and should be generated with the matching
      FXC compiler from the Xbox One XDK. While they will continue to work if outdated,
      a mismatch will cause runtime compilation overhead that would otherwise be avoided.

This project has adopted the Microsoft Open Source Code of Conduct. For more information see the
Code of Conduct FAQ or contact opencode@microsoft.com with any additional questions or comments.

https://opensource.microsoft.com/codeofconduct/


---------------------------------
COMPARISONS TO DIRECTX 11 VERSION
---------------------------------

* No support for loading .CMO models or DGSL effect shaders (i.e. DGSLEffect)

* VertexTypes does not include VertexPositionNormalTangentColorTexture or
  VertexPositionNormalTangentColorTextureSkinning

* DirectX Tool Kit for DirectX 11 supports Feature Level 9.x, while DirectX 12 requires
  Direct3D Feature Level 11.0. There are no expected DirectX 12 drivers for
  any lower feature level devices.

* The library assumes it is building for Windows 10 (aka _WIN32_WINNT=0x0A00) so it makes
  use of XAudio 2.9 and WIC2 as well as DirectX 12.

* DirectX Tool Kit for Audio, GamePad, Keyboard, Mouse, and SimpleMath are identical
  to the DirectX 11 version


---------------
RELEASE HISTORY
---------------

April 24, 2017
    Regenerated shaders using Windows 10 Creators Update SDK (15063)
    Fixed NormalMapEffect shader selection for specular texture usage
    Fixed Direct3D validation layer issues when using Creators Update
    Fixed AudioEngine enumeration when using Single Threaded Apartment (STA)
    Fixed bug with GamePad (Windows.Gaming.Input) when no user bound 
    Updated D3DX12 internal copy to latest version

April 7, 2017
    VS 2017 updated for Windows Creators Update SDK (15063)
    XboxDDSTextureLoader updates

February 10, 2017
    SpriteBatch default rasterizer state now matches DirectX 11 version
    DDSTextureLoader now supports loading planar video format textures
    GamePad now supports special value of -1 for 'most recently connected controller'
    WIC format 40bppCMYKAlpha should be converted to RGBA8 rather than RGBA16
    DDS support for L8A8 with bitcount 8 rather than 16
    Updated D3DX12 internal copy to latest version
    Minor code cleanup

December 5, 2016
    Mouse and Keyboard classes updated with IsConnected method
    Windows10 project /ZW switch removed to support use in C++/WinRT projection apps
    VS 2017 RC projects added
    Updated D3DX12 internal copy to latest version
    Minor code cleanup

October 6, 2016
    SDKMESH loader and BasicEffects support for compressed vertex normals with biasing
    *breaking change* 
      DDSTextureLoader Ex bool forceSRGB and generateMipsIfMissing parmeters are now a DDS_LOADER flag
      WICTextureLoader Ex bool forceSRGB and generateMips parameters are now a WIC_LOADER flag
    Add vertexCount member to ModelMeshPart
    Minor code cleanup

September 15, 2016
    Rebuild shaders using 1.0 Root Signature for improved compatibility
    Minor code cleanup

September 1, 2016
    EffectPipelineStateDescription is now in it's own header
    Additional debug object naming
    Fixed Tier 1 hardware support issues with BasicEffect and generating mipmaps
    Fixed default graphics memory alignment to resolve rendering problems on some hardware
    Added forceSRGB optional parameter to SpriteFont ctor
    EffectFactory method EnableForceSRGB added
    Removed problematic ABI::Windows::Foundation::Rect interop for SimpleMath
    Updated D3DX12 internal copy for the Windows 10 Anniversary Update SDK (14393)
    Minor code cleanup

August 4, 2016
    GraphicsMemory fix for robustness during cleanup
    Regenerated shaders using Windows 10 Anniversary Update SDK (14393)

August 2, 2016
    Updated for VS 2015 Update 3 and Windows 10 SDK (14393)

August 1, 2016
    Model effects array is now indexed by part rather than by material
    GamePad capabilities information updated for Universal Windows and Xbox One platforms
    Specular falloff lighting computation fix in shaders

July 18, 2016
    *breaking changes* to CommonStates, DescriptorHeap, Effects, Model,
        EffectPipelineStateDescription, and SpriteBatchPipelineStateDescription
    - added texture sampler control to Effects and SpriteBatch
    - fixed Model control of blend and rasterizer state
    - fixed problems with PerPixelLighting control (EffectFactory defaults to per-pixel lighting)
    - fixed control of weights-per-vertex optimization for SkinnedEffect
    - removed unnecesary "one-light" shader permutations
    - fixed bug in AlphaTestEfect implementation
    - improved debug messages for misconfigured effects
    NormalMapEffect for normal-map with optional specular map rendering
    EnvironmentMapEffect now supports per-pixel lighting
    Effects updated with SetMatrices and SetColorAndAlpha methods
    GraphicsMemory support for SharedGraphicsResource shared_ptr style smart-pointer
    PrimitiveBatch fix for DrawQuad
    ScreenGrab handles resource state transition
    SimpleMath: improved interop with DirectXMath constants
    WICTextureLoader module LoadWICTexture* methods
    Fixed bugs with GenerateMips for sRGB and BGRA formats
    Code cleanup

June 30, 2016
    Original release
