# -*- coding: utf-8 -*-
#   Alacarte Menu Editor - Simple fd.o Compliant Menu Editor
#   Copyright (C) 2006  Travis Watkins
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Library General Public
#   License as published by the Free Software Foundation; either
#   version 2 of the License, or (at your option) any later version.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Library General Public License for more details.
#
#   You should have received a copy of the GNU Library General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

from os.path import isfile, join
import gtk
from ConfigParser import ConfigParser


class DesktopParser(ConfigParser):
    """XXX: Add docs."""

    def __init__(self, filename=None, file_type='Application'):
        ConfigParser.__init__(self)
        self.filename = filename
        self.file_type = file_type
        self.keys_menu = ['Desktop Entry']
        if filename:
            if not isfile(filename):
                self.filename = join('/usr/share/applications/', filename)
            if len(self.read(self.filename)) == 0:
                #file doesn't exist
                self.add_section('Desktop Entry')
        else:
            self.add_section('Desktop Entry')
        self._list_separator = ';'
        self.update_keys()

    def optionxform(self, option):
        # makes keys to not be lowercase
        return option
    def update_keys(self):
        for key in self.sections():
            if key != 'Desktop Entry'  and key != '':
                try:
                    self.keys_menu.append(key)
                except:
                    pass
                
    def append_section(self, keyname):
        ConfigParser.add_section(self,keyname)
        self.keys_menu.append(keyname)
        
    def remove_sections(self, keyname):
        self.remove_section(keyname)
        ConfigParser.remove_section(self,keyname)
        try:
            self.keys_menu.remove(keyname)
        except:
            #todo: log exception just ignoring it for now
            pass
        
    def get(self, option, strkey='Desktop Entry', locale=None):
        locale_option = option + '[%s]' % locale
        try:
            value = ConfigParser.get(self, strkey, locale_option)
        except:
            try:
                value = ConfigParser.get(self, strkey, option)
            except:
                return None
        if self._list_separator in value:
            value = value.split(self._list_separator)
        elif value == 'true':
            value = True
        elif value == 'false':
            value = False
        else:
            try:
                value = int(value)
            except:
                # It's not possible to tell when a string is a number or a
                # full string without a lot of test here test there. So this
                # will return an int only when it's convertible.
                pass

        return value

    def set(self, option, value, locale=None, strkey='Desktop Entry'):
        if locale:
            option = option + '[%s]' % locale
        if value == True:
            value = 'true'
        if value == False:
            value = 'false'
        if isinstance(value, tuple) or isinstance(value, list):
            value = self._list_separator.join(value) + ';'
        ConfigParser.set(self, strkey, option, value)

    def write(self, file_object):
        file_object.write('[Desktop Entry]\n')
        if not self.filename:
            file_object.write('Encoding=UTF-8\n')
            file_object.write('Type=' + str(self.file_type) + '\n')
            file_object.write('StartupNotify=true\n')
        for skey in self.keys_menu:
            items = []
            if skey != 'Desktop Entry':
                file_object.write('\n[%s]\n' % skey)
            for item in self.items(skey):
                if item[0] != 'Visible':
                    file_object.write(item[0] + '=' + item[1] + '\n')



