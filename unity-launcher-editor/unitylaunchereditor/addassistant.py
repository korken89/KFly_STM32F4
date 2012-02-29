from os.path import expanduser, join
import gobject
import gtk
from pango import WRAP_WORD,ELLIPSIZE_END

from gio import app_info_get_all,ThemedIcon, AppInfo
from os.path import isfile
from unitylaunchereditor.core.iconmanager import IconManager
from unitylaunchereditor.dialogs.dialogs import MessageBox
from unitylaunchereditor.core.translation import _
from unitylaunchereditor.widgets.launcherview import LauncherView
from unitylaunchereditor.widgets.launcherinfo import LauncherInfo
from unitylaunchereditor.core.utils import normalize_path

class PageFromInfo(gtk.VBox):
    __gsignals__ = { 'add-clicked': (
                                 gobject.SIGNAL_RUN_LAST,
                                 gobject.TYPE_NONE,
                                 (gobject.TYPE_PYOBJECT,),
                                 ),
    }
    
    def __init__(self):
        super(PageFromInfo, self).__init__()
        self.set_border_width(12)
        
        self.launcherinfo = LauncherInfo()
        
        bbox = gtk.HButtonBox()
        bbox.set_layout(gtk.BUTTONBOX_END)
        button = gtk.Button(stock=gtk.STOCK_ADD)
        button.connect('clicked', self.__on_add_clicked)
        
        bbox.pack_end(button, expand=False, fill=False, padding=6)
        self.pack_start(self.launcherinfo,fill=False,expand=False,padding=0)
        self.pack_end(bbox,fill=True,expand=False,padding=0)
        
        
    def __on_add_clicked(self, widget):
        obj = {'Name': self.launcherinfo.launcher_name}
        obj['Exec']= self.launcherinfo.launcher_command
        obj['Comment']= self.launcherinfo.launcher_comment
        obj['Terminal'] = self.launcherinfo.launcher_run_terminal
        lname = self.launcherinfo.launcher_icon_name
        lname =lname.strip()
        if len(lname)>0:
            obj['Icon'] = lname
        else:
            obj['Icon']='image-missing'
            
        self.emit('add-clicked', obj)
        
    
class PageFromFile(gtk.VBox):
    __gsignals__ = { 'add-clicked': (
                                 gobject.SIGNAL_RUN_LAST,
                                 gobject.TYPE_NONE,
                                 (gobject.TYPE_STRING,),
                                 ),
    }
    def __init__(self):
        super(PageFromFile, self).__init__()
        self.set_border_width(12)
        self.__gtk_file= gtk.FileChooserWidget()
        
        filter = gtk.FileFilter()
        filter.set_name("Desktop files")
        filter.add_mime_type("application/x-desktop")
        filter.add_pattern("*.desktop")
        
        self.__gtk_file.set_filter(filter)
        self.__gtk_file.set_current_folder(expanduser('~/.local/share/applications/'))
        self.pack_start(self.__gtk_file,True,True)
        
        self.bbox = gtk.HButtonBox()
        self.bbox.set_layout(gtk.BUTTONBOX_END)
        button = gtk.Button(stock=gtk.STOCK_ADD)
        button.connect('clicked', self.__on_add_clicked)
        self.bbox.pack_end(button, expand=False, fill=False, padding=0)
        self.pack_start(self.bbox,fill=True,expand=False,padding=6)
        
    def __on_add_clicked(self, widget):
        filename = self.__gtk_file.get_filename()
        self.emit('add-clicked', filename)
        

class PageFromSystem(gtk.Frame):
    __gsignals__ = { 'add-clicked': (
                                 gobject.SIGNAL_RUN_LAST,
                                 gobject.TYPE_NONE,
                                 (gobject.TYPE_PYOBJECT,gobject.TYPE_PYOBJECT,),
                                 ),
    }
    def __init__(self):
        super(PageFromSystem,self).__init__()
        self.icon_manager = IconManager()
        self.set_shadow_type(gtk.SHADOW_NONE)
        self.set_border_width(12)
        self.vbox1 = gtk.VBox()
        self.bbox = gtk.HButtonBox()
        self.bbox.set_layout(gtk.BUTTONBOX_END)
        button = gtk.Button(stock=gtk.STOCK_ADD)
        button.connect('clicked', self.__on_add_clicked)
        self.bbox.pack_end(button, expand=False, fill=True, padding=0)
        
        self.scrolledwindow1 = gtk.ScrolledWindow()
        self.scrolledwindow1.set_shadow_type(gtk.SHADOW_ETCHED_IN)
        
        
        self.hbox1 = gtk.HBox()
        self.label2 = gtk.Label('Find an item to add to panel:')
        self.txtSearch = gtk.Entry()
        self.txtSearch.set_icon_from_stock (gtk.ENTRY_ICON_SECONDARY, gtk.STOCK_CLEAR)
        self.txtSearch.connect("changed", self.on_change)
        self.txtSearch.connect("icon-press", lambda x, y, z : self.txtSearch.set_text(''))
        
        self.hbox1.pack_start(self.label2,expand=False, fill=True, padding=4)
        self.hbox1.pack_start(self.txtSearch,expand=True, fill=True, padding=0)
        
        self.vbox1.pack_start(self.hbox1,expand=False, fill=True, padding=5)
        self.vbox1.pack_start(self.scrolledwindow1,expand=True, fill=True, padding=4)
        self.vbox1.pack_start(self.bbox,expand=False, fill=True, padding=0)
        self.add(self.vbox1) 
            
        self.lvwApplications = LauncherView()
        self.lvwApplications.enable_filter(self.match_type)
        self.scrolledwindow1.add(self.lvwApplications)
        
        self.populate()
        
    def __on_add_clicked(self, widget):
        path, iter = self.lvwApplications.get_selected_row()
        self.emit('add-clicked',self.lvwApplications.get_model(),iter)
        
    def on_change(self,w):
        #if self.txtSearch.get_text_length()>3:
        self.lvwApplications.refilter()
           
    def match_type(self, model, iter):
        value = model.get_value(iter, 4)

        if self.txtSearch.get_text_length() == 0:
            return True
        if self.txtSearch.get_text().lower() in value.lower():
            return True
        else:
            return False  
            
    def populate(self):
        items = app_info_get_all()
        for appinfo in items:
            
            if isfile(normalize_path(appinfo.get_id())):
                sname=appinfo.get_name().replace('&', '&amp;')
                comm = ('%s' % appinfo.get_description()) if appinfo.get_description() else ''
                comm = comm.replace('&', '&amp;')
                pix=self.icon_manager.get_icon(appinfo, 32)
                
                title = ('<b>%s</b>\n%s' % (sname,comm)) if comm else '<b>%s</b>' % sname
    
                row=(pix, title, appinfo, sname.upper(),appinfo.get_id())
                self.lvwApplications.add_row(row)
            
        return