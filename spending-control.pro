TEMPLATE = subdirs

SUBDIRS = lang src tests
src.depends = lang
tests.depends = src
