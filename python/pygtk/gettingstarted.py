#!/usr/bin/env python

import gtk

class HelloWorld:
    def __init__(self):
        window = gtk.Window()
        button = gtk.Button("Click Here")
        window.connect("destroy", lambda w: gtk.main_quit())
        button.connect("clicked", self.print_hello)

        window.add(button)
        window.show_all()

    def print_hello(self, widget):
        print "Hello World"

if __name__ == '__main__':
    HelloWorld()
    gtk.main()
