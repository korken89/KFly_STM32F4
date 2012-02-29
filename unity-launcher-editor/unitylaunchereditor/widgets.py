# -*- coding: utf-8 -*-

# Authors: Natalia B. Bidart <nataliabidart@canonical.com>
# Authors: Evan Dandrea <evan.dandrea@canonical.com>
#
# Copyright 2009-2010 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranties of
# MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
# PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.

"""A set of useful widgets."""

from os.path import expanduser, join
import gobject
import gtk
from pango import WRAP_WORD

from gio import app_info_get_all,ThemedIcon, AppInfo

from unitylaunchereditor.core.iconmanager import IconManager
from unitylaunchereditor.core.util.dialogs import MessageBox
from unitylaunchereditor.core.translation import _
from unitylaunchereditor.widgets.launcherview import LauncherView
from unitylaunchereditor.widgets.launcherinfo import LauncherInfo
from unitylaunchereditor.addassistant import PageFromSystem, PageFromFile

DEFAULT_PADDING = (10, 10)


class Loading(gtk.HBox):
    """A spinner and a label."""

    def __init__(self, label, fg_color=None, *args, **kwargs):
        super(Loading, self).__init__(*args, **kwargs)
        self.label = gtk.Label(label)
        self.spinner = gtk.Spinner()
        self.spinner.start()

        if fg_color is not None:
            self.spinner.modify_fg(gtk.STATE_NORMAL, gtk.gdk.Color(fg_color))
            self.label.modify_fg(gtk.STATE_NORMAL, gtk.gdk.Color(fg_color))

        self.pack_start(self.spinner, expand=False)
        self.pack_start(self.label, expand=False)
        self.set_spacing(5)

        self.show_all()


class LabelLoading(gtk.Alignment):
    """A spinner and a label."""

    def __init__(self, loading_label, fg_color=None, *args, **kwargs):
        super(LabelLoading, self).__init__(*args, **kwargs)
        self.loading = Loading(loading_label, fg_color=fg_color)

        self.label = gtk.Label()
        self.label.set_selectable(True)
        self.label.show()
        if fg_color is not None:
            self.label.modify_fg(gtk.STATE_NORMAL, gtk.gdk.Color(fg_color))

        self.add(self.loading)

        self.show()
        self.set(xalign=0.5, yalign=0.5, xscale=0, yscale=0)
        self.set_padding(padding_top=5, padding_bottom=0,
                         padding_left=5, padding_right=5)
        self.start()

    @property
    def active(self):
        """Whether the Loading widget is visible or not."""
        return self.gLabelLoadinget_child() is self.loading

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


class PanelTitle(gtk.Label):
    """A box with a given color and text."""

    def __init__(self, markup='', fg_color=None, **kwargs):
        super(PanelTitle, self).__init__(fg_color, **kwargs)
        self.set_markup(markup)
        self.set_padding(*DEFAULT_PADDING)
        self.set_property('xalign', 0.0)
        self.set_line_wrap(True)
        self.set_line_wrap_mode(WRAP_WORD)
        self.set_selectable(False)
        if fg_color:
            self.modify_fg(gtk.STATE_NORMAL, gtk.gdk.Color(fg_color))
        
        self.show_all()

def on_size_allocate(widget, allocation, label):
    """Resize labels according to who 'widget' is being resized."""
    label.set_size_request(allocation.width - 2, -1)

class UbuntuOneBin(gtk.VBox):
    """A Ubuntu One bin."""

    TITLE = ''

    def __init__(self, title=None):
        gtk.VBox.__init__(self)
        self._is_processing = False

        if title is None:
            title = self.TITLE

        title = '<span font_size="large" color="white">%s</span>' % title
        self.title = PanelTitle(markup=title)
        self.pack_start(self.title, expand=False)

        self.message = LabelLoading('loading...')
        #self.pack_start(self.message, expand=False)

        self.connect('size-allocate', on_size_allocate, self.title)
        
        self.show_all()
    def set_title(self,value):
        self.title.set_markup('<span font_size="large" color="white">%s</span>' % value)
    def _get_is_processing(self):
        """Is this panel processing a request?"""
        return self._is_processing

    def _set_is_processing(self, new_value):
        """Set if this panel is processing a request."""
        if new_value:
            self.message.start()
            self.set_sensitive(False)
        else:
            self.message.stop()
            self.set_sensitive(True)

        self._is_processing = new_value

    is_processing = property(fget=_get_is_processing, fset=_set_is_processing)

