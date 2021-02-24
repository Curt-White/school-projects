This project uses a quad tree to test collisions between shapes in an efficient manner by reducing the number of necessary collision checks. The simulation is displayed in the console using the NCurses library.

In order to build this project, the boost and ncurses libraries are required.

Some brief instructions on how to use the program:
	* Click and drag the mouse from the top left corner of the desired rectangle to the bottom right (Otherwise will yield no rectangle)
	* Click and drag (must drag a little to register because of curses) on a rectangle to go into editing mode.
		* While in editing mode the rectangle will turn red
		* you will not be able to draw other rectangles
		* you can move the rectangle with the arrow keys
		* can leave editing mode by pressing escape
	* Exit the program with Shift-Q

To build the project:
	The project is built in the same way that the example project was presented. Provide an install directory
	to the first command then use that same directory when executing the demo file.

	cmake -H. -Btmp_cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
	cmake --build tmp_cmake --clean-first --target install
	$INSTALL_DIR/bin/demo

Some small known issues:
	* attempting to draw many rectangles in quick succession can lead to a rectangle's top left point being at the location of the previous rectangle
	* Only on the lab machines, sometimes items stretch to nearby items. For example, if you move two items exactly a certain number of characters away
		from each other and horizontally aligned, sometimes a trail will exist to the right of the left object. Since this only happens on the lab machine,
		I have been unable to figure out the cause of this issue. The lab machines also produce a flicker from refreshing that does not exist on my local machine.
		This is only an asthetic issue as the trail does not colliding.
