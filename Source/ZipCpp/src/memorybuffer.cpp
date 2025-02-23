#include "ZipCpp/memorybuffer.h"

ZipCpp::MemoryBuffer::MemoryBuffer(const std::size_t size) : ZipCpp::MemoryBuffer(MemoryBuffer::Data_t(size)) {}

ZipCpp::MemoryBuffer::MemoryBuffer(MemoryBuffer::Data_t&& data)
    : data_(std::move(data)), characterView_(reinterpret_cast<char*>(data_.data()), data_.size())
{}

ZipCpp::MemoryBuffer::MemoryBuffer(const MemoryBuffer& other)
    : data_(other.data_), characterView_(reinterpret_cast<char*>(data_.data()), data_.size())
{}

ZipCpp::MemoryBuffer::MemoryBuffer(MemoryBuffer&& other) noexcept
    : data_(std::move(other.data_)), characterView_(reinterpret_cast<char*>(data_.data()), data_.size())
{}

ZipCpp::MemoryBuffer::~MemoryBuffer() = default;

ZipCpp::MemoryStream ZipCpp::MemoryBuffer::getMemoryStream() const
{
    return MemoryStream{characterView_};
}
ZipCpp::MemoryBuffer::Data_t& ZipCpp::MemoryBuffer::getData()
{
    return data_;
}
const ZipCpp::MemoryBuffer::Data_t& ZipCpp::MemoryBuffer::getData() const
{
    return data_;
}
