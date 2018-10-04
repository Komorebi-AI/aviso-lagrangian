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

#include <cstdlib>
#include <thread>

// ___________________________________________________________________________//

#include "lagrangian/integration.hpp"
#include "lagrangian/list.hpp"
#include "lagrangian/reader/netcdf.hpp"
#include "lagrangian/stencil.hpp"
#include "lagrangian/trace.hpp"

// ___________________________________________________________________________//

namespace std {
using ::getenv;
}  // namespace std

// ___________________________________________________________________________//

namespace lagrangian {

/**
 * @brief Properties of a regular grid
 */
class MapProperties {
 protected:
  double x_min_;  //!< Minimal longitude
  double y_min_;  //!< Minimal longitude
  double step_;   //!< Step between two consecutive longitudes and latitudes
  int nx_;        //!< Number of longitudes
  int ny_;        //!< Number of latitudes

 public:
  /**
   * @brief Default constructor
   *
   * @param nx Number of longitudes
   * @param ny Number of latitudes
   * @param x_min Minimal longitude
   * @param y_min Minimal latitude
   * @param step Step between two consecutive longitudes and latitudes
   */
  MapProperties(const int nx, const int ny, const double x_min,
                const double y_min, const double step)
      : x_min_(x_min), y_min_(y_min), step_(step), nx_(nx), ny_(ny) {}

  /**
   * @brief Default method invoked when a MapProperties is destroyed.
   */
  virtual ~MapProperties() = default;

  /**
   * @brief Get the longitude value
   *
   * @param ix %Index of the longitude in the grid
   * @return The longitude
   */
  inline double GetXValue(const int ix) const { return x_min_ + ix * step_; }

  /**
   * @brief Get the latitude value
   *
   * @param iy %Index of the latitude in the grid
   * @return The longitude
   */
  inline double GetYValue(const int iy) const { return y_min_ + iy * step_; }

  /**
   * @brief Get the number of longitudes in the grid
   *
   * @return The number of longitudes
   */
  inline int get_nx() const { return nx_; }

  /**
   * @brief Get the number of latitudes in the grid
   *
   * @return The number of latitudes
   */
  inline int get_ny() const { return ny_; }

  /**
   * @brief Get the step between two consecutive longitudes and latitudes
   *
   * @return The step
   */
  inline double get_step() const { return step_; }

  /**
   * @brief Get the minimal longitude
   *
   * @return The minimal longitude
   */
  inline double get_x_min() const { return x_min_; }

  /**
   * @brief Get the minimal latitude
   *
   * @return The minimal latitude
   */
  inline double get_y_min() const { return y_min_; }
};

// ___________________________________________________________________________//

/**
 * @brief Regular grid
 */
template <class T>
class Map : public MapProperties {
 private:
  std::vector<T> grid_;

 public:
  Map()
      : MapProperties(0, 0, std::numeric_limits<double>::quiet_NaN(),
                      std::numeric_limits<double>::quiet_NaN(), 0),
        grid_(std::vector<T>()) {}

  /**
   * Move constructor
   *
   * @param rhs right value
   */
  Map(Map&& rhs) = default;

  /**
   * Move assignment operator
   *
   * @param rhs right value
   */
  Map& operator=(Map&& rhs) = default;

  /**
   * @brief Default constructor
   *
   * @param nx Number of longitudes
   * @param ny Number of latitudes
   * @param x_min Minimal longitude
   * @param y_min Minimal latitude
   * @param step Step between two consecutive longitudes and latitudes
   */
  Map(const int nx, const int ny, const double x_min, const double y_min,
      const double step)
      : MapProperties(nx, ny, x_min, y_min, step),
        grid_(std::vector<T>(nx * ny)) {}

  /**
   * @brief Set the value for the cell [ix, iy]
   *
   * @param ix %Index of the longitude in the grid
   * @param iy %Index of the latitude in the grid
   * @param item Value
   */
  inline void SetItem(const int ix, const int iy, const T& item) {
    grid_[ix * get_ny() + iy] = item;
  }

  /**
   * @brief Get the value for the cell [ix, iy]
   *
   * @param ix %Index of the longitude in the grid
   * @param iy %Index of the latitude in the grid
   * @return The value of the cell
   */
  inline const T& GetItem(const int ix, const int iy) const {
    return grid_[ix * get_ny() + iy];
  }
};

// ___________________________________________________________________________//

namespace map {

/**
 * @brief Handles the computation of the map
 */
class FiniteLyapunovExponents {
 private:
  /**
   * @brief Compute a sub part of the map in a separate thread
   *
   * @param splitter Parameters of the sub-matrix to compute
   * @param fle Finite Lyapunov exponents
   * @param it Current time step
   */
  void ComputeHt(Splitter<Index>& splitter,
                 lagrangian::FiniteLyapunovExponentsIntegration& fle,
                 Iterator& it);

  /**
   * @brief Test if the computation for a cell is over
   *
   * @param index %Index of the cell
   * @return True if the computation is over otherwise false
   */
  inline bool Completed(const Index& index) {
    Position* position = map_.GetItem(index.get_i(), index.get_j());
    return position->get_completed() || position->IsMissing();
  }

