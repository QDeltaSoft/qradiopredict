TEMPLATE = subdirs

SUBDIRS += MapGraphics \
    FGPropagationTool

FGPropagationTool.depends += MapGraphics

