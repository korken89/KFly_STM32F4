# -*- coding: utf-8 -*-
#####################################################################
#  Laudeci Oliveira <laudeci@ubuntu.com>
#  Ursula Junques <ursinha@ubuntu.com>
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
import glob
import pango
from desktop import DesktopParser
from os.path import exists, expanduser, isdir, isfile, join
from xdg import BaseDirectory
from pkg_resources import resource_filename
from utils import DialogType, FileChooser, get_icon, MessageBox, nullstring
# Constants for column mapping
(COLUMN_ICON,
 COLUMN_NAME,
 COLUMN_DESKTOP,
 COLUMN_PATH,
 COLUMN_COMMENT,
 COLUMN_SORT) = range(6)
 
class AddLauncherDialog(gtk.Dialog):
    """XXX: add docs."""
    def __init__(self, title='Add Launcher...'):
        super(AddLauncherDialog, self).__init__(title,
            None, (gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT | 
                gtk.DIALOG_NO_SEPARATOR),
            (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL))

        glade_file = "application.glade"
        self.builder = gtk.Builder()
        glade_path = resource_filename(__name__, join("data", glade_file))
        #glade_path = join(expanduser("~/Desktop/unity-launcher-editor/data"), glade_file)
        if not exists(glade_path):
            raise Exception("Critical: couldn't find glade file: %s" % glade_path)
        self.builder.add_from_file(glade_path)
        self.add_panel = self.builder.get_object('add_new_laucher_box')

        self.ok_button = self.add_button(gtk.STOCK_OK, gtk.RESPONSE_OK)
        self.ok_button.grab_default()
        self.ok_button.set_sensitive(False)
        self.set_alternative_button_order(
            [gtk.RESPONSE_CANCEL, gtk.RESPONSE_OK])

        self.lvwAddLaunchers = self.builder.get_object('lvwAddLaunchers')
        self.txtSearch = self.builder.get_object('entry2')
        self.txtSearch.connect("icon-press", lambda x, y, z : self.filter.refilter())
        
        self.listmodel = gtk.ListStore(gtk.gdk.Pixbuf, gobject.TYPE_STRING,
            gobject.TYPE_PYOBJECT, gobject.TYPE_STRING, gobject.TYPE_STRING, gobject.TYPE_STRING)
        
        self.filter = self.listmodel.filter_new()
        self.filter.set_visible_func(self.match_type)
        self.sorter = gtk.TreeModelSort(self.filter)
        render_pixbuf = gtk.CellRendererPixbuf()
        render_text = gtk.CellRendererText()
               
        col = gtk.TreeViewColumn()
        
        col.pack_start(render_pixbuf, expand=False)
        col.add_attribute(render_pixbuf, 'pixbuf', COLUMN_ICON)
        col.pack_start(render_text, expand=True)
        col.add_attribute(render_text, 'text', COLUMN_NAME)
        self.lvwAddLaunchers.append_column(col)
        render_comment = gtk.CellRendererText()
        
        col = gtk.TreeViewColumn()
        col.pack_start(render_comment, expand=True)
        col.add_attribute(render_comment, 'text', COLUMN_COMMENT)
        #render_comment.set_property('ellipsize',pango.ELLIPSIZE_END)
        render_comment.set_property('wrap-mode',pango.WRAP_WORD)
        render_comment.set_property('wrap-width',500)
        self.lvwAddLaunchers.append_column(col)
        self.lvwAddLaunchers.set_model(self.sorter)
        #self.lvwAddLaunchers.set_search_entry(self.txtSearch);
        apps_list = []
        for n in  ['/usr/share/']:#BaseDirectory.xdg_data_dirs:
            desktop_dir = '%sapplications' % n
            
            for launcher_location in glob.glob('%s/*.desktop' % desktop_dir):
                # blacklist ubiquity as will have two ubiquity in the netbook live session then
                if not "ubiquity" in launcher_location:
                    apps_list.append(launcher_location) #= self.register_new_app(launcher_location, apps_list)
        apps_list.sort()
        self.populate(apps_list)
        self.listmodel.set_sort_column_id(COLUMN_SORT,gtk.SORT_ASCENDING)
        self.vbox.pack_start(self.add_panel, True, True, 0)
        

    def match_type(self, model, iter):
        value = model.get_value(iter, COLUMN_DESKTOP)

        if self.txtSearch.get_text_length() == 0:
            return True
        if self.txtSearch.get_text().lower() in value.get("Name").lower():
            return True
        else:
            return False
  
    def populate(self, values):
        var = []
        from gio import app_info_get_all
        
        items = app_info_get_all()
        for appinfo in items:
            sname=appinfo.get_name()
            icon = appinfo.get_icon()
            
            if icon is None:
                pix = get_icon('image-missing', 16)
            else:
                icon = icon.to_string()
                pix = get_icon(icon, 16)
            
            comm = appinfo.get_description()
            self.listmodel.append((pix, '%s' % sname, None, appinfo.get_id(), comm,sname.upper()))
        
        return
        for menu_item in values:
            print menu_item
            desktop_parser = DesktopParser(menu_item)
            desktop_parser.update_keys()
            sname = desktop_parser.get('Name')
            icon = desktop_parser.get('Icon')
            comm = desktop_parser.get('Exec')
            if sname is None:
                sname = desktop_parser.get('Name', 'en')
            if icon is None:
                pix = get_icon('image-missing', 16)
            else:
                pix = get_icon(icon, 16)
            var.append(('%s' % sname, pix, desktop_parser, menu_item, comm,sname.upper()))
            
            #self.listmodel.append((pix, '%s' % sname, desktop_parser, menu_item))
        #var.sort()
         
        for sname, pix, desktop_parser, menu_item, comm, ssort in var:
            self.listmodel.append((pix, '%s' % sname, desktop_parser, menu_item, comm,ssort.upper()))
        #self.main_window.listview_launcher.set_model(store)
    
    def get_desktop_dir(self):
        ''' no python binding from xdg to get the desktop directory? '''
        
        possible_desktop_folder = None
        try:
            for line in file('%s/user-dirs.dirs' % BaseDirectory.xdg_config_home):
                values = line.split('=')
                if values[0] == 'XDG_DESKTOP_DIR':
                    try:
                        possible_desktop_folder = values[1][1:-2].replace('$HOME', expanduser('~'))
                        if isdir(possible_desktop_folder):
                            return possible_desktop_folder
                        else:
                            possible_desktop_folder = None
                            break
                    except IndexError:
                        continue
        except IOError:
            pass
        return expanduser('~/Desktop')

  
    def register_new_app(self, launcher_location, apps_list):
        ''' append a new app with full desktop path if valid, take care of dups '''
        
        # default distribution launcher don't go into that function (as don't have an aboslute path)
        entry = ""
        if exists(launcher_location):
            print "    == %s: exists" % launcher_location
            # try to strip the full path we had in unity mutter if it's part of a xdg path:
            # or try to get that for other desktop file based on name.
            candidate_desktop_filename = launcher_location.split("/")[-1]
            # some desktop file with modified exec key (like in cairo-dock contains 01desktopfilename.desktop, strip that)
            try:
                candidate_cairodock_desktop_filename = candidate_desktop_filename.split("01")[1]
            except IndexError:
                candidate_cairodock_desktop_filename = ""
            for xdg_dir in BaseDirectory.xdg_data_dirs:
                xdg_app_dir = join(xdg_dir, "applications", "")
                if launcher_location.startswith(xdg_app_dir):
                    candidate_desktop_file = launcher_location.split(xdg_app_dir)[1]
                    # if really the xdg path is the path to the launcher
                    if not '/' in candidate_desktop_file:
                        entry = candidate_desktop_file
                        print "    Direct match found for system desktop file"
                        break
                # second chance: try to see if the desktop filename is in xdg path and so, assume it's a match
                if not entry and exists("%s/%s" % (xdg_app_dir, candidate_desktop_filename)):
                    entry = candidate_desktop_filename
                    print"    Similar desktop file name with system desktop file"
                    break
                # third chance: try to see if a tweaked cairo-dock like deskto file name is in xdg path
                if not entry and exists("%s/%s" % (xdg_app_dir, candidate_cairodock_desktop_filename)):
                    entry = candidate_cairodock_desktop_filename
                    print "    Similar Cairo-Dock -like desktop file name with system desktop file"
                    break
            # fourth and last chance: try to find a corresponding Exec key.
            # Wait! scanning /usr/share/applications is heavy !!!
            # Don't panic, we have the bamf.index for that :)
            if not entry:
                exec_arg = ""
                try:
                    for line in open(launcher_location):
                        if "Exec=" in line:
                            exec_arg = line.split("Exec=")[1]
                            break
                except IOError:
                    print "    Can't open %s for reading Exec" % launcher_location     
                if exec_arg:
                    try:
                        for line in open("/usr/share/applications/bamf.index"):
                            if exec_arg in line:
                                entry = line.split()[0]
                                print "    Coherent exec key found with system desktop file"
                                break
                    except IOError:
                        print "    No bamf.index file found on the system!"
                
            if not entry:
                entry = launcher_location
            print "    %s: real entry is %s" % (launcher_location, entry)
            if entry not in apps_list:
                print "    --- adding %s as not in app_list" % entry
                apps_list.append(entry)
            else:
                print"    --- NOT adding %s as already in app_list" % entry
        else:
            print"    == %s: doesn't exist" % launcher_location

        return apps_list 
       
    def run(self, destroy=True):
        """Returns True if yes was clicked, False otherwise."""
        resp = super(AddLauncherDialog, self).run()
        obj = None
        if resp == gtk.RESPONSE_OK:
            obj = {'GroupName': self.txtGroup.get_text(),
                   'Name': self.txtName.get_text(),
                   'Exec': self.txtCommand.get_text(),
                   'TargetEnvironment': (
                      self.cmbTargetEnvironment.get_active_text()),
            }
        if destroy:
            self.destroy()
        if resp == gtk.RESPONSE_OK:
            return True, obj
        else:
            return False, None


if __name__ == "__main__":
    dialog = AddLauncherDialog()
    result, group = dialog.run()