  /// Number of threads
  int num_threads_;

  /// List of cells of the matrix to be solved
  SplitList<Index> indexes_;

 protected:
  /// Grid
  Map<Position*> map_;

 public:
  /**
   * @brief Default constructor
   *
   * @param nx Number of longitudes
   * @param ny Number of latitudes
   * @param x_min Minimal longitude
   * @param y_min Minimal latitude
   * @param step Step between two consecutive longitudes and latitudes
   *
   * @throw std::runtime_error if the shell variable OMP_NUM_THREADS
   * contains an invalid value
   */
  FiniteLyapunovExponents(const int nx, const int ny, const double x_min,
                          const double y_min, const double step)
      : num_threads_(std::thread::hardware_concurrency()),
        map_(nx, ny, x_min, y_min, step)

  {
    // Get the number of threads wanted by the user
    char* omp_num_threads = std::getenv("OMP_NUM_THREADS");
    if (omp_num_threads != nullptr) {
      try {
        num_threads_ = boost::lexical_cast<int>(omp_num_threads);
      } catch (boost::bad_lexical_cast& e) {
        throw std::runtime_error(
            std::string("Invalid value for OMP_NUM_THREADS: ") +
            omp_num_threads);
      }
    }
    Debug(str(boost::format("Uses %d threads") % num_threads_));
  }

  /**
   * @brief Default method invoked when a map is destroyed.
   */
  virtual ~FiniteLyapunovExponents() {
    for (int ix = 0; ix < map_.get_nx(); ++ix) {
      for (int iy = 0; iy < map_.get_ny(); ++iy) {
        delete map_.GetItem(ix, iy);
      }
    }
  }

  /**
   * @brief Initializing the grid cells
   *
   * @param fle Finite Lyapunov exponents
   * @param stencil Type of stencil used for the calculation of finite
   * difference.
   */
  void Initialize(
      lagrangian::FiniteLyapunovExponentsIntegration& fle,
      lagrangian::FiniteLyapunovExponentsIntegration::Stencil stencil =
          lagrangian::FiniteLyapunovExponentsIntegration::kTriplet);

  /**
   * @brief Initializing the grid cells. Cells located on the hidden values
   * ​​(eg continents) will be deleted from the calculation
   *
   * @param fle Finite Lyapunov exponents
   * @param reader NetCDF reader allow to access of the mask's value.
   * @param stencil Type of stencil used for the calculation of finite
   * difference.
   */
  void Initialize(
      lagrangian::FiniteLyapunovExponentsIntegration& fle,
      lagrangian::reader::Netcdf& reader,
      lagrangian::FiniteLyapunovExponentsIntegration::Stencil stencil =
          lagrangian::FiniteLyapunovExponentsIntegration::kTriplet);

  /**
   * @brief Compute the map
   *
   * @param fle Finite Lyapunov exponents
   */
  void Compute(lagrangian::FiniteLyapunovExponentsIntegration& fle);
};

}  // namespace map

// ___________________________________________________________________________//

/**
 * @brief Handles a map of Finite Size or Time Lyapunov Exponents
 */
class MapOfFiniteLyapunovExponents : public map::FiniteLyapunovExponents {
 private:
  using GetExponent = double (lagrangian::FiniteLyapunovExponents::*)() const;

  /**
   * @brief Default constructor
   *
   * @param nan Value of undefined cell
   * @param fle_integration integration object used to compute the
   *  integration
   * @param pGetExponent Function to use to calculate the exponent
   * @param pGetUndefinedExponent Function to use to return default value for
   * undefined exponent
   */
  Map<double>* GetMapOfExponents(
      const double nan,
      lagrangian::FiniteLyapunovExponentsIntegration& fle_integration,
      GetExponent pGetExponent, GetExponent pGetUndefinedExponent) const {
    lagrangian::FiniteLyapunovExponents fle{};

    auto result =
        new Map<double>(map_.get_nx(), map_.get_ny(), map_.get_x_min(),
                        map_.get_y_min(), map_.get_step());

    for (int ix = 0; ix < map_.get_nx(); ++ix) {
      for (int iy = 0; iy < map_.get_ny(); ++iy) {
        Position* position = map_.GetItem(ix, iy);
        if (position->IsMissing()) {
          result->SetItem(ix, iy, nan);
        } else {
          bool defined = fle_integration.ComputeExponents(position, fle);

          if (fle_integration.get_mode() ==
              lagrangian::FiniteLyapunovExponentsIntegration::kFTLE) {
            // In that case position is always completed
            double exponent = defined
                                  ? (fle.*pGetExponent)()
                                  : std::numeric_limits<double>::quiet_NaN();
            result->SetItem(ix, iy, exponent);
          } else {
            if (position->get_completed()) {
              double exponent = defined
                                    ? (fle.*pGetExponent)()
                                    : std::numeric_limits<double>::quiet_NaN();
              result->SetItem(ix, iy, exponent);
            } else {
              result->SetItem(ix, iy, (fle.*pGetUndefinedExponent)());
            }
          }
        }
      }
    }
    return result;
  }

