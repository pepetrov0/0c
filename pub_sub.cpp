
#include "lib.hpp"
#include <zmq.hpp>

int main() {
  zmq::context_t ctx;
  zmq::socket_t rx(ctx, zmq::socket_type::xsub);
  zmq::socket_t tx(ctx, zmq::socket_type::xpub);
  zc_run(std::move(ctx), std::move(rx), std::move(tx));
  return 0;
}
