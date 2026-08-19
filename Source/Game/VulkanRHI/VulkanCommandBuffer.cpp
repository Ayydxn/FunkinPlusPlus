#include "FunkinPCH.h"
#include "VulkanCommandBuffer.h"

CVulkanCommandBuffer::CVulkanCommandBuffer(vk::CommandBuffer CommandBuffer)
    : m_CommandBuffer(CommandBuffer) {}
