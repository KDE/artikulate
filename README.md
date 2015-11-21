Artikulate
==========

Artikulate is a language learning application that helps improving pronunciation skills for
various languages. This repository maintains the application and language specifications. All course
files are maintained in a separate repository named "artikulate-data" and hosted on the KDE
infrastructure.

For a detailed explanation how to contribute new languages and/or new courses, please refer to the
information at techbase: http://techbase.kde.org/Projects/Edu/Artikulate

* contribute: please go to phabricator.kde.org

Packaging and Building Information
==================================

# Build Dependencies

All build dependencies are defined in the CMake files. Please run CMake and and consider the
the output.

# Packaging

Building produces the binary "artikulate" and the following shared libraries, which are only used
internally inside Artikulate:

* artikulatelearnerprofile
* artikuklatesound
* artikulatecore


# Runtime Dependencies

* Breeze icons 

# Licensing

The major part of the codebase is licensed as GPL-2+ with the additional KDE e.V. clause. If no
license is given, this is the default license (though please report missing licenses asap.)
Main parts of the above named libraries are licenses as LGL-2+ with the additional KDE e.V. clause.
CMake files are BSD licensed.


# Patches and Bug Reports

Please use the bugs.kde.org or the git.reviewboard.kde.org infrastructure for this.

