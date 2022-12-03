#include "server.hpp"

#include <netdb.h>
#include <fcntl.h>
#include <poll.h>

namespace alewa {

int const AI_FLAGS = AI_PASSIVE;
int const AI_FAMILY = AF_UNSPEC;
int const AI_SOCKTYPE = SOCK_STREAM;

int const SOCK_LEVEL = SOL_SOCKET;
int const SOCK_OPTNAME = SO_REUSEADDR;

int const F_CMD = F_SETFL;
int const F_ARG = O_NONBLOCK;

short const INPUT_EVENTS = POLLIN;

}  // namespace alewa
