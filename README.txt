---DEPENDENCIES---
  You will need:
    OpenGL (openGL renderer atleast 4.6)
    GL dev library (atleast 1.2)
    GLEW dev library (alteast 2.2)
    SDL2 library (atleast 2.0)
    automake
    autoconf
    make
    GNU Compiler collection

---BUILD---
  Make sure your inside the programs directory:
    cd path/to/program/directory
  (replace the path/to/program/directory with the actual directory names)

  Then, you'll need allow autogen.sh to run as a porgram:
    chmod +x autogen.sh

  Then run the autogen.sh
    ./autogen.sh

  If the configuration ended successfully run the makefile:
    make

  Lastly run this command:
    sudo make install

  You can wright 'Spinning_cube' in the terminal (no matter which directory) to run the program !
