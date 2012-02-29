from gi.repository import Gio
from mainwindow import MainWindow
import os
from gobject import GError

class AppGUI:
    """XXX: add docs."""

    def __init__(self):
        """Display the main window."""
        self.gsettings = Gio.Settings('com.canonical.Unity.Launcher')
        self.main_window = MainWindow(self)
        self.main_window.show()
        self.get_unity_list()

    def save_unity_list(self, menu_items):
        try:
            print str(menu_items)
            self.gsettings.set_strv('favorites', menu_items)
            self.get_unity_list()
            return True
        except GError, e:
            return False
        #return_code = os.system('unity --reset') #subprocess.call(['unity','--reset'])
        #if return_code != 0:
        #    print "Settings fail to transition to new unity compiz favorites"

    def get_unity_list(self):
        """ Create a list of items from unity's Launcher Panel."""
        unity = self.gsettings.get_value('favorites')

        self.main_window.load_launcher_from_list(unity)
        


