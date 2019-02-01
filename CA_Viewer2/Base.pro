
TEMPLATE = app
TARGET = Base

QT += gui widgets opengl

CONFIG += qt debug_and_release c++11

Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR     = release/.moc
Debug:OBJECTS_DIR   = debug/.obj
Debug:MOC_DIR       = debug/.moc


win32-msvc2015{
        LIBS           += -lopengl32
        QMAKE_CXXFLAGS += /MP /Zi
        QMAKE_LFLAGS   += /MACHINE:X64
}

macx{
        QMAKE_SONAME_PREFIX = @executable_path/../Frameworks
        QMAKE_LFLAGS += -Wl,-rpath,@executable_path/../Frameworks
        QMAKE_LFLAGS   -= -mmacosx-version-min=10.8
        QMAKE_LFLAGS   += -mmacosx-version-min=10.9
        QMAKE_CXXFLAGS -= -mmacosx-version-min=10.8
        QMAKE_CXXFLAGS += -mmacosx-version-min=10.9
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
}

unix:!macx{
        CONFIG += C++11
}

mingw{
        LIBS += -lopengl32
}

# Input
HEADERS += glwidget.h mainwindow.h \
    trianglemesh.h \
    plyreader.h \
    GeometryP.h \
    Particle.h \
    cal3d/animation.h \
    cal3d/animation_action.h \
    cal3d/animation_cycle.h \
    cal3d/animcallback.h \
    cal3d/bone.h \
    cal3d/buffersource.h \
    cal3d/cal3d.h \
    cal3d/cal3d.rc \
    cal3d/cal3d_wrapper.h \
    cal3d/coreanimation.h \
    cal3d/corebone.h \
    cal3d/corekeyframe.h \
    cal3d/corematerial.h \
    cal3d/coremesh.h \
    cal3d/coremodel.h \
    cal3d/coremorphanimation.h \
    cal3d/coreskeleton.h \
    cal3d/coresubmesh.h \
    cal3d/coresubmorphtarget.h \
    cal3d/coretrack.h \
    cal3d/datasource.h \
    cal3d/error.h \
    cal3d/global.h \
    cal3d/hardwaremodel.h \
    cal3d/loader.h \
    cal3d/matrix.h \
    cal3d/mesh.h \
    cal3d/mixer.h \
    cal3d/model.h \
    cal3d/morphtargetmixer.h \
    cal3d/physique.h \
    cal3d/platform.h \
    cal3d/quaternion.h \
    cal3d/refcounted.h \
    cal3d/refptr.h \
    cal3d/renderer.h \
    cal3d/resource.h \
    cal3d/saver.h \
    cal3d/skeleton.h \
    cal3d/springsystem.h \
    cal3d/streamsource.h \
    cal3d/submesh.h \
    cal3d/tinyxml.h \
    cal3d/transform.h \
    cal3d/vector.h \
    modelC.h \
    tga.h
FORMS += mainwindow.ui
SOURCES += glwidget.cpp main.cpp mainwindow.cpp \
    trianglemesh.cpp \
    plyreader.cpp \
    GeometryP.cpp \
    Particle.cpp \
    cal3d/animation.cpp \
    cal3d/animation_action.cpp \
    cal3d/animation_cycle.cpp \
    cal3d/bone.cpp \
    cal3d/buffersource.cpp \
    cal3d/cal3d_wrapper.cpp \
    cal3d/coreanimation.cpp \
    cal3d/corebone.cpp \
    cal3d/corekeyframe.cpp \
    cal3d/corematerial.cpp \
    cal3d/coremesh.cpp \
    cal3d/coremodel.cpp \
    cal3d/coremorphanimation.cpp \
    cal3d/coreskeleton.cpp \
    cal3d/coresubmesh.cpp \
    cal3d/coresubmorphtarget.cpp \
    cal3d/coretrack.cpp \
    cal3d/error.cpp \
    cal3d/global.cpp \
    cal3d/hardwaremodel.cpp \
    cal3d/loader.cpp \
    cal3d/matrix.cpp \
    cal3d/mesh.cpp \
    cal3d/mixer.cpp \
    cal3d/model.cpp \
    cal3d/morphtargetmixer.cpp \
    cal3d/physique.cpp \
    cal3d/platform.cpp \
    cal3d/quaternion.cpp \
    cal3d/renderer.cpp \
    cal3d/saver.cpp \
    cal3d/skeleton.cpp \
    cal3d/springsystem.cpp \
    cal3d/streamsource.cpp \
    cal3d/submesh.cpp \
    cal3d/tinyxml.cpp \
    cal3d/tinyxmlerror.cpp \
    cal3d/tinyxmlparser.cpp \
    cal3d/vector.cpp \
    modelC.cpp \
    tga.cpp

DISTFILES += \
    shaders/simpleshader.vert \
    shaders/simpleshader.frag \
    cal3d/cal3d.dsp \
    cal3d/SConscript

RESOURCES += \
    resources.qrc

LIBS += -lopengl32
