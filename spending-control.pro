TEMPLATE = subdirs

SUBDIRS = lang src tests
src.depends = lang
tests.depends = src

OTHER_FILES += README.md
