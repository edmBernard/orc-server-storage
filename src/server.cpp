//
//  orc-server-storage
//
//  https://github.com/edmBernard/orc-server-storage
//
//  Created by Erwan BERNARD on 04/01/2019.
//
//  Copyright (c) 2019 Erwan BERNARD. All rights reserved.
//  Distributed under the Apache License, Version 2.0. (See accompanying
//  file LICENSE or copy at http://www.apache.org/licenses/LICENSE-2.0)
//

#include "stryke/core.hpp"
#include "stryke/thread.hpp"
#include <nlohmann/json.hpp>
#include <cxxopts.hpp>
#include <uWS/uWS.h>
#include <string>

using namespace uWS;
using json = nlohmann::json;

struct Clo {
  int port;
};


void send_response_message(HttpResponse *res, std::string message) {
  std::cout << message << std::endl;
  res->end(message.data(), message.length());
}


int main(int argc, char *argv[]) {
  Clo clo;

  try {
    // =================================================================================================
    // Parse command line options
    cxxopts::Options options(argv[0], "A storage server for Orc file");
    options.positional_help("[optional args]").show_positional_help();

    // clang-format off
    options.add_options()
      ("help", "Print help")
      ("p, port", "Served port",
        cxxopts::value<int>()->default_value("80"), "PORT");
    // clang-format on
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      std::cout << options.help({"", "Group"}) << std::endl;
      exit(0);
    }
    std::cout << "port :" << result["port"].as<int>() << std::endl;

    clo.port = result["port"].as<int>();

  } catch (const cxxopts::OptionException &e) {
    std::cout << "Error parsing options: " << e.what() << std::endl;
    exit(1);
  }


  // =================================================================================================
  // Parse command line options
  std::string response = "Hello!";
  Hub h;

  h.onHttpRequest([&](HttpResponse *res, HttpRequest req, char *data, size_t length, size_t remainingBytes) {
    std::cout << "onHttpRequest" << std::endl;
    res->httpSocket->setUserData(new std::string);
    if (req.getMethod() == uWS::HttpMethod::METHOD_POST) {
      if (!remainingBytes) {
        json message;
        try {
          message = json::parse(std::string(data, length));
        } catch (const json::exception &e){
          send_response_message(res, std::string("Error parsing json: ") + e.what());
          return;
        }
        std::cout << message.dump() << std::endl;
        res->end(response.data(), response.length());
        return;
      }
    }
    res->end(nullptr, 0);
    return;
  });

  if (h.listen(clo.port)) {
    h.run();
  }
}
