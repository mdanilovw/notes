#ifndef _RETURN_CODE_HPP_
#define _RETURN_CODE_HPP_

/**
 * Defines function return codes
 */
enum class ReturnCode {
    OK,
    INVALID_PASSWORD,
    WRONG_PASSWORD,
    GENERIC_ERROR,
    NOT_FOUND,
    EMPTY
};

#endif
