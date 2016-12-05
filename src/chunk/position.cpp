#include <cassert>
#include "position.h"
#include "chunk.h"

address_t RelativePosition::get() const {
    assert(object != nullptr);
    assert(object->getParent() != nullptr);
    return object->getParent()->getPosition()->get() + offset;
}

void RelativePosition::set(address_t value) {
    assert(object != nullptr);
    assert(object->getParent() != nullptr);
    assert(value >= object->getParent()->getPosition()->get());
    setOffset(value - object->getParent()->getPosition()->get());
}

address_t CachedRelativePosition::get() const {
    if(cache.isValid()) return cache.get();

    auto value = RelativePosition::get();
    cache.set(value);
    return value;
}

void CachedRelativePosition::set(address_t value) {
    RelativePosition::set(value);
    cache.set(value);
}

void CachedRelativePosition::setOffset(address_t offset) {
    RelativePosition::setOffset(offset);
    cache.invalidate();
}

void ComputedSize::adjustBy(diff_t add) {
    assert(static_cast<diff_t>(size + add) >= 0);
    size += add;
}

bool Range::contains(address_t point) const {
    return point >= _start && point < getEnd();
}
bool Range::contains(const Range &other) const {
    return other._start >= _start && other.getEnd() <= getEnd();
}
bool Range::overlaps(address_t point) const {
    return contains(point);
}
bool Range::overlaps(const Range &other) const {
    return !(other._start >= getEnd() || other.getEnd() <= _start);
}

bool Range::operator < (const Range &other) const {
    if(_start < other._start) return true;
    if(_start == other._start) {
        if(getEnd() < other.getEnd()) return true;
    }

    return false;
}
bool Range::operator == (const Range &other) const {
    return _start == other._start && _size == other._size;
}

Range Range::fromPoint(address_t point) {
    return std::move(Range(point, 1));
}
Range Range::fromEndpoints(address_t start, address_t end) {
    assert(end >= start);
    return std::move(Range(start, end - start));
}
