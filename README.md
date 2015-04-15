# Starship Fontana #
This is an example C++ application using the SDL library.
It tries to be as nicely C++11 as possible but do keep in
mind that SDL is written in C and, at some stage, you have
to interface with it.

## Story ##
The evil b’Kuhn has stolen the code to Earth’s defence system.
With this code he can, at any time, defeat the entire human race.
Only one woman is brave enough to go after b’Kuhn. Will she be
Earth’s hero? Puzzle your way though the universe in the company
of Commander Fontana in **Starship Fontana**.

## Installation ##
You will need the SDL development libraries installed on
your system.

This installation guide assumes the use of **Fedora**.

Start by installing the required libraries:

* Groups:
* * C Development Tools and Libraries
* Packages:
* * SDL2-devel
* * SDL2_image-devel
* * gcc-c++

You can run the commands below to fetch these libraries:

```bash
  $ yum groupinstall "C Development Tools and Libraries"
  $ yum install SDL2-devel SDL2_image-devel gcc-c++
```

The easiest way to compile is to use a command-line.
The commands below will compile the game into an executable file `starship` in the
top-level directory:

```bash
  $ g++ -c src/*.cpp
  $ g++ -o starship *.o -l SDL2 -l SDL2_image
```

To run the compiled game do the following:

```bash
  $ ./starship
```
 
From the top-level directory, the game will expect to find the
`assets` directory under its current working directory.

* Repo
* * **starship** (executable)
* * **assets** (folder)
* * src (folder)
* * [etc ...]

## Issues ##
* SDL1 to SDL2 port introduced bounding box collision issues.

## Credits ##
The sprites in this game come directly from 
[SpriteLib](http://www.widgetworx.com/widgetworx/portfolio/spritelib.html) and are used
under the terms of the [CPL 1.0](http://opensource.org/licenses/cpl1.0.php).

The event system is based on [Libsigc++](http://libsigc.sourceforge.net/)
and is used under the [LGPL](http://www.gnu.org/copyleft/lgpl.html).

The main game code comes from [AidanDelaney/StarshipFontana](https://github.com/AidanDelaney/StarshipFontana) please check the [License](https://github.com/AidanDelaney/StarshipFontana/blob/master/LICENCE) for more information.