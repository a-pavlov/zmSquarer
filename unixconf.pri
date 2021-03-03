
!isEmpty(TJPEG_ROOT) {
    LIBS += -L$$(TJPEG_ROOT)/build -lturbojpeg
    INCLUDEPATH += $$(TJPEG_ROOT)
}

