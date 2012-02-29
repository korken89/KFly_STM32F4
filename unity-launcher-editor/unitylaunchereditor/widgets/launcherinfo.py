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
import gobject
from gio import File, FileIcon,ThemedIcon
from unitylaunchereditor.core.iconmanager import IconManager
from unitylaunchereditor.core.constants import IMAGE_SIZE
from unitylaunchereditor.dialogs.dialogs import FileChooser, MessageBox
from unitylaunchereditor.core.translation import _
from unitylaunchereditor.core.log import Logger

log = Logger('LauncherInfo')

class LauncherInfo(gtk.HBox):
    __gsignals__ = { 'field-changed': (
                                       gobject.SIGNAL_RUN_LAST, 
                                       gobject.TYPE_NONE,
                                       (gobject.TYPE_STRING,),
                                        ),
        }
    
    def __init__(self):
        super(LauncherInfo,self).__init__()
        self.set_border_width(12)
        self.__command=""
        self.__name=""
        self.__icon = ""
        self.__comment = ""
        self.__terminal = False
        
        self.icon_manager = IconManager()
        self.__create_image_frame()
        self.__create_text_frame()
        
    @property
    def launcher_command(self):
        return self.__command
    @launcher_command.setter
    def launcher_command(self,value):
        self.__command = value
    
    @property
    def launcher_comment(self):
        return self.__comment
    @launcher_comment.setter
    def launcher_comment(self,value):
        self.__comment = value    
              
    @property
    def launcher_name(self):
        return self.__name
    @launcher_name.setter
    def launcher_name(self,value):
            self.__name= value
    @property
    def launcher_icon_name(self):
        return self.__icon
    @launcher_icon_name.setter
    def launcher_icon_name(self,value):
        self.__icon = value
        if value.rfind('.')!= -1:
            pixbuf = self.icon_manager.get_icon(FileIcon(File(value)),96)
        else:
            pixbuf = self.icon_manager.get_icon(ThemedIcon(value), 96)
        self.imgIcon.set_from_pixbuf(pixbuf)
    
    @property
    def launcher_run_terminal(self):
        return self.__terminal
    @launcher_run_terminal.setter
    def launcher_run_terminal(self,value):
            self.__terminal = value
    
    def __create_image_frame(self):
                
        label = gtk.Label(_('Icon:'))
        label.set_alignment(xalign=0, yalign=0)
        
        #image button
        self.imgIcon = gtk.Image()
        self.imgIcon.set_from_pixbuf(self.icon_manager.get_icon(ThemedIcon('image-missing'), 96))

        self.cmdIcon = gtk.Button()
        #self.cmdIcon.set_use_action_appearence(False)
        self.cmdIcon.set_use_action_appearance(False)
        self.cmdIcon.set_size_request(96, 96)
        self.cmdIcon.add(self.imgIcon)
        self.cmdIcon.connect('clicked', self.__load_icon_from_file)
        
        vbox = gtk.VBox()
        vbox.pack_start(self.cmdIcon, expand=False, fill=False, padding=0)
        
        
        hbox = gtk.HBox()
        hbox.pack_start(label, expand=False, fill=False, padding=0)
        hbox.pack_start(vbox, expand=False, fill=False, padding=0)
        hbox.set_spacing(3)
        
        alignent = gtk.Alignment()
        alignent.set_padding(padding_left=12,padding_top=0,padding_bottom=0,padding_right=12)
        alignent.add(hbox)
        
        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_NONE)
        frame.set_label_align(xalign=0, yalign=0)
        
        frame.add(alignent)
        self.pack_start(frame, expand=False, fill=True, padding=0)
    
    def __create_text_frame(self):
        label_name = gtk.Label(_('Launcher Name:'))
        label_name.set_alignment(xalign=0, yalign=0)
        self.entry_name = gtk.Entry()
        self.entry_name.connect('focus-out-event', self.__on_txt_focus_out, 'launcher_name')
        self.entry_name.connect('changed', self._on_change)
        
        label_command = gtk.Label(_('Command:'))
        label_command.set_alignment(xalign=0, yalign=0)
        self.entry_command = gtk.Entry()
        self.entry_command.connect('focus-out-event', self.__on_txt_focus_out, 'launcher_command')
        self.entry_command.connect('changed', self._on_change)
        
        button_command = gtk.Button('...')
        hbox = gtk.HBox()
        hbox.pack_start(self.entry_command)
        hbox.pack_start(button_command, expand=False, fill=True, padding=0)
        button_command.connect('clicked', self.__do_choose_run, self.entry_command)
        
        label_comment = gtk.Label(_('Comment:'))
        label_comment.set_alignment(xalign=0, yalign=0)
        self.entry_comment = gtk.TextView()
        self.entry_comment.connect('focus-out-event', self.__on_txt_focus_out, 'launcher_comment')
        scrolled = gtk.ScrolledWindow()
        scrolled.set_shadow_type(gtk.SHADOW_ETCHED_IN)
        scrolled.add(self.entry_comment)
        
        self.chkTerminal = gtk.CheckButton()
        self.chkTerminal.set_label(_('Run in terminal'))
        self.chkTerminal.connect('toggled', self.__on_txt_focus_out, None,'launcher_run_terminal')

        table = gtk.Table(rows=4, columns=2, homogeneous=False)
        table.set_col_spacings(5)
        table.set_row_spacings(5)
        
        table.attach(label_name, 
                     left_attach=0, 
                     right_attach=1, 
                     top_attach=0, 
                     bottom_attach=1, 
                     xoptions=gtk.FILL, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)

        table.attach(self.entry_name, 
                     left_attach=1, 
                     right_attach=2, 
                     top_attach=0, 
                     bottom_attach=1, 
                     xoptions=gtk.FILL|gtk.EXPAND , 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0
                     )
        
        table.attach(label_command, 
                     left_attach=0, 
                     right_attach=1, 
                     top_attach=1, 
                     bottom_attach=2, 
                     xoptions=gtk.FILL, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)
        
        table.attach(hbox, 
                     left_attach=1, 
                     right_attach=2, 
                     top_attach=1, 
                     bottom_attach=2, 
                     xoptions=gtk.FILL|gtk.EXPAND , 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)
        
        table.attach(label_comment, 
                     left_attach=0, 
                     right_attach=1, 
                     top_attach=2, 
                     bottom_attach=3, 
                     xoptions=gtk.FILL, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)
        
        table.attach(scrolled, 
                     left_attach=1, 
                     right_attach=2, 
                     top_attach=2, 
                     bottom_attach=3, 
                     xoptions=gtk.FILL|gtk.EXPAND, 
                     yoptions=gtk.FILL|gtk.EXPAND, 
                     xpadding=0, 
                     ypadding=0)
        
        table.attach(gtk.Label(), 
                     left_attach=0, 
                     right_attach=1, 
                     top_attach=3, 
                     bottom_attach=4, 
                     xoptions=gtk.FILL, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)
        
        table.attach(self.chkTerminal, 
                     left_attach=1, 
                     right_attach=2, 
                     top_attach=3, 
                     bottom_attach=4, 
                     xoptions=gtk.FILL|gtk.EXPAND, 
                     yoptions=gtk.FILL, 
                     xpadding=0, 
                     ypadding=0)
        
      
        #alignent = gtk.Alignment()
        #alignent.set_padding(padding_left=12,padding_top=0,padding_bottom=0,padding_right=0)
        #alignent.add(table)
        
        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_NONE)
        frame.set_label_align(xalign=0, yalign=0)
        frame.add(table)
        
        self.pack_start(frame, expand=True, fill=True, padding=0)
    
    def __load_icon_from_file(self, widget):
        # XXX: Here maybe we could make a list with all mimetypes and patterns
        # somewhere else and just iterate over them here.
        filter = gtk.FileFilter()
        filter.set_name("Images")
        filter.add_mime_type("image/png")
        filter.add_mime_type("image/jpeg")
        filter.add_mime_type("image/gif")
        filter.add_mime_type("image/svg+xml")
        filter.add_pattern("*.png")
        filter.add_pattern("*.jpg")
        filter.add_pattern("*.gif")
        filter.add_pattern("*.tif")
        filter.add_pattern("*.xpm")
        filter.add_pattern("*.svg")

        dialog = FileChooser(filters=filter)
        response, filename = dialog.run()
        if response:
            self.launcher_icon_name = filename
            
    def _on_change(self, widget):
        self.emit('field-changed',widget.get_text)
        
    def __on_txt_focus_out(self, widget, event, prop_name):
        ilength = 0
        if  isinstance(widget, gtk.TextView):
            buffer = widget.get_buffer()
            value = buffer.get_text(buffer.get_start_iter(),
                buffer.get_end_iter())
            ilength = len(value)
        elif isinstance(widget, gtk.CheckButton):
            print getattr(self, prop_name)
            value = widget.get_active()
            ilength = 1
        else:
            ilength = widget.get_text_length() 
            value = widget.get_text()
        
        old = getattr(self, prop_name)
        
        if ilength > 0 and old != value:
            setattr(self, prop_name, value)
            
            
    def __do_choose_run(self, widget, entry_widget):
        dialog = FileChooser()
        response, filename = dialog.run()
        if response:
            self.launcher_command=filename
            if entry_widget:
                entry_widget.set_text(filename)