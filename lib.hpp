#pragma once

#include <zmq.hpp>

void zc_run(zmq::context_t ctx, zmq::socket_t rx, zmq::socket_t tx);
