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

from gio import  AppInfo, FileIcon, ThemedIcon, content_type_get_icon
import gobject
import gtk
from gtk.gdk import INTERP_BILINEAR, pixbuf_new_from_file_at_size
from os import listdir
from os.path import exists, isdir, isfile, join

from unitylaunchereditor.core.constants import IMAGE_SIZE

class IconManager:
    def __init__(self):
        self.icontheme = gtk.icon_theme_get_default()
        # add the humanity icon theme to the iconpath, as not all icon 
        # themes contain all the icons we need
        # this *shouldn't* lead to any performance regressions
        path = '/usr/share/icons/Humanity'
        if exists(path):
            for subpath in listdir(path):
                subpath = join(path, subpath)
                if isdir(subpath):
                    for subsubpath in listdir(subpath):
                        subsubpath = join(subpath, subsubpath)
                        if isdir(subsubpath):
                            self.icontheme.append_search_path(subsubpath)
        
    def get_icon_with_type(self,filepath, size=24):
        icon = self.icontheme.lookup_icon(self.icontheme,IMAGE_SIZE , 0)
    
        try:
            pixbuf = self.icontheme.load_icon(icon[0], size, 0)
        except:
            return self.get_icon_with_name('image-missing', size)
    
        if pixbuf.get_height() != size:
            return pixbuf.scale_simple(size, size, INTERP_BILINEAR)
    
        return pixbuf

    def get_icon_with_cate(self,cate, size):
        pass
    
    def get_icon_with_name(self,name, size=24):
        try:
            pixbuf = self.icontheme.load_icon(name, size, 0)
        except:
            pixbuf = self.icontheme.load_icon('image-missing', size, 0)
    
        if pixbuf.get_height() != size:
            return pixbuf.scale_simple(size, size, INTERP_BILINEAR)
    
        return pixbuf
    
    def get_icon_with_file(self,file, size):
        try:
            return pixbuf_new_from_file_at_size(file, size, size)
        except:
            return self.get_icon_with_name('image-missing', size)

    def mime_type_get_icon(self,mime, size = 24):
        try:
            gicon = content_type_get_icon(mime)
            iconinfo = self.icontheme.choose_icon(gicon.get_names(), size, gtk.ICON_LOOKUP_USE_BUILTIN)
            if not iconinfo:
                pixbuf = self.get_icon_with_name('application-x-executable', size)
            else:
                pixbuf = iconinfo.load_icon()
    
            if pixbuf.get_width() != size:
                return pixbuf.scale_simple(size, size, INTERP_BILINEAR)
        except:
            return self.get_icon_with_name('image-missing', size)
    
        return pixbuf

    def get_icon(self, app, size):
        
        try:
            if isinstance(app, AppInfo):
                gicon = app.get_icon()
            else:
                gicon = app
            
    
            if gicon:
                if isinstance(gicon, ThemedIcon):
                    names = gicon.get_names()
                    names_list = []
                    for name in names:
                        if name.rfind('.') != -1:
                            names_list.append(name[:name.rfind('.')])
                        else:
                            names_list.append(name)
    
                    iconinfo = self.icontheme.choose_icon(names_list, size, gtk.ICON_LOOKUP_USE_BUILTIN)
                    if not iconinfo:
                        return self.get_icon_with_name('application-x-executable', size)
    
                    return iconinfo.load_icon()
                elif isinstance(gicon, FileIcon):
                    file = app.get_file().get_path()
                    return self.get_icon_with_file(file, size)
            else:
                return self.get_icon_with_name(app, size)
        except:
            return self.get_icon_with_name('image-missing', size)