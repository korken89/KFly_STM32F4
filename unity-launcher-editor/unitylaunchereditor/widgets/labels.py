# -*- coding: utf-8 -*-
#####################################################################
#  Laudeci Oliveira <laudeci@ubuntu.com>
#  Ursula Junque <ursinha@ubuntu.com>
#  Original Code: UbuntuOne Developers
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
from gtk.gdk import Color
from pango import WRAP_WORD

DEFAULT_PADDING = (10, 10)

class TitleLabel(gtk.Label):
    """A box with a given color and text."""

    def __init__(self, markup='', fg_color=None, **kwargs):
        super(TitleLabel, self).__init__(fg_color, **kwargs)
        self.set_markup(markup)
        self.set_padding(*DEFAULT_PADDING)
        self.set_property('xalign', 0.0)
        self.set_property('ypad',10)
        self.set_alignment(0, 0)
        self.set_line_wrap(True)
        self.set_line_wrap_mode(WRAP_WORD)
        self.set_selectable(False)
        if fg_color:
            self.modify_fg(gtk.STATE_NORMAL, Color(fg_color))
        
        self.show_all()
        
class LoadingBox(gtk.HBox):
    """A spinner and a label."""

    def __init__(self, label, fg_color=None, *args, **kwargs):
        super(LoadingBox, self).__init__(*args, **kwargs)
        self.label = gtk.Label(label)
        self.spinner = gtk.Spinner()
        self.spinner.start()

        if fg_color is not None:
            self.spinner.modify_fg(gtk.STATE_NORMAL, Color(fg_color))
            self.label.modify_fg(gtk.STATE_NORMAL, Color(fg_color))

        self.pack_start(self.spinner, expand=False)
        self.pack_start(self.label, expand=False)
        self.set_spacing(5)

        self.show_all()


class LabelLoading(gtk.Alignment):
    """A spinner and a label."""

    def __init__(self, loading_label, fg_color=None, *args, **kwargs):
        super(LabelLoading, self).__init__(*args, **kwargs)
        self.loading = LoadingBox(loading_label, fg_color=fg_color)
        
        self.label = gtk.Label()
        self.label.set_selectable(True)
        self.label.show()
        if fg_color is not None:
            self.label.modify_fg(gtk.STATE_NORMAL, Color(fg_color))

        self.add(self.loading)

        self.show()
        self.set(xalign=0.5, yalign=0.5, xscale=0, yscale=0)
        self.set_padding(padding_top=5, padding_bottom=0,
                         padding_left=0, padding_right=5)
        self.start()

    @property
    def active(self):
        """Whether the Loading widget is visible or not."""
        return self.get_child() is self.loading

    def start(self):
        """Show the Loading instead of the Label widget."""
        for child in self.get_children():
            self.remove(child)

        self.add(self.loading)

    def stop(self):
        """Show the label instead of the Loading widget."""
        for child in self.get_children():
            self.remove(child)

        self.add(self.label)

    def set_text(self, text):
        """Set 'text' to be the label's text."""
        self.label.set_text(text)

    def set_markup(self, text):
        """Set 'text' to be the label's markup."""
        self.label.set_markup(text)

    def get_text(self):
        """Get the label's text."""
        return self.label.get_text()

    def get_label(self):
        """Get the label's markup."""
        return self.label.get_label()