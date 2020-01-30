#include "PackageError.h"


PackageError::PackageError(const std::string &what) : std::runtime_error(what) {

}

PackageParseError::PackageParseError(const std::string &what) : PackageError(what){

}

PackageControlSumError::PackageControlSumError(const std::string &what)  : PackageParseError(what)
{}