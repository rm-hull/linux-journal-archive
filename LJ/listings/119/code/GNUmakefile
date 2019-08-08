include $(GNUSTEP_MAKEFILES)/common.make

APP_NAME = TiffViewer

TiffViewer_OBJC_FILES = AppController.m ImageModel.m
TiffViewer_RESOURCE_FILES = TiffViewer.gsmarkup \
	GNUstep-Menu.gsmarkup

TiffViewer_LOCALIZED_RESOURCE_FILES = \
	Localizable.strings
TiffViewer_LANGUAGES = French

ADDITIONAL_GUI_LIBS += -lRenaissance
ADDITIONAL_OBJCFLAGS = -Wall -Wno-import


include $(GNUSTEP_MAKEFILES)/application.make
