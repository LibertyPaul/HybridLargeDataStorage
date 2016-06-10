TEMPLATE = app
CONFIG += console
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    HybridLargeDataStorage.hpp \
    BaseNode.hpp \
    Node.hpp \
    ValueNode.hpp \
    TailTreeIterator.hpp \
    TailTree.hpp \
    HLDSIterator.hpp \
    HeadsIterator.hpp \
    HeadsHolder.hpp \
    BranchHolder.hpp \
    Key.hpp

INCLUDEPATH += ./TinyTestFramework/
INCLUDEPATH -= ./TinyTestFramework/main.cpp
