#include <concepts>

namespace alewa::io {

template <typename T>
concept IoApi = requires(T t)
{
    /* types */
    typename T::PollFd;
    typename T::Nfds_t;

    /* methods */
    requires requires (typename T::PollFd* fds, typename T::Nfds_t nfds)
    {
        { t.poll(fds, nfds, int{}) } -> std::same_as<int>;
    };
};

}  // namespace alewa::io
