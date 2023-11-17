include("${CMAKE_SOURCE_DIR}/cmake/Util.cmake")
GROUP_SOURCE_FILES(${CMAKE_CURRENT_SOURCE_DIR})

#adding data files
Set (VulkanDataPath "${CMAKE_SOURCE_DIR}/Assets/Graphics/Vulkan")

# SUBDIRLIST(DATASUBDIRS ${VulkanDataPath})

# Set ( DataList "")
# FOREACH(subdir ${DATASUBDIRS})
#     #message (${subdir})
#     file(GLOB DataList
#         "${VulkanDataPath}/${subdir}/*.frag"
#         "${VulkanDataPath}/${subdir}/*.vert"
#         "${VulkanDataPath}/${subdir}/*.spv"
#         "${VulkanDataPath}/${subdir}/*.refl"
#     )
#     LIST(APPEND SourceList ${DataList})

#     source_group(${subdir} FILES ${DataList})
# ENDFOREACH()

set(VULKAN_FILES
inc/PresentationEngine.h
inc/ValidationManager.h
inc/VkCommandBufferFactory.h
inc/VkDrawCommandBuffer.h
inc/VkCommandWriter.h
inc/VkFrameBufferFactory.h
inc/VkQueueFactory.h
inc/VkRenderPassFactory.h
inc/VkSamplerFactory.h
inc/VulkanManager.h

src/PresentationEngine.cpp
src/ValidationManager.cpp
src/VkCommandBufferFactory.cpp
src/VkDrawCommandBuffer.cpp
src/VkCommandWriter.cpp
src/VkFrameBufferFactory.cpp
src/VkQueueFactory.cpp
src/VkRenderPassFactory.cpp
src/VkSamplerFactory.cpp
src/VulkanManager.cpp
)

set(VULKAN_PIPELINE_FILES
inc/pipeline/VulkanGraphicsPipeline.h
inc/pipeline/VulkanGraphicsPipelineFactory.h

src/pipeline/VulkanGraphicsPipeline.cpp
src/pipeline/VulkanGraphicsPipelineFactory.cpp
)

set(SHADER_MANAGEMENT
inc/shading/VkBufferFactory.h
inc/shading/VkDescriptorPoolFactory.h
inc/shading/VkShaderFactory.h
inc/shading/VkShaderResourceAllocator.h
inc/shading/VkShaderResourceManager.h

src/shading/VkBufferFactory.cpp
src/shading/VkDescriptorPoolFactory.cpp
src/shading/VkShaderFactory.cpp
src/shading/VkShaderResourceAllocator.cpp
src/shading/VkShaderResourceManager.cpp
)

set(SYNCHRONISATION_FILES 
inc/synchronisation/VkSynchroniserFactory.h
src/synchronisation/VkSynchroniserFactory.cpp
)

set(VULKAN_UTILITY_FILES
inc/utility/vk_mem_alloc.h
inc/utility/VkRenderingUnwrapper.h
inc/utility/VulkanMemoryManager.h
inc/utility/VulkanSettings.h
inc/utility/VulkanUtility.h
inc/utility/VkImageFactory.h

src/utility/VkRenderingUnwrapper.cpp
src/utility/VulkanMemoryManager.cpp
src/utility/VulkanUtility.cpp
src/utility/VkImageFactory.cpp

)

set(TARGET_NAME "gfxVulkan")
add_library(${TARGET_NAME} 
    ${VULKAN_FILES} 
    ${VULKAN_PIPELINE_FILES} 
    ${SHADER_MANAGEMENT} 
    ${SYNCHRONISATION_FILES} 
    ${VULKAN_UTILITY_FILES} 
)
add_library(Loops::${TARGET_NAME} ALIAS ${TARGET_NAME})

target_link_libraries(${TARGET_NAME} 
    PRIVATE
        glfw glm::glm Vulkan::Vulkan rapidjson Loops::core
)

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    VULKAN_ASSETS_PATH="${VulkanDataPath}"
    EFFECTS_PATH="${EFFECTS_PATH}"
)

IF( APPLE )
    MESSAGE( STATUS "Running under MacOS X" )
# Watch out, for this check also is TRUE under MacOS X because it
# falls under the category of Unix-like.
ELSEIF( UNIX )
    MESSAGE( STATUS "Running under Unix or a Unix-like OS" )
# Despite what you might think given this name, the variable is also
# true for 64bit versions of Windows.
ELSEIF( WIN32 )
    MESSAGE( STATUS "Running under Windows (either 32bit or 64bit), setting character set to unicode" )
    add_definitions(-DUNICODE -D_UNICODE)
ENDIF()
