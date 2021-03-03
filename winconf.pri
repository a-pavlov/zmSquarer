
!isEmpty(TJPEG_ROOT) {
    LIBS += -L$$(TJPEG_ROOT)/build turbojpeg-static.lib
    INCLUDEPATH += $$(TJPEG_ROOT)
}

#VCINSTALLDIR=F:\dev\vc\2017\VC\
#windeployqt --verbose=1 --list=target --qmldir=c:\dev\zmSquarer\qml  zmSquarer.exe
