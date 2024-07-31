#include "lib.hpp"

#include <condition_variable>
#include <csignal>
#include <spdlog/spdlog.h>

static std::condition_variable terminate_signal;
static bool terminated = false;

void logger(zmq::socket_ref capture) {
  try {
    while (true) {
      zmq::message_t message;
      auto _ = capture.recv(message, zmq::recv_flags::none);
      spdlog::trace("Forwarding message: size={}b", message.size());
    }
  } catch (zmq::error_t e) {
    if (!terminated)
      spdlog::warn("Logger exitting with error: {}", e.what());
  }

  // thread is terminating, terminate whole application
  terminate_signal.notify_all();
}

void proxy(zmq::socket_ref rx, zmq::socket_ref tx, zmq::socket_ref capture) {
  spdlog::info("Listening..");

  try {
    zmq::proxy(rx, tx, capture);
  } catch (zmq::error_t e) {
    if (!terminated)
      spdlog::warn("Proxy exitting with error: {}", e.what());
  }

  // thread is terminating, terminate whole application
  terminate_signal.notify_all();
}

void zc_run(zmq::context_t ctx, zmq::socket_t rx, zmq::socket_t tx) {
  spdlog::info("Welcome to 0c!");
  spdlog::info("Booting up..");

  // configuration variables
  bool traces = std::getenv("0C_TRACE");

  // setup signals
  std::signal(SIGTERM, [](int sig) { terminate_signal.notify_all(); });
  std::signal(SIGINT, [](int sig) { terminate_signal.notify_all(); });

  // enable traces
  if (traces) {
    spdlog::info("Enabling traces..");
    spdlog::set_level(spdlog::level::trace);
  }

  // logger sockets
  zmq::socket_t logger_tx(ctx, zmq::socket_type::push);
  zmq::socket_t logger_rx(ctx, zmq::socket_type::pull);

  // binding sockets
  logger_rx.bind("inproc://#logger");
  logger_tx.connect("inproc://#logger");
  rx.bind("tcp://*:3000");
  tx.bind("tcp://*:4000");

  // finished setup
  spdlog::info("Boot finished! [rx :3000, tx :4000]");
  std::thread proxy_thread(proxy, std::move(rx), std::move(tx),
                           std::move(logger_tx));
  std::thread logger_thread(logger, std::move(logger_rx));

  // wait for termination
  if (!terminated) {
    std::mutex m;
    std::unique_lock lg(m);
    terminate_signal.wait(lg);
    terminated = true;
  }

  // close context and await threads to complete
  spdlog::info("Shutting down..");
  ctx.close();
  proxy_thread.join();
  logger_thread.join();

  spdlog::info("Goodbye!");
}
