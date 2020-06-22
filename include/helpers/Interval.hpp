#pragma once

template<typename T> class Interval {
  T min_, max_;
  constexpr static T two_{2};

public:
  Interval(T min, T max) : min_{min}, max_{max} { assert(max_ >= min_); }
  static Interval fromCentredSize(T centre, T size) {
    const T top{centre - size / two_};
    return {top, top + size};
  }

  T min() const { return min_; }
  T max() const { return max_; }
  T difference() const { return max_ - min_; }

  Interval& unify(const Interval& other) {
    min_ = std::min(min_, other.min_), max_ = std::max(max_, other.max_);
    return *this;
  }
  Interval unified(const Interval& other) {
    Interval output{*this};
    output.unify(other);
    return output;
  }
};