#include <memory>
#include <cstdint>
#include <string>
#include <iostream>
#include <evhttp.h>
int main()
{
  if (!event_init())
  {
    std::cerr << "Failed to init libevent." << std::endl;
    return -1;
  }
  char const SrvAddress[] = "127.0.0.1";
  std::uint16_t SrvPort = 5555;
  std::unique_ptr<evhttp, decltype(&evhttp_free)> Server(evhttp_start(SrvAddress, SrvPort), &evhttp_free);
  if (!Server)
  {
    std::cerr << "Failed to init http server." << std::endl;
    return -1;
  }
  void (*OnReq)(evhttp_request *req, void *) = [] (evhttp_request *req, void * arg)
  {
    evbuffer *buf;
    buf = evhttp_request_get_input_buffer(req);
    if(buf == nullptr)
    {
      std::cout << "ERROR!!!\n";
      return;
    }

    char cbuf[4096];
    int n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
    std::string msg(cbuf, n);
    cout << "MESSAGE: " << msg << "\n";
    evhttp_send_reply(req, 200, "OK", nullptr);
  };
  
  evhttp_set_gencb(Server.get(), OnReq, nullptr);
  if (event_dispatch() == -1)
  {
    std::cerr << "Failed to run messahe loop." << std::endl;
    return -1;
  }
  return 0;
}