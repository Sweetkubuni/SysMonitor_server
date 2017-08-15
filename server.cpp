#include <memory>
#include <cstdint>
#include <string>
#include <iostream>
#include <evhttp.h>

#include "rapidjson/document.h"

int main()
{

  event_base *base;
	evhttp *http;
	evhttp_bound_socket *handle;
  
  base = event_base_new();
  char const address[] = "0.0.0.0";
  std::uint16_t port = 80;
 
  if (!base) {
		std::cout<< "Couldn't create an event_base: exiting\n";
		return 1;
	}
    
  /* Create a new evhttp object to handle requests. */
	http = evhttp_new(base);
	if (!http) {
		std::cout <<"couldn't create evhttp. Exiting.\n";
		return 1;
	}
    
  void (*onReq)(evhttp_request *req, void *) = [] (evhttp_request *req, void * arg)
  {
    std::cout << "We got request!!!!\n\n";
    evbuffer *buf;
    buf = evhttp_request_get_input_buffer(req);
    if(buf == nullptr)
    {
      std::cout << "ERROR!!!\n";
      return;
    }

    char data[4096];
    int n = evbuffer_get_length(buf);
     evbuffer_copyout(buf, data, n);
    bool failed = false;
    // parse json file
    std::string msg(data, n);
    rapidjson::Document parsedocument;
    if(parsedocument.Parse<0>(msg.c_str()).HasParseError())
    {
      failed = true;
      evhttp_send_reply(req, 400, "BAD", nullptr);
    }

    std::string key = parsedocument["key"].GetString();
    std::uint64_t ram_total       = parsedocument["ram total"].GetInt();
    std::uint64_t ram_used        = parsedocument["ram used"].GetInt();
    double        cpu_percentage  = parsedocument["cpu percentage"].GetDouble();

    std::cout << "KEY: "<< key << "cpu_percentage: " << cpu_percentage << "ram_total"<< ram_total << "ram_used" << ram_used << "\n";
    if(!failed){
      evhttp_send_reply(req, 200, "OK", nullptr);
    }
  };
  
 evhttp_set_gencb(http, onReq, nullptr);
 handle = evhttp_bind_socket_with_handle(http, address, port);
 if (!handle) {
		std::cout << "couldn't bind to port" << port << "Exiting.\n";
		return 1;
	}
  event_base_dispatch(base);
  return 0;
}
