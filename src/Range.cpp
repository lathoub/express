#include "Express.h"

BEGIN_EXPRESS_NAMESPACE

///
const Range &_Request::rangeParse(const String &str, const size_t &maxSize) {
  auto range_ = new Range();

  if (str.length() < 3)
    return *range_;

  auto index = str.indexOf('=');
  if (index < 0)
    return *range_;

  size_t sum = 0;

  range_->type = str.substring(0, index); // before = (type)
  auto ranges = str.substring(index + 1); // after =

  index = ranges.indexOf(',');
  while (index >= 0) {
    auto range = ranges.substring(0, index); // before ,
    ranges = ranges.substring(index + 1);    // after ,

    index = range.indexOf('-');
    if (index >= 0) {
      auto start = range.substring(0, index).toInt(); // before ,
      auto end = range.substring(index + 1).toInt();  // after ,

      // check if start is bigger than end of the previous
      if (range_->ranges.size() > 0)
        if (start <= range_->ranges.back().end)
          throw new _Error("volgende probleem");

      if (sum + (end - start + 1) >= maxSize) {
        end = (maxSize - sum + start - 1);
        range_->ranges.push_back({start, end});
        return *range_; // we have reached the max amount of bytes, leave here
      }

      sum += (end - start + 1);

      range_->ranges.push_back({start, end});
    }

    index = ranges.indexOf(',');
  }

  index = ranges.indexOf('-');
  if (index >= 0) {
    auto start = ranges.substring(0, index).toInt(); // before ,
    auto end = ranges.substring(index + 1).toInt();  // after ,
    if (end == 0)
      end = INT_MAX;

    if (sum + (end - start + 1) >= maxSize) {
      end = (maxSize - sum + start - 1);
    }

    range_->ranges.push_back({start, end});
  }

  return *range_;
}

END_EXPRESS_NAMESPACE
