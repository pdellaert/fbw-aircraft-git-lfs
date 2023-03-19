// Copyright (c) 2023 FlyByWire Simulations
// SPDX-License-Identifier: GPL-3.0

#ifndef FLYBYWIRE_A32NX_DATAOBJECTBASE_H
#define FLYBYWIRE_A32NX_DATAOBJECTBASE_H

#include <optional>
#include <sstream>
#include <string>
#include <utility>

#include <MSFS/Legacy/gauges.h>

#include "SimUnits.h"

/**
 * @brief The DataObjectBase class is the base class for all data objects.
 */
class DataObjectBase {
 private:

 protected:
  /**
   * The name of the variable in the sim
   */
  const std::string name;

  explicit DataObjectBase(const std::string& varName) : name(varName) {}


 public:
  DataObjectBase() = delete;                                  // no default constructor
  DataObjectBase(const DataObjectBase&) = delete;             // no copy constructor
  DataObjectBase& operator=(const DataObjectBase&) = delete;  // no copy assignment
  virtual ~DataObjectBase() = default;

  /**
   * @return the name of the variable
   */
  [[nodiscard]] const std::string& getName() const { return name; }

  /**
   * @return as string representation of the data object for logging and debugging purposes
   */
  [[nodiscard]] virtual std::string str() const {
    return name;
  }
};

#endif  // FLYBYWIRE_A32NX_DATAOBJECTBASE_H
