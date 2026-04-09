#ifndef MEASUREMENT_HPP
#define MEASUREMENT_HPP
#include <string>
#include <map>

class Measurement {
private: 
  double values;
  double uncertain;
  std::map<std::string, int> units;
  std::string unitsToString() const;
  static std::string sciNotation(double x);

public:
  Measurement(double values, double uncertain, const std::string &unit);
  std::string ToString() const;
  Measurement Add(const Measurement &item) const;
  Measurement Subtract(const Measurement &item) const;
  Measurement Multiply(const Measurement &item) const;
  Measurement Divide(const Measurement &item) const;
  Measurement RaisedToPower(int num) const;
};
#endif
