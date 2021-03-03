
!isEmpty(TJPEG_ROOT) {
    LIBS += -L$$(TJPEG_ROOT)/build -lturbojpeg
    INCLUDEPATH += $$(TJPEG_ROOT)
}

QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic -Wformat-security
