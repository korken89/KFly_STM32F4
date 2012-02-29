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



import locale
import gettext

# application specific
APP = 'unity-launcher-editor'
DIR = '/usr/share/locale/'

locale.setlocale(locale.LC_ALL, '')
gettext.bindtextdomain(APP, DIR)
gettext.textdomain(APP)
_ = gettext.gettext

LOCALE = locale.getdefaultlocale()[0]
