#include "ZipCpp/memorystream.h"

ZipCpp::membuf::membuf(const std::string_view characterView) : characterView_(characterView)
{
    char* p(const_cast<char*>(characterView_.data()));
    this->setg(p, p, p + characterView_.size());
}
ZipCpp::membuf::membuf(ZipCpp::membuf&& other) noexcept : membuf(other.characterView_) {}

ZipCpp::MemoryStream::MemoryStream(const std::string_view characterView)
    : membuf(characterView), std::istream(static_cast<std::streambuf*>(this))
{}

ZipCpp::MemoryStream::MemoryStream(ZipCpp::MemoryStream&& other) noexcept
    : membuf(std::move(other)), std::istream(static_cast<std::streambuf*>(this))
{}
std::iostream::pos_type ZipCpp::MemoryStream::seekpos(std::iostream::pos_type sp, std::ios_base::openmode which)
{
    return seekoff(sp - std::iostream::pos_type(std::iostream::off_type(0)), std::ios_base::beg, which);
}
std::iostream::pos_type ZipCpp::MemoryStream::seekoff(
        std::iostream::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode)
{
    if (dir == std::ios_base::cur)
    {
        gbump(static_cast<int>(off));
    }
    else if (dir == std::ios_base::end)
    {
        setg(eback(), egptr() + off, egptr());
    }
    else if (dir == std::ios_base::beg)
    {
        setg(eback(), eback() + off, egptr());
    }
    return gptr() - eback();
}
