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
    NodeFactory.hpp \

INCLUDEPATH += ./boost_1_60_0/
INCLUDEPATH += ./TinyTestFramework/
INCLUDEPATH -= ./TinyTestFramework/main.cpp
