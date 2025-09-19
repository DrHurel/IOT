#pragma once

/*
 * Author: $AUTHOR$
 * Creation date: $DATE$
 */

namespace $NAMESPACE$ {
class $CLASS_NAME$ {
   public:
    $CLASS_NAME$() noexcept = default;
    virtual ~$CLASS_NAME$() noexcept = default;
    $CLASS_NAME$($CLASS_NAME$ const&) noexcept = delete;
    $CLASS_NAME$($CLASS_NAME$&&) noexcept = delete;
    $CLASS_NAME$& operator=($CLASS_NAME$ const&) noexcept = delete;
    $CLASS_NAME$& operator=($CLASS_NAME$&&) noexcept = delete;
    virtual void do_stuff() const = 0;
};
} // namespace $NAMESPACE$
