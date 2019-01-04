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
#include "stryke/multifile.hpp"
#include "stryke/thread.hpp"
#include <array>
#include <cxxopts.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <uWS/uWS.h>

using namespace uWS;
using namespace stryke;
using json = nlohmann::json;

void send_response_message(HttpResponse *res, std::string message) {
  std::cout << message << std::endl;
  res->end(message.data(), message.length());
}

int main(int argc, char *argv[]) try {
  // =================================================================================================
  // Parse command line options
  cxxopts::Options options(argv[0], "A storage server for Orc file");
  options.positional_help("[optional args]").show_positional_help();

  // clang-format off
    options.add_options()
      ("help", "Print help")
      ("p, port", "Served port", cxxopts::value<int>()->default_value("80"), "PORT")
      ("cron", "Cron to close file in minutes", cxxopts::value<int>()->default_value("60"), "CRON")
      ("prefix", "File prefix", cxxopts::value<std::string>()->default_value("orc_"), "PREFIX")
      ("dir", "Data directory", cxxopts::value<std::string>()->default_value("data"), "DIR")
      ("col0", "Columns name", cxxopts::value<std::string>()->default_value({"timestamp"}))
      ("col1", "Columns name", cxxopts::value<std::string>()->default_value({"col1"}))
      ("col2", "Columns name", cxxopts::value<std::string>()->default_value({"col2"}))
      ("col3", "Columns name", cxxopts::value<std::string>()->default_value({"col3"}))
      ("col4", "Columns name", cxxopts::value<std::string>()->default_value({"col4"}));
  // clang-format on
  options.parse_positional({"col0", "col1", "col2", "col3", "col4"});
  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help({""}) << std::endl;
    exit(0);
  }

  // Get Port Parameter
  std::cout << "port :" << result["port"].as<int>() << std::endl;

  std::cout << "col0 :" << result["col0"].as<std::string>() << std::endl;
  std::cout << "col1 :" << result["col1"].as<std::string>() << std::endl;
  std::cout << "col2 :" << result["col2"].as<std::string>() << std::endl;
  std::cout << "col3 :" << result["col3"].as<std::string>() << std::endl;
  std::cout << "col4 :" << result["col4"].as<std::string>() << std::endl;

  // =================================================================================================
  // Initialise writer
  auto orc_options = WriterOptions();
  orc_options.set_cron(result["cron"].as<int>());
  OrcWriterThread<OrcWriterMulti, TimestampNumber, Long, Long, Long, Long> writer({result["col0"].as<std::string>(), result["col1"].as<std::string>(), result["col2"].as<std::string>(), result["col3"].as<std::string>(), result["col4"].as<std::string>()}, result["dir"].as<std::string>(), result["prefix"].as<std::string>(), orc_options);

  // =================================================================================================
  // Parse command line options

  Hub h;

  h.onHttpRequest([&](HttpResponse *res, HttpRequest req, char *data, size_t length, size_t remainingBytes) {
    std::cout << "onHttpRequest" << std::endl;
    res->httpSocket->setUserData(new std::string);
    if (req.getMethod() == uWS::HttpMethod::METHOD_POST) {
      if (!remainingBytes) {
        json message;
        try {
          message = json::parse(std::string(data, length));
          std::cout << message.dump() << std::endl;

          if (!message.is_array()) {
            send_response_message(res, std::string("Data must be an array"));
            return;
          }

          for (auto &&elem : message) {
            writer.write(
                elem[result["col0"].as<std::string>()].get<double>(),
                elem[result["col1"].as<std::string>()].get<long>(),
                elem[result["col2"].as<std::string>()].get<long>(),
                elem[result["col3"].as<std::string>()].get<long>(),
                elem[result["col4"].as<std::string>()].get<long>());
          }

        } catch (const json::exception &e) {
          send_response_message(res, std::string("Error parsing json: ") + e.what());
          return;
        }
        send_response_message(res, std::string("Data successfuly uploaded"));
        return;
      }
    }
    res->end(nullptr, 0);
    return;
  });

  if (h.listen(result["port"].as<int>())) {
    h.run();
  }
} catch (const cxxopts::OptionException &e) {
  std::cout << "Error parsing options: " << e.what() << std::endl;
  exit(1);
}