 public:
  /**
   * @brief Default constructor
   *
   * @param nx Number of longitudes
   * @param ny Number of latitudes
   * @param x_min Minimal longitude
   * @param y_min Minimal latitude
   * @param step Step between two consecutive longitudes and latitudes
   */
  MapOfFiniteLyapunovExponents(const int nx, const int ny, const double x_min,
                               const double y_min, const double step)
      : map::FiniteLyapunovExponents(nx, ny, x_min, y_min, step) {}

  /**
   * @brief Get the map of the FLE associated to the maximum eigenvalues of
   * Cauchy-Green strain tensor
   *
   * @param nan Value representing an undefined data
   * @param fle FLE handler
   * @param get_lambda1 Function to use to return the value of λ₁
   * @param pGetUndefinedExponent Function to use to return default value for
   * undefined exponent
   *
   * @return The map of λ₁ (unit 1/sec)
   */
  Map<double>* GetMapOfLambda1(
      const double nan,
      lagrangian::FiniteLyapunovExponentsIntegration& fle) const {
    return GetMapOfExponents(
        nan, fle, &lagrangian::FiniteLyapunovExponents::get_lambda1,
        &lagrangian::FiniteLyapunovExponents::GetUndefinedExponent);
  }

  /**
   * @brief Get the map of the FLE associated to the minimum eigenvalues of
   * Cauchy-Green strain tensor
   *
   * @param nan Value representing an undefined data
   * @param fle FLE handler
   * @param get_lambda2 Function to use to return the value of λ₂
   * @param pGetUndefinedExponent Function to use to return default value for
   * undefined exponent
   *
   * @return The map of λ₂ (unit 1/sec)
   */
  Map<double>* GetMapOfLambda2(
      const double nan,
      lagrangian::FiniteLyapunovExponentsIntegration& fle) const {
    return GetMapOfExponents(
        nan, fle, &lagrangian::FiniteLyapunovExponents::get_lambda2,
        &lagrangian::FiniteLyapunovExponents::GetUndefinedExponent);
  }

  /**
   * @brief Get the map of the orientation of the eigenvectors associated
   * to the maximum eigenvalues of Cauchy-Green strain tensor
   *
   * @param nan Value representing an undefined data
   * @param fle FLE handler
   * @param get_theta1 Function to use to return the value of θ₁
   * @param pGetUndefinedVector Function to use to return default value for
   * undefined vector
   *
   * @return The map of θ₁ (unit degrees)
   */
  Map<double>* GetMapOfTheta1(
      const double nan,
      lagrangian::FiniteLyapunovExponentsIntegration& fle) const {
    return GetMapOfExponents(
        nan, fle, &lagrangian::FiniteLyapunovExponents::get_theta1,
        &lagrangian::FiniteLyapunovExponents::GetUndefinedVector);
  }

  /**
   * @brief Get the map of the orientation of the eigenvectors associated
   * to the minimum eigenvalues of Cauchy-Green strain tensor
   *
   * @param nan Value representing an undefined data
   * @param fle FLE handler
   * @param get_theta1 Function to use to return the value of θ₂
   * @param pGetUndefinedVector Function to use to return default value for
   * undefined vector
   *
   * @return The map of θ₂ (unit degrees)
   */
  Map<double>* GetMapOfTheta2(
      const double nan,
      lagrangian::FiniteLyapunovExponentsIntegration& fle) const {
    return GetMapOfExponents(
        nan, fle, &lagrangian::FiniteLyapunovExponents::get_theta2,
        &lagrangian::FiniteLyapunovExponents::GetUndefinedVector);
  }

  /**
   * @brief Get the map of the advection time
   *
   * @param nan Value representing an undefined data
   * @param fle FLE handler
   * @param get_delta_t Function to use to return the value of delta_t
   * @param GetUndefinedDeltaT Function to use the default value for undefined
   * delta_t
   *
   * @return The map of advection time (unit number of seconds elapsed
   * since the beginning of the integration)
   */
  Map<double>* GetMapOfDeltaT(
      const double nan,
      lagrangian::FiniteLyapunovExponentsIntegration& fle) const {
    return GetMapOfExponents(
        nan, fle, &lagrangian::FiniteLyapunovExponents::get_delta_t,
        &lagrangian::FiniteLyapunovExponents::GetUndefinedDeltaT);
  }

  /**
   * @brief Get the map of the effective final separation distance
   *
   * @param nan Value representing an undefined data
   * @param fle FLE handler
   * @param get_final_separation Function to use to return the value of
   * final_separation
   * @param GetUndefinedFinalSeparation Function to use the default value for
   * undefined final_separation
   *
   * @return The map of the effective final separation distance (unit
   * degree)
   */
  Map<double>* GetMapOfFinalSeparation(
      const double nan,
      lagrangian::FiniteLyapunovExponentsIntegration& fle) const {
    return GetMapOfExponents(
        nan, fle, &lagrangian::FiniteLyapunovExponents::get_final_separation,
        &lagrangian::FiniteLyapunovExponents::GetUndefinedFinalSeparation);
  }
};

}  // namespace lagrangian
