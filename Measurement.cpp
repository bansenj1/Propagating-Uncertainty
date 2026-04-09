#include "Measurement.hpp"
#include <sstream>
#include <cmath>
#include <algorithm>
#include <stdexcept>

std::string Measurement::sciNotation(double x) {
  if (x == 0.0) {
    return "0.00e+00";
  }
  int exponent = static_cast<int>(std::floor(std::log10(std::fabs(x))));
  double mantissa = x / std::pow(10.0, exponent);
  mantissa = std::round(mantissa * 100.0) / 100.0;

  if (std::fabs(mantissa) >= 10.0) {
    mantissa /= 10.0;
    exponent += 1;
  }
  std::ostringstream ss;
  ss << mantissa;
  std::string s = ss.str();
  if (s.find('.') == std::string::npos) {
    s += ".00";
  }
  else {
    int after = (int)(s.size() - s.find('.') - 1);
    if (after < 2) {
      s.append(2 - after, '0');
    }
    else if (after > 2) s = s.substr(0, s.find('.') + 3);
  }
  std::ostringstream y;
  y << "e" << (exponent >= 0 ? "+" : "-")
    << (std::abs(exponent) < 10 ? "0" : "")
    << std::abs(exponent);

  return s + y.str();
}


Measurement::Measurement(double a, double b, const std::string& unit) 
  : values(a), uncertain(b) {
  if (!unit.empty()) {
    units[unit] = 1;
  }
}


std::string Measurement::unitsToString() const {
  if (units.empty()) return "";
  std::ostringstream ss;
  for (auto &p : units) {
    if (p.second == 1) {
      ss << p.first << " ";
    }
    else {
      ss << p.first << "^" << p.second << " ";
    }
  }
  return ss.str();
}


std::string Measurement::ToString() const {
  return sciNotation(values) + " +- " + sciNotation(uncertain) + " " + unitsToString();
}


Measurement Measurement::Add(const Measurement &item) const {
  if (units != item.units) {
    throw std::invalid_argument("Unit mismatch");
  }
  double a = values + item.values;
  double b = std::sqrt(uncertain * uncertain + item.uncertain * item.uncertain);
  Measurement z(a, b, "");
  z.units = units;
  return z;
}

Measurement Measurement::Subtract(const Measurement &item) const {
  if (units != item.units) {
    throw std::invalid_argument("Unit mismatch");
  }
  double a = values - item.values;
  double b = std::sqrt(uncertain * uncertain + item.uncertain * item.uncertain);
  Measurement z(a, b, "");
  z.units = units;
  return z;
}


Measurement Measurement::Multiply(const Measurement &item) const {
  double a = values * item.values;
  double eq1 = uncertain / values;
  double eq2 = item.uncertain / item.values;
  double b = std::abs(a) * std::sqrt(eq1 * eq1 + eq2 * eq2);
  Measurement z(a, b, "");
  z.units = units;
  for (auto &p : item.units) {
    z.units[p.first] += p.second;
    if (z.units[p.first] == 0) {
      z.units.erase(p.first);
    }
  }
  return z;
}


Measurement Measurement::Divide(const Measurement &item) const {
  double a = values / item.values;
  double eq1 = uncertain / values;
  double eq2 = item.uncertain / item.values;
  double b = std::abs(a) * std::sqrt(eq1 * eq1 + eq2 * eq2);
  Measurement z(a, b, "");
  z.units = units;
  for (auto &p : item.units) {
    z.units[p.first] -= p.second;
    if (z.units[p.first] == 0) {
      z.units.erase(p.first);
    }
  }
  return z;
}


Measurement Measurement::RaisedToPower(int num) const {
  double a = std::pow(values, num);
  double b = std::abs(num * std::pow(values, num - 1) * uncertain);
  Measurement z(a, b, "");
  for (auto &p : units) {
    z.units[p.first] = p.second * num;
  }
  for (auto it = z.units.begin(); it != z.units.end();) {
    if (it -> second == 0) {
      it = z.units.erase(it);
    } 
    else {
      ++it;
    }
  }
  return z;
}
