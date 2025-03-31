#include "rt_buffer.hpp"


#include <cassert>
#include <cstring>

namespace rt {


     // Palauttaa minimi instassi koon jotta se toimisi laitteen minOffsetAlignment
    VkDeviceSize RtBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
        if (minOffsetAlignment > 0) {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    RtBuffer::RtBuffer(
        RtDevice &device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment)
        : rtDevice{ device },
        instanceSize{ instanceSize },
        instanceCount{ instanceCount },
        usageFlags{ usageFlags },
        memoryPropertyFlags{ memoryPropertyFlags } {
        alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;
        device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
    }

    RtBuffer::~RtBuffer() {
        unmap();
        vkDestroyBuffer(rtDevice.device(), buffer, nullptr);
        vkFreeMemory(rtDevice.device(), memory, nullptr);
    }


    //M‰p‰‰ muintion kooan t‰h‰ bufferiin, jos onnistuu  niin m‰pp‰‰ ne tietylle bufferin koolle
    VkResult RtBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
        assert(buffer && memory && "Called map on buffer before create");
        return vkMapMemory(rtDevice.device(), memory, offset, size, 0, &mapped);
    }

    // unm‰pp‰‰ muisti rangen
    void RtBuffer::unmap() {
        if (mapped) {
            vkUnmapMemory(rtDevice.device(), memory);
            mapped = nullptr;
        }
    }

    
    // Kopio kuvatun datan m‰pin bufferiin, esiasetettuna koko bufferin rangelle
    void RtBuffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
        assert(mapped && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE) {
            memcpy(mapped, data, bufferSize);
        }
        else {
            char* memOffset = (char*)mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    //Putsaa muistin rangen bufferista jotta se n‰kyisi laitteille
    VkResult RtBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(rtDevice.device(), 1, &mappedRange);
    }


    // N‰ytt‰‰ invalidoitun muisti rangen bufferista jotta se n‰kyy k‰ytt‰j‰lle
    VkResult RtBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(rtDevice.device(), 1, &mappedRange);
    }


    // luodaan bufferin tiedon kuvaajan
    VkDescriptorBufferInfo RtBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
        return VkDescriptorBufferInfo{
            buffer,
            offset,
            size,
        };
    }


    //Kopio "instanceSize" data m‰p‰tylle bufferille ofsetilla "index * alignmentSize"
    void RtBuffer::writeToIndex(void* data, int index) {
        writeToBuffer(data, instanceSize, index * alignmentSize);
    }


    // Putsaa muisti range "index * alignmentSize" bufferista jotta se n‰kyis laitteelle
    VkResult RtBuffer::flushIndex(int index) { return flush(alignmentSize, index * alignmentSize); }


    //Tekee bufferi info kuvaajasta
    VkDescriptorBufferInfo RtBuffer::descriptorInfoForIndex(int index) {
        return descriptorInfo(alignmentSize, index * alignmentSize);
    }


    // Invalidoi  muisti range bufferista jotta se n‰kyis k‰ytt‰j‰lle
    VkResult RtBuffer::invalidateIndex(int index) {
        return invalidate(alignmentSize, index * alignmentSize);
    }

}