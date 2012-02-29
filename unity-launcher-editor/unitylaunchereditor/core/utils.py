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
import re 
from gobject import GError
from os.path import basename, exists, expanduser, isfile, join

def normalize_path(str_path, exist_only = False):
        file_name = basename(str_path)
        str_path = '%s/.local/share/applications/%s' % (expanduser('~'), file_name)
        if not isfile(str_path) and exist_only:
            str_path = join('/usr/share/applications/', file_name)
        return str_path

def remove_html_tags(data):
    p = re.compile(r'<.*?>')
    return p.sub('', data)

def nullstring(text):
    if text is None:
        text = ''
    return text

