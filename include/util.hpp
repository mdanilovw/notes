/**
 * Utility functions
 */
#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <functional>
#include <termios.h>

namespace Util  {
    static struct termios SAVED_TERM_ATTR;

    template<typename T>
    using Pred = std::function<bool(const T&)>;

    /**
     * Perform logical "and" operation for two predicates
     * 
     * @param p1, p2 Predicates to be combined 
     * @return Predicate which returns true if p1 and p2 return true
     */
    template<typename X>
    Pred<X> AND(Pred<X> p1, Pred<X> p2) {
    return [p1, p2](const X &x){return p1(x) && p2(x);};
    }

    /**
     * Inverts predicate behavior
     * 
     * @param p Predicate to be inverted
     * @return Predicate which returns opposite than p's result
     */
    template<typename X>
    Pred<X> INV(Pred<X> p) {
        return [p](const X &x){return !p(x);};
    }

    /**
     * Enable echoing chars in terminal
     */
    void SetTerminalNormalInputMode();

    /**
     * Disable echoing chars in terminal
     */
    void SetTerminalPasswordInputMode();
}

#endif // Util
