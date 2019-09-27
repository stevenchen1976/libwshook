#include "utils.hpp"

namespace libwshook::utils {

  std::string encode64(std::string const &val) {
    using namespace boost::archive::iterators;
    using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
    return tmp.append((3 - val.size() % 3) % 3, '=');
  }

  std::string lower(std::string const &s) {
    return boost::algorithm::to_lower_copy(s);
  }

} // namespace libwshook::utils
