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
import gobject
import gtk
from pango import ELLIPSIZE_END

from unitylaunchereditor.core.translation import _

class DialogHeader(gtk.VBox):
    __title__ = 'WARNING: can not get name'
    __desc__ = ' '

    def __init__(self, title = None):
        super(DialogHeader,self).__init__()
        self.set_border_width(0)
        if title:
            self.__title__ = title
        if self.__title__ and self.__desc__:
            self.__draw_title()

        self.inner_vbox = gtk.VBox(False, 0)
        self.inner_vbox.set_border_width(0)
        self.pack_start(self.inner_vbox)

    def set_title(self, title):
        self.__title__ = title
        self.label_title.set_markup('<b><big>%s</big></b>' % self.__title__)

    def set_description(self, description):
        self.__desc__ = description
        self.label_desc.set_text(self.__desc__)

    def set_icon(self, value):
        if isinstance(value, str):
            if value.rfind('.') != -1:
                self.image.set_from_file(value)
            else:
                self.image.set_from_icon_name(value, gtk.ICON_SIZE_DIALOG)
        else:
            self.image.set_from_pixbuf(value)
            
    def add_start(self, child, expand=True, fill=True, padding=0):
        self.inner_vbox.pack_start(child, expand, fill, padding)

    def add_end(self, child, expand=True, fill=True, padding=0):
        self.inner_vbox.pack_end(child, expand, fill, padding)

    def __draw_title(self):

        vbox = gtk.VBox()
        self.pack_start(vbox, False, False, 0)

        align = gtk.Alignment(0.5, 0.5, 1.0, 1.0)
        align.set_padding(5, 5, 5, 5)
        vbox.pack_start(align)

        hbox = gtk.HBox(False, 6)
        align.add(hbox)

        inner_vbox = gtk.VBox(False, 0)
        hbox.pack_start(inner_vbox)

        align = gtk.Alignment(0.5, 0.5, 1.0, 1.0)
        inner_vbox.pack_start(align, False, False, 0)

        inner_hbox = gtk.HBox(False, 0)
        align.add(inner_hbox)

        self.label_title = gtk.Label()
        self.label_title.set_markup('<b><big>%s</big></b>' % self.__title__)
        self.label_title.set_alignment(0, 0.5)
        inner_hbox.pack_start(self.label_title, False, False, 0)

        self.label_desc = gtk.Label(self.__desc__)
        self.label_desc.set_ellipsize(ELLIPSIZE_END)
        self.label_desc.set_alignment(0, 0.5)
        inner_vbox.pack_start(self.label_desc, False, False, 0)

        self.image =gtk.Image()
        self.image.set_from_icon_name('aptoncd', gtk.ICON_SIZE_DIALOG)
        self.image.set_alignment(0, 0)
        self.image.set_padding(5, 5)
        hbox.pack_end(self.image, False, False, 0)

class HeaderBoxButton(DialogHeader):
    __gsignals__ = { 'button-clicked': (
                                 gobject.SIGNAL_RUN_LAST,
                                 gobject.TYPE_NONE,
                                 (gobject.TYPE_INT,),
                                 ),
    }

    def __init__(self, title=None, color=None):
        super(HeaderBoxButton, self).__init__()
        if title:
            self.set_title(title)
            
        self.button_box = gtk.HBox()
        #------------------------------------
        hsep = gtk.HSeparator()
        #hsep.modify_bg(gtk.STATE_NORMAL, gtk.gdk.Color('#43413c'))
        
        hsep2 = gtk.HSeparator()
        #hsep2.modify_bg(gtk.STATE_NORMAL, gtk.gdk.Color(color))
        
        hbbox = gtk.HButtonBox()
        hbbox.set_layout(gtk.BUTTONBOX_CENTER)
        hbbox.set_spacing(-1)
        
        system_tab = gtk.RadioButton(group=None, label=_('From System'))
        file_tab = gtk.RadioButton(group=system_tab, label=_('From File'))
        custom_tab = gtk.RadioButton(group=system_tab, label=_('Custom'))

        system_tab.set_property('draw_indicator', False)
        system_tab.set_name('system_tab')
        file_tab.set_property('draw_indicator', False)
        file_tab.set_name('file_tab')
        custom_tab.set_property('draw_indicator', False)
        custom_tab.set_name('custom_tab')

        hbbox.pack_start(system_tab, False,False,-5)
        hbbox.pack_start(file_tab, False,False,-5)
        hbbox.pack_start(custom_tab, False,False,-5)

        self.button_box.pack_start(hsep)
        self.button_box.pack_start(hbbox, False,True,0)
        self.button_box.pack_start(hsep2)
        
        self.add_start(self.button_box, False,False)
        system_tab.connect('toggled', self._on_clicked, 0)
        file_tab.connect('toggled', self._on_clicked, 1)
        custom_tab.connect('toggled', self._on_clicked, 2)
        
        self.show_all()
        
    def _on_clicked(self,w,id):
        print getattr(w, 'name')
        self.emit('button-clicked',id)