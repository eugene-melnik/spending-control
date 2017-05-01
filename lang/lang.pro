 ###################################################################
#                                                                   #
#  SpendingControl                                                  #
#  Copyright (C) 2017 Eugene Melnik <jeka7js@gmail.com>             #
#                                                                   #
 ###################################################################

TEMPLATE = aux

OTHER_FILES = spending-control-ru_RU.ts \
              spending-control-uk_UA.ts

    # Generating application translations #

LRELEASE = $$[QT_INSTALL_BINS]/lrelease
win32:LRELEASE ~= s,/,\\,g

system( $$LRELEASE spending-control-ru_RU.ts -qm spending-control-ru_RU.qm )
system( $$LRELEASE spending-control-uk_UA.ts -qm spending-control-uk_UA.qm )

    # Qt translations #

COPY = cp

QT_RU_SRC = $$[QT_INSTALL_TRANSLATIONS]/qtbase_ru.qm
QT_RU_DST = $$PWD/qt-ru_RU.qm
QT_UK_SRC = $$[QT_INSTALL_TRANSLATIONS]/qtbase_uk.qm
QT_UK_DST = $$PWD/qt-uk_UA.qm

win32 {
    COPY = copy
    QT_RU_SRC ~= s,/,\\,g
    QT_RU_DST ~= s,/,\\,g
    QT_UK_SRC ~= s,/,\\,g
    QT_UK_DST ~= s,/,\\,g
}

system( $$COPY $$QT_RU_SRC $$QT_RU_DST )
system( $$COPY $$QT_UK_SRC $$QT_UK_DST )
