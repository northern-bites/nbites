To build the library from source, you can use the aldebaran's qibuild 
build framework:

Create a toolchain with the cross compiler for your robot, e.g. cross-atom

Configure the project with 
  qibuild configure libgamectrl -c yourtoolchain

Build the project with 
  qibuild make libgamecrtl -c yourtoolchain  --release

Copy the library on the nao and add it to your autoload.ini file.