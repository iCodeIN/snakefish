#ifndef SNAKEFISH_CHANNEL_H
#define SNAKEFISH_CHANNEL_H

#include <atomic>
#include <tuple>
#include <utility>

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include "buffer.h"
#include "shared_buffer.h"

namespace snakefish {

static const unsigned PICKLE_PROTOCOL = 4;
static const size_t MAX_SOCK_MSG_SIZE = 1024;                          // bytes
static const size_t DEFAULT_CHANNEL_SIZE = 2l * 1024l * 1024l * 1024l; // 2 GiB

class receiver; // forward declaration to solve circular dependency

/*
 * The sending part of an IPC channel.
 *
 * Note that the `send_*()` functions do not protect against concurrent
 * accesses.
 */
class sender {
public:
  /*
   * No default constructor.
   */
  sender() = delete;

  /*
   * Default destructor that does nothing.
   *
   * Since a `sender` is used for IPC, any resources held by it should
   * only be released by the last owning process. That process should
   * call the `dispose()` function.
   */
  ~sender() = default;

  /*
   * No copy constructor.
   */
  sender(const sender &t) = delete;

  /*
   * No copy assignment operator.
   */
  sender &operator=(const sender &t) = delete;

  /*
   * Default move constructor.
   */
  sender(sender &&t) = default;

  /*
   * No move assignment operator.
   */
  sender &operator=(sender &&t) = delete;

  /*
   * Create a synchronous channel.
   *
   * Here, synchronous means that the sender will block if the channel is full,
   * and the receiver will block if there's no incoming messages.
   *
   * \returns Two pairs of `(sender, receiver)`. One for each communicating
   * party.
   */
  friend std::tuple<sender, receiver, sender, receiver>
  sync_channel(size_t buffer_size);

  /*
   * Send some bytes.
   *
   * \param bytes Pointer to the start of the bytes.
   * \param len Number of bytes to send.
   */
  void send_bytes(const void *bytes, size_t len);

  /*
   * Send a python object.
   *
   * This function will serialize `obj` using `pickle` and send the binary
   * output.
   */
  void send_pyobj(const py::object &obj);

  /*
   * Release resources held by this `sender`.
   */
  void dispose();

private:
  /*
   * Private constructor to be used by the friend functions.
   */
  explicit sender(int socket_fd, shared_buffer shared_mem)
      : socket_fd(socket_fd), shared_mem(std::move(shared_mem)) {}

  int socket_fd;
  shared_buffer shared_mem; // buffer used to hold large messages
};

/*
 * The receiving part of an IPC channel.
 *
 * Note that the `receive_*()` functions do not protect against concurrent
 * accesses.
 */
class receiver {
public:
  /*
   * No default constructor.
   */
  receiver() = delete;

  /*
   * Default destructor that does nothing.
   *
   * Since a `receiver` is used for IPC, any resources held by it should
   * only be released by the last involved process. That process should
   * call the `dispose()` function.
   */
  ~receiver() = default;

  /*
   * No copy constructor.
   */
  receiver(const receiver &t) = delete;

  /*
   * No copy assignment operator.
   */
  receiver &operator=(const receiver &t) = delete;

  /*
   * Default move constructor.
   */
  receiver(receiver &&t) = default;

  /*
   * No move assignment operator.
   */
  receiver &operator=(receiver &&t) = delete;

  /*
   * Create a synchronous channel.
   *
   * Here, synchronous means that the sender will block if the channel is full,
   * and the receiver will block if there's no incoming messages.
   *
   * \returns Two pairs of `(sender, receiver)`. One for each communicating
   * party.
   */
  friend std::tuple<sender, receiver, sender, receiver>
  sync_channel(size_t buffer_size);

  /*
   * Receive some bytes.
   *
   * \param len Number of bytes to receive.
   *
   * \returns The received bytes wrapped in a `buffer`.
   */
  buffer receive_bytes(size_t len);

  /*
   * Receive a python object.
   *
   * This function will receive some bytes and deserialize them using `pickle`.
   */
  py::object receive_pyobj();

  /*
   * Release resources held by this `receiver`.
   */
  void dispose();

private:
  /*
   * Private constructor to be used by the friend functions.
   */
  explicit receiver(int socket_fd, shared_buffer shared_mem)
      : socket_fd(socket_fd), shared_mem(std::move(shared_mem)) {}

  int socket_fd;
  shared_buffer shared_mem; // buffer used to hold large messages
};

/*
 * Create a synchronous channel.
 *
 * Here, synchronous means that the sender will block if the channel is full,
 * and the receiver will block if there's no incoming messages.
 *
 * \returns Two pairs of `(sender, receiver)`. One for each communicating
 * party.
 */
std::tuple<sender, receiver, sender, receiver> sync_channel();

/*
 * Create a synchronous channel.
 *
 * Here, synchronous means that the sender will block if the channel is full,
 * and the receiver will block if there's no incoming messages.
 *
 * \param buffer_size The size of the channel buffer, which is allocated as
 * shared memory.
 *
 * \returns Two pairs of `(sender, receiver)`. One for each communicating
 * party.
 */
std::tuple<sender, receiver, sender, receiver> sync_channel(size_t buffer_size);

} // namespace snakefish

#endif // SNAKEFISH_CHANNEL_H
