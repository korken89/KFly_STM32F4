# -*- coding: utf-8 -*-
#####################################################################
#  Laudeci Oliveira <laudeci@ubuntu.com>
#  Ursula Junque <ursinha@ubuntu.com>
#
#  Copyright 2011 unity-launcher-editor-dev.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published
#  by the Free Software Foundation; version 3 only.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#####################################################################

import gtk
from gobject import GError
from os.path import isfile


def get_icon(filename, size=48):
    image_size = size
    image = gtk.Image()
    icon_theme = gtk.icon_theme_get_default()

    if isfile(filename):
        image.set_from_file(filename)
        icon = image.get_pixbuf()
        # resize the image to 48 pixels
        if icon:
            icon = icon.scale_simple(image_size, image_size,
                gtk.gdk.INTERP_BILINEAR)
    else:
        try:
            # to fix skype error loading a named icon like skype.png and no
            # skype
            icon = icon_theme.load_icon(filename.split('.')[0], image_size, 0)
        except:
            # returns a default icon, which unity uses when no icon is found.
            try:
                icon = icon_theme.load_icon('twf', image_size, 0)
            except GError:
                # icon not found, defaulting to the last fallback
                icon = icon_theme.load_icon('image-missing', image_size, 0)
    return icon


def nullstring(text):
    if text is None:
        text = ''
    return text


class DialogType:
    """ Class to define constants for define Message class types"""
    (
        DIALOG_INPUT,
        DIALOG_MESSAGE,
        DIALOG_CONFIRMATION
    ) = range(3)

class FileChooser(gtk.FileChooserDialog):
    def __init__(self, title='Open...', filters=None):
        super(FileChooser, self).__init__(
                                    title,
                                    None,
                                    gtk.FILE_CHOOSER_ACTION_OPEN, (
                                   gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                   gtk.STOCK_OPEN, gtk.RESPONSE_OK)
                                    )
        self.set_default_response(gtk.RESPONSE_OK)
        if filters:
            self.add_filter(filters)

    def run(self, destroy=True):
        """Returns True if yes was clicked, False otherwise."""
        resp = super(FileChooser, self).run()

        filename = self.get_filename()
        if destroy:
            self.destroy()
        if resp == gtk.RESPONSE_OK:
            return True, filename
        else:
            return False, None
            
class MessageBox(gtk.MessageDialog):
    def __init__(self, parent,
                 window_title,
                 action_text,
                 message,
                 dlg_type=DialogType.DIALOG_MESSAGE,
                 label_text=None):
        
        text = "<b>%s</b>\n\n%s" % (action_text, message)
        
        self.dialog_type = dlg_type
        
        if dlg_type == DialogType.DIALOG_MESSAGE:
            icon = gtk.MESSAGE_WARNING
            buttons = gtk.BUTTONS_OK 
        elif dlg_type == DialogType.DIALOG_INPUT:
            icon = gtk.MESSAGE_QUESTION
            buttons = gtk.BUTTONS_CANCEL
        elif dlg_type == DialogType.DIALOG_CONFIRMATION:
            icon = gtk.MESSAGE_QUESTION
            buttons = gtk.BUTTONS_YES_NO
            
        super(MessageBox, self).__init__(parent,
                                         gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,
                                         icon, buttons)
        self.set_title(window_title)
        self.set_markup(text)
        self.set_transient_for(parent)
        if dlg_type == DialogType.DIALOG_INPUT:
            
            # create a horizontal box to pack the entry and a label
            self.text = gtk.Entry()
            self.text.set_activates_default(gtk.TRUE)
            hbox = gtk.HBox()
            hbox.pack_start(gtk.Label(label_text), False, 5, 5)
            hbox.pack_end(self.text)
            self.vbox.pack_end(hbox, True, True, 0)
            self.ok_button = self.add_button(gtk.STOCK_OK, gtk.RESPONSE_OK)
            self.ok_button.grab_default()
           
            self.ok_button.set_sensitive(False)
            self.set_alternative_button_order([gtk.RESPONSE_CANCEL, gtk.RESPONSE_OK])
            self.set_default_response(gtk.RESPONSE_OK)
            self.text.connect('changed', lambda widget: self.ok_button.set_sensitive(widget.get_text_length() > 0))
            
        self.show_all()
    
    def run(self, destroy=True):
        """Returns True if yes was clicked, False otherwise."""
        resp = super(MessageBox, self).run()
        dialog_type = self.dialog_type
        text = None
        if self.dialog_type == DialogType.DIALOG_INPUT:
            text = self.text.get_text()
        if destroy:
            self.destroy()
        if resp == gtk.RESPONSE_YES or resp == gtk.RESPONSE_OK:
            if dialog_type == DialogType.DIALOG_INPUT:
                return True, text
            else:
                return True, text
        else:
            return False, text
