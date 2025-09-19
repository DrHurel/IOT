#pragma once

/*
 * Author: $AUTHOR$
 * Creation date: $DATE$
 */

namespace $NAMESPACE$ {
template <typename Type> class $CLASS_NAME$ final {
public:
    $CLASS_NAME$() noexcept = default;
    ~$CLASS_NAME$() noexcept = default;
    $CLASS_NAME$($CLASS_NAME$ const&) noexcept = delete;
    $CLASS_NAME$($CLASS_NAME$&&) noexcept = delete;
    $CLASS_NAME$& operator=($CLASS_NAME$ const&) noexcept = delete;
    $CLASS_NAME$& operator=($CLASS_NAME$&&) noexcept = delete;
    void do_stuff() const { }
};
} // namespace $NAMESPACE$
