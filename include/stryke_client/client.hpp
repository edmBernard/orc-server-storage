//
//  Stryke
//
//  https://github.com/edmBernard/Stryke
//
//  Created by Erwan BERNARD on 02/12/2018.
//
//  Copyright (c) 2018 Erwan BERNARD. All rights reserved.
//  Distributed under the Apache License, Version 2.0. (See accompanying
//  file LICENSE or copy at http://www.apache.org/licenses/LICENSE-2.0)
//
#pragma once
#ifndef STRYKE_CLIENT_HPP_
#define STRYKE_CLIENT_HPP_


#include "stryke/type.hpp"
#include "stryke/options.hpp"
#include "nlohmann/json.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>


namespace stryke_client {

namespace utils {
// ==============================================================
//  Create Cast Implementation: function to Cast tuple in json
// ==============================================================

template <typename Types, uint64_t N>
bool addData(nlohmann::json &dst_data, const Types &data, std::string column_name) {
  dst_data[column_name] = std::get<N>(data);
  return true;
}

template <typename... Types, std::size_t... Indices>
auto filljsonImpl(std::index_sequence<Indices...>, nlohmann::json &dst_data, std::tuple<Types...> &data, std::array<std::string, sizeof...(Types)> column_names) -> std::vector<bool> {
  return {addData<std::tuple<Types...>, Indices>(dst_data, data, column_names[Indices])...};
}

template <typename... Types>
auto filljson(nlohmann::json &dst_data, std::tuple<Types...> &data, std::array<std::string, sizeof...(Types)> column_names) {
  return filljsonImpl<Types...>(std::index_sequence_for<Types...>(), dst_data, data, column_names);
}

} // namespace utils

// ==============================================================
//  Simple Writer Implementation
// ==============================================================

template <typename... Types>
class OrcClient {
public:
  OrcClient(std::array<std::string, sizeof...(Types)> column_names, std::string host, std::string route, const stryke::WriterOptions &options)
      : writeroptions(options), column_names(column_names), url(host + route) {
  }

  ~OrcClient() {
    addToFile();
  }

  void write(Types... dataT) {
    this->write_tuple(std::make_tuple(dataT...));
  }

  void close() {
    this->addToFile();
  }

  void write_tuple(std::tuple<Types...> dataT) {

    if (this->numValues >= this->writeroptions.batchSize) {
      addToFile();
    }

    nlohmann::json elem;
    utils::filljson(elem, dataT, this->column_names);
    this->data.push_back(elem);

    ++this->numValues;
  }

  void addToFile() {
    static int count = 0;
    auto r = cpr::Post(cpr::Url{this->url},
                       cpr::Body{this->data.dump()},
                       cpr::Header{{"Content-Type", "text/plain"}});

    if (r.status_code != 200) {
      std::cout << "response : " << r.status_code << " : " << r.text << std::endl;
      std::cout << "message : " << this->data.dump() << std::endl;
    }
    ++count;
    std::cout << "count : " << count << std::endl;
    this->data.clear();

    this->numValues = 0;
  }

private:
  stryke::WriterOptions writeroptions;

  nlohmann::json data = nlohmann::json::array(); // buffer that holds a batch of rows in json
  std::array<std::string, sizeof...(Types)> column_names;

  std::string url;

  uint64_t numValues = 0; // num of lines read in a batch
};

} // namespace stryke_client

#endif // !STRYKE_HPP_
