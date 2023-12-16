#include <dorset-lang/Utils/Version.h>

namespace Dorset
{
    version getVersion()
    {
        return version{0, 2, 1, semver::prerelease::alpha, 0};
    }
}