class Doit(gtk.HBox):
    def __init__(self):
        super(Doit, self).__init__()
        hsep = gtk.HSeparator()
        hsep.modify_bg(gtk.STATE_NORMAL, gtk.gdk.Color('#43413c'))
        
        hsep2 = gtk.HSeparator()
        hsep2.modify_bg(gtk.STATE_NORMAL, gtk.gdk.Color('#43413c'))
        
        hbbox = gtk.HButtonBox()
        hbbox.set_layout(gtk.BUTTONBOX_SPREAD)
        hbbox.set_spacing(-1)
        
        self.grad1 = gtk.RadioButton(group=None, label="From System")
        self.grad2 = gtk.RadioButton(group=self.grad1, label="From File")
        self.grad3 = gtk.RadioButton(group=self.grad1, label="Custom")

        self.grad1.set_property('draw_indicator', False)
        self.grad2.set_property('draw_indicator', False)
        self.grad3.set_property('draw_indicator', False)

        hbbox.pack_start(self.grad1, False,False,-5)
        hbbox.pack_start(self.grad2, False,False,-5)
        hbbox.pack_start(self.grad3, False,False,-5)

        
        self.pack_start(hsep)
        self.pack_start(hbbox, False,True,0)
        self.pack_start(hsep2)
    
def change_page(widget,note, id, u1):
    messages = {
        0:"Create a Launcher from System",
        1:"Create a Launcher from file",
        2:"Create a custom Launcher"
    }
    note(id)
    u1.set_title(messages[id])
    
if __name__ == "__main__":
    b= gtk.Window()
    b.set_resizable(False)
    b.present_with_time(1)
    b.set_urgency_hint(True)
    
    b.set_default_size(700, 500)
    b.set_size_request(700, 500)

    itself = gtk.VBox()
    header = gtk.EventBox()
    header.set_visible_window(True)
    header.modify_bg(gtk.STATE_NORMAL, gtk.gdk.Color('#3c3b37'))
    
    notebook= gtk.Notebook()
    notebook.set_border_width(0)
    notebook.set_show_border(False)
    notebook.set_show_tabs(False)
    button_box = gtk.HButtonBox()
    button_box.set_layout(gtk.BUTTONBOX_END)
    button_box.set_border_width(6)
    #button_box.pack_start(gtk.Button('cancel'))
    close_button = gtk.Button('Close')
    button_box.pack_start(close_button)
    close_button.connect('clicked', lambda w: gtk.main_quit())
    itself.show_all()
    
    u1=UbuntuOneBin('Create a Launcher from System')
    vbox2 = gtk.VBox()
    vbox2.pack_start(u1, False,False)
    don=Doit()
    
    don.grad1.connect('toggled', change_page, notebook.set_current_page, 0, u1)
    don.grad2.connect('toggled', change_page,notebook.set_current_page,1, u1)
    don.grad3.connect('toggled', change_page,notebook.set_current_page,2,u1)
    vbox2.pack_start(don, False,False)
    
    header.add(vbox2)
    button_box.show_all()
    itself.pack_start(header, False,True)
    itself.pack_start(notebook, True,True,0)
    itself.pack_start(gtk.HSeparator(), False,True,0)
    itself.pack_start(button_box, False,True,0)
    
    fra=PageFromSystem()
    
    notebook.append_page(fra)
    
    pfile = PageFromFile()
    notebook.append_page(pfile)
    
    pnew=LauncherInfo()
    notebook.append_page(pnew)
    itself.show_all()
    b.add(itself)
    b.connect('delete-event', lambda w, e: gtk.main_quit())

    b.set_title("Add Launcher...")
    #.show_all()
    gtk.main()
