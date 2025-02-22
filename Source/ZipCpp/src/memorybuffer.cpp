#include "ZipCpp/memorybuffer.h"

ZipCpp::MemoryBuffer::MemoryBuffer(const std::size_t size) : ZipCpp::MemoryBuffer(MemoryBuffer::Data_t(size)) {}

ZipCpp::MemoryBuffer::MemoryBuffer(MemoryBuffer::Data_t&& data)
    : data_(std::move(data)), characterView_(reinterpret_cast<char*>(data_.data()), data_.size())
{}

ZipCpp::MemoryStream ZipCpp::MemoryBuffer::getMemoryStream() const
{
    return {characterView_};
}
ZipCpp::MemoryBuffer::Data_t& ZipCpp::MemoryBuffer::getData()
{
    return data_;
}
const ZipCpp::MemoryBuffer::Data_t& ZipCpp::MemoryBuffer::getData() const
{
    return data_;
}
