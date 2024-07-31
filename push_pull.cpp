
#include "lib.hpp"
#include <zmq.hpp>

int main() {
  zmq::context_t ctx;
  zmq::socket_t rx(ctx, zmq::socket_type::pull);
  zmq::socket_t tx(ctx, zmq::socket_type::push);
  zc_run(std::move(ctx), std::move(rx), std::move(tx));
  return 0;
}
