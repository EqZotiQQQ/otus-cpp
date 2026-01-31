#include <string>
#include <span>
#include <boost/functional/hash.hpp>
#include <boost/program_options.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>

struct IBlockHasher {
    virtual ~IBlockHasher() = default;
    virtual std::string hash(const char* data, size_t size) = 0;
};

struct Md5Hasher : IBlockHasher {
    std::string hash(const char* data, size_t size) override {
        boost::uuids::detail::md5 md5;
        boost::uuids::detail::md5::digest_type digest;

        md5.process_bytes(data, size);
        md5.get_digest(digest);

        const auto* bytes = reinterpret_cast<const unsigned char*>(&digest);

        std::string result;
        result.reserve(32);
        boost::algorithm::hex(bytes, bytes + sizeof(digest), std::back_inserter(result));
        return result;
    }
};


