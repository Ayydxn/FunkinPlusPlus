#pragma once

#include "VulkanIncludes.h"
#include "RHICore/CommandBuffer.h"

class CVulkanCommandBuffer final : public ICommandBuffer
{
public:
    explicit CVulkanCommandBuffer(vk::CommandBuffer CommandBuffer);
    ~CVulkanCommandBuffer() override = default;

    vk::CommandBuffer GetHandle() const { return m_CommandBuffer; }
private:
    vk::CommandBuffer m_CommandBuffer;
};
