optimizations = "-Ofast"

env = Environment(CC="g++",
                  CCFLAGS=' '.join(["-std=c++11", optimizations]),
                  CPPPATH=["src"],
                  LIBS=["X11", "pthread"])

env.Program("bin/find_panel_boxes", ["src/find_panel_boxes.cpp"])
env.Program("bin/filter_panels", ["src/filter_panels.cpp"])
env.Program("bin/make_mosaic", ["src/make_mosaic.cpp", "src/Bitmap.cpp"])
