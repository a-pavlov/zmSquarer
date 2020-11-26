INCLUDEPATH += $$PWD

HEADERS +=  $$PWD/logger.h \
            $$PWD/motion.h \
            $$PWD/config.h \
            $$PWD/conf.h \
            $$PWD/util.h \
            $$PWD/translate.h\
            $$PWD/netcam.h\
            $$PWD/netcam_http.h\
            $$PWD/netcam_jpeg.h\
            $$PWD/netcam_wget.h\
            $$PWD/netcam_ftp.h\
            $$PWD/rotate.h

#            $$PWD/alg.h

SOURCES +=  $$PWD/logger.c \
            $$PWD/conf.c \
            $$PWD/util.c \
            $$PWD/translate.c \
            $$PWD/motion.c\
            $$PWD/netcam.c\
            $$PWD/netcam_http.c\
            $$PWD/netcam_jpeg.c\
            $$PWD/netcam_wget.c\
            $$PWD/netcam_ftp.c\
            $$PWD/rotate.c
#            $$PWD/alg.c

