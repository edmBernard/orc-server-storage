//
//  stryke-server
//
//  https://github.com/edmBernard/stryke-server
//
//  Created by Erwan BERNARD on 07/01/2019.
//
//  Copyright (c) 2019 Erwan BERNARD. All rights reserved.
//  Distributed under the Apache License, Version 2.0. (See accompanying
//  file LICENSE or copy at http://www.apache.org/licenses/LICENSE-2.0)
//

#include <cpr/cpr.h>
#include <curl/curl.h>
#include "stryke/type.hpp"
#include "stryke/options.hpp"
#include "stryke/thread.hpp"
#include "stryke_client/client.hpp"
#include <cxxopts.hpp>
#include <iostream>
#include <istream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <iomanip>

using json = nlohmann::json;

int main(int argc, char *argv[]) try {

  // =================================================================================================
  // Parse command line options
  cxxopts::Options options(argv[0], "A client stryke server");
  options.positional_help("[optional args]").show_positional_help();

  // clang-format off
    options.add_options()
      ("help", "Print help")
      ("h, host", "Server full hostname with port if needed", cxxopts::value<std::string>()->default_value("http://localhost:80"), "HOST")
      ("b, batchsize", "Size of batch send", cxxopts::value<int>()->default_value("1000"), "BATCHSIZE")
      ("n, number", "Number of point send", cxxopts::value<int>()->default_value("10000"), "NUMBER")
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
    return EXIT_SUCCESS;
  }

  // Get Port Parameter
  std::cout << "host :" << result["host"].as<std::string>() << std::endl;

  std::cout << "col0 :" << result["col0"].as<std::string>() << std::endl;
  std::cout << "col1 :" << result["col1"].as<std::string>() << std::endl;
  std::cout << "col2 :" << result["col2"].as<std::string>() << std::endl;
  std::cout << "col3 :" << result["col3"].as<std::string>() << std::endl;
  std::cout << "col4 :" << result["col4"].as<std::string>() << std::endl;

  auto start1 = std::chrono::high_resolution_clock::now();

  stryke::WriterOptions writeroptions;
  writeroptions.set_batch_size(result["batchsize"].as<int>());

  {
    stryke::OrcWriterThread<stryke_client::OrcClient, double, long, long, long, long> writer(
      {
        result["col0"].as<std::string>(),
        result["col1"].as<std::string>(),
        result["col2"].as<std::string>(),
        result["col3"].as<std::string>(),
        result["col4"].as<std::string>()
      },
      result["host"].as<std::string>(), std::string(""), writeroptions);

    for (int j = 0; j < result["number"].as<int>(); ++j) {

      double timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000.;
      writer.write(timestamp, j, j, j + 3,j + 4);
    }

    std::chrono::duration<double, std::milli> elapsed1 = std::chrono::high_resolution_clock::now() - start1;
    std::cout << std::setw(40) << std::left << "processing time : before sync : " << elapsed1.count() << " ms\n";
  }
  std::chrono::duration<double, std::milli> elapsed2 = std::chrono::high_resolution_clock::now() - start1;
  std::cout << std::setw(40) << std::left << "processing time : after forced sync : " << elapsed2.count() << " ms\n";
  return 0;

} catch (const cxxopts::OptionException &e) {
  std::cout << "Error parsing options: " << e.what() << std::endl;
  return EXIT_FAILURE;
}
