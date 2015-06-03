import gtk

class App(object):
        def __init__(self):
                self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
                self.window.set_title("MyApp1")
                self.window.set_size_request(400,400)
                self.window.connect("delete_event",gtk.main_quit)
                
                self.box0 = gtk.VBox()
                self.box1 = gtk.HBox()
                self.box0.pack_start(self.box1, fill=False)
                self.box1.show()
                
                self.quit_button = gtk.Button("Quit", gtk.STOCK_QUIT)
                self.quit_button.set_size_request(200,200)
                self.quit_button.set_tooltip_text("Close this window")
                self.quit_button.connect("clicked",gtk.main_quit)
                
                self.box1.pack_start(self.quit_button, fill=False)
                
                self.window.add(self.box0)
                self.box0.show()
                self.quit_button.show()
                
                self.window.show()
                
a = App()
gtk.main()
