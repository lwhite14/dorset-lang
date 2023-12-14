#include <dorsetDriver/Utils/Version.h>

version getVersion()
{
    return version{0, 2, 0, semver::prerelease::alpha, 0};
}
