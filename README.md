Packaging and Building Information
==================================

Artikulate is a language learning application that helps improving pronunciation skills for
various languages. This repository maintains the application and language specifications. All course
files are maintained in a separate repository named "artikulate-data" and hosted on the KDE
infrastructure.

For a detailed explanation how to contribute new languages and/or new courses, please refer to the
information at techbase: http://techbase.kde.org/Projects/Edu/Artikulate

# Build Dependencies

Artikulate has the following build dependencies:

* KDElibs >= 4.11.1
* Qt >= 4.8 (but less than Qt 5.x)
* CMake >= 2.8.9
* QtGStreamer >= 1.1.90 (i.e., based on GStreamer-1.x)


# Packaging

Building produces the binary "artikulate" and the following shared libraries, which are only used
internally inside Artikulate:

* artikulatelearnerprofile
* artikuklatesound
* artikulatecore


# Runtime Dependencies

* kqtquickcharts: a KDE Edu library that provides a QtQuick plugin for plotting graphs
* KDE-runtime >= 4.11.1


# GStreamer-Problems

It is important that there is /no/ explicit or implicit linking to any gstreamer-0.10 base library.
Please ensure this, e.g., by executing "ldd artikulate" and by confirming that its output does not
contain any reference to gstreamer-0.10. Mixing both gstreamer-0.10 and gstreamer-1.0 will lead
to crashes and failures of all sound recording and playing functionality of the application.


# Licensing

The major part of the codebase is licensed as GPL-2+ with the additional KDE e.V. clause. If no
license is given, this is the default license (though please report missing licenses asap.)
Main parts of the above named libraries are licenses as LGL-2+ with the additional KDE e.V. clause.
CMake files are BSD licensed.


# Patches and Bug Reports

Please use the bugs.kde.org or the git.reviewboard.kde.org infrastructure for this.
