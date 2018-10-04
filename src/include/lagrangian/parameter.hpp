/*
    This file is part of lagrangian library.

    lagrangian is free software: you can redistribute it and/or modify
    it under the terms of GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    lagrangian is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of GNU Lesser General Public License
    along with lagrangian. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

// ___________________________________________________________________________//

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <ostream>
#include <string>
#include <vector>

// ___________________________________________________________________________//

namespace lagrangian {

/**
 * @brief Management for parameter file.
 */
class Parameter {
 private:
  std::map<std::string, std::vector<std::string>> data_;

  bool Parse(std::string& line, std::string& buffer);

  /**
   * @brief Return items associated with a parameter.
   *
   * @param key Parameter to read
   *
   * @returns The list of values
   *
   * @throw std::runtime_error if the setting does not exist
   */
  std::vector<std::string> const& Items(const std::string& key) const {
    if (!Exists(key)) {
      throw std::runtime_error(
          boost::str(boost::format("parameter `%s' is not defined") % key));
    }

    auto it = data_.find(key);
    return (*it).second;
  }

  /**
   * @brief Transforms a string to the type defined by the user.
   *
   * @param value
   *
   * @return The transformed value
   *
   * @throw  std::runtime_error If the value can not be converted to type T.
   */
  template <class T>
  T ParameterCast(const std::string& value) const {
    try {
      return boost::lexical_cast<T>(value);
    } catch (boost::bad_lexical_cast& e) {
      throw std::runtime_error(boost::str(
          boost::format("value `%s' could not be interpreted as %s") %
          boost::lexical_cast<std::string>(value) % e.target_type().name()));
    }
  }

 public:
  /**
   * @brief Create a new instance from an existing file
   *
   * @param filename File to load into memory.
   *
   * @throw std::runtime_error if the file does not exist or contains a
   * syntax error.
   */
  explicit Parameter(const std::string& filename) { Load(filename); }

  /**
   * @brief Create a new instance with no data.
   */
  Parameter() {}

  /**
   * @brief Loads the configuration file.
   *
   * @param filename File to load into memory
   *
   * @throw std::runtime_error if the file does not exist or contains a syntax
   * error
   */
  void Load(const std::string& filename);

  /**
   * @brief Deletes data associated with a key.
   *
   * @param key Key to delete
   */
  inline void Clear(const std::string& key) { data_.erase(key); }

  /**
   * @brief Removes all data associated with the instance.
   */
  inline void Clear() { data_.clear(); }

  /**
   * @brief Associates a new value to a key.
   *
   * @param key Key to update
   * @param value New value to assign
   */
  template <class T>
  inline void AddValue(const std::string& key, T value) {
    data_[key].push_back(boost::lexical_cast<std::string>(value));
  }

  /**
   * @brief Test if a parameter exists.
   *
   * @param key Key to test
   *
   * @result True if the key exists otherwise false
   */
  inline bool Exists(const std::string& key) const {
    auto it = data_.find(key);
    return it != data_.end();
  }

  /**
   * @brief Returns the number of values loaded into memory for a given
   * parameter.
   *
   * @param key Key to test
   *
   * @result Number of values associated with the key.
   */
  inline size_t Size(const std::string& key) const { return Items(key).size(); }

  /**
   * @brief Returns the number of parameters loaded into memory.
   *
   * @result Number of parameters
   */
  inline size_t Size() const { return data_.size(); }

  /**
   * @brief Returns list of known parameters.
   *
   * @result Parameters list
   */
  std::vector<std::string> Keys() const {
    std::vector<std::string> result;

    for (auto& item : data_) {
      result.push_back(item.first);
    }

    return result;
  }

  /**
   * @brief Returns the list of values associated with a parameter.
   *
   * @result Value list
   */
  template <class T>
  std::vector<T> Values(const std::string& key) const {
    std::vector<T> result;

    if (Exists(key)) {
      for (auto& item : Items(key)) {
        result.push_back(ParameterCast<T>(item));
      }
    }
    return result;
  }

  /**
   * @brief Returns the value of a parameter.
   *
   * @param key %Parameter
   * @param index %Index of the value to read
   *
   * @result Value read
   *
   * @throw std::runtime_error if the value can not be converted to type T
   * @throw std::out_of_range if the requested index is greater than the
   * number of available values
   */
  template <class T>
  inline T Value(const std::string& key, const int index = 0) const {
    return ParameterCast<T>(Items(key).at(index));
  }

  /**
   * @brief Display settings loaded into memory.
   *
   * @param os The stream to update
   * @param p Parameters to display
   *
   * @result The stream updated
   */
  friend std::ostream& operator<<(std::ostream& os, const Parameter& p) {
    for (auto& key : p.Keys()) {
      for (auto& value : p.Values<std::string>(key)) {
        os << key << " = " << value << std::endl;
      }
    }
    return os;
  }

  /**
   * @brief Converts the instance to a string
   */
  std::string ToString() const {
    std::ostringstream ss;
    ss << *this;
    std::string result = ss.str();
    return result;
  }
};
}  // namespace lagrangian
