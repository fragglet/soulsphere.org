// Emacs style mode select -*- java -*-
//----------------------------------------------------------------------
//
// Game of life java applet
//
// By Simon Howard
//
//----------------------------------------------------------------------

// appletviewer tag:
// <applet width=430 height=300 code=LifeApplet></applet>

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.applet.*;
import java.net.*;

//
// board class
//
// this is abstracted and does not apply to any particular 'game'
// to use it for life we derive a LifeBoard class
// 

class Board extends Canvas {

    public static final int cellsize = 8;       // size of cells on board (pixels)

    protected int xcells, ycells;               // board size
    protected Color[][] board;                 // board

    // constructor

    public Board(int x, int y) {
	board = new Color[x][y];
	xcells = x;
	ycells = y;

	// set the size of this component

	setSize(xcells * cellsize + 1, ycells * cellsize + 1);

	// add an action listener to allow the user to toggle the
	// values of the individual cells

	addMouseListener(new MouseListener() {
		public void mouseClicked(MouseEvent event) {
		    toggleCell(event.getX() / cellsize,
			       event.getY() / cellsize);
		    refresh();
		}

		// these aren't used:

		public void mouseDragged(MouseEvent e) {}
		public void mouseMoved(MouseEvent e) {}
		public void mousePressed(MouseEvent e) {}
		public void mouseReleased(MouseEvent e) {}
		public void mouseEntered(MouseEvent e) {}
		public void mouseExited(MouseEvent e) {}
	    });
    }

    public Board() {
	this(8, 8);                  // 8x8 default
    }

    // find if a particular cell is 'alive'

    public boolean cellAlive(int x, int y) {
	if(x < 0 || y < 0 || x >= xcells || y >= ycells)
	    return false;
	else
	    return board[x][y] != null;
    }

    // set value of cell

    public void setCell(int x, int y, Color value) {

	// sanity check

	if(x < 0 || y < 0 || x >= xcells || y >= ycells)
	    return;

	board[x][y] = value;
    }

    // toggle value of cell

    public void toggleCell(int x, int y) {

	// sanity check

	if(x < 0 || y < 0 || x >= xcells || y >= ycells)
	    return;

	// jump to next

	board[x][y] =
	    board[x][y] == null ? Color.red :
	    board[x][y] == Color.red ? Color.blue : null;
    }

    // returns the number of neighbouring squares for a
    // particular square which are 'alive'

    public int cellNeighbours(int x, int y) {

	// sanity check

	if(x < 0 || y < 0 || x >= xcells || y >= ycells)
	    return 0;

	// count up neighbours

	int total = 0;

	for(int i=-1; i<=1; i++)
	    for(int j=-1; j<=1; j++) {

		// if this is a neighbouring cell and it is alive
		// then add to the total

		if((i != 0 || j != 0) &&
		   cellAlive(x + i, y + j))
		    total++;
	    }

	return total;
    }

    // paint method to display to applet etc.

    public void paint(Graphics g) {

	// draw each cell in turn

	for(int x=0; x<xcells; x++)
	    for(int y=0; y<ycells; y++) {

		// if this cell is alive then draw a filled square (red or blue)
		// else just draw the cell border
		// we assume the background has already been cleared
		
		if(board[x][y] == null) {
		    g.setColor(Color.black);
		    g.clearRect(x * cellsize, y * cellsize, cellsize, cellsize);
		    g.drawRect(x * cellsize, y * cellsize, cellsize, cellsize);
		} else {
		    g.setColor(board[x][y]);
		    g.fillRect(x * cellsize, y * cellsize, cellsize, cellsize);
		}
	    }	
    }

    // redraw the component
    // unlike repaint this does not wipe the background (which can be ugly)
    // this is kind of messy

    public void refresh() {
	Graphics g = getGraphics();
	//	g.clearRect(0, 0, xcells * cellsize, ycells * cellsize);
	paint(g);
    }

    // randomize: fill board with random squares 

    public void randomize() {
	for(int x=0; x<xcells; x++)
	    for(int y=0; y<ycells; y++) {

		// random colour:

		if(Math.random() < 0.2)
		    board[x][y] =
			new Color((int) (Math.random() * 255),
				  (int) (Math.random() * 255),
				  (int) (Math.random() * 255));
		else
		    board[x][y] = null;
			
	    }

	repaint();
    }

    // clear the board

    public void clear() {
	for(int x=0; x<xcells; x++)
	    for(int y=0; y<ycells; y++)
		board[x][y] = null;
	repaint();
    }

    // load in a text file(url) with board data

    public void loadBoard(URL url) {

	// clear board first

	clear();

	// open url
	
	BufferedReader input;

	try {
	    input = new BufferedReader(new InputStreamReader(url.openStream()));
	} catch(IOException e) {
	    return;
	}

	// read each line in
	// increment y with each line read

	String inputline;

	int y = 0;

	while(true) {

	    // read next line from url

	    try {
		inputline = input.readLine();
	    } catch(IOException e) {
		break;
	    }

	    // if last line, exit loop

	    if(inputline == null)
		break;

	    // comment

	    if(inputline.charAt(0) == '#')
		continue;

	    // set cells for this line

	    for(int x=0; x<inputline.length(); x++) {
		char c = inputline.charAt(x);
		
		if(c == '1')
		    setCell(x, y, Color.red);
		else if(c == '2')
		    setCell(x, y, Color.blue);
		else if(c == '3')
		    setCell(x, y, Color.green);
		else if(c == '4')
		    setCell(x, y, Color.magenta);
		else if(c == '5')
		    setCell(x, y, Color.cyan);
		else if(c == '6')
		    setCell(x, y, Color.yellow);
	    }

	    y++;        // next line
	}

	repaint();
    }
}

//
// the LifeBoard class is derived from the Board class and
// implements the Life 'game'
//

class LifeBoard extends Board {

    // constructor

    public LifeBoard() {
	super();
    }

    public LifeBoard(int x, int y) {
	super(x, y);
    }

    // mixes colours of nearby cells to 
    // create a new colour

    public Color mixColour(int x, int y) {

	// sanity check

	if(x < 0 || y < 0 || x >= xcells || y >= ycells)
	    return null;

	// count up colours of neighbours

	int r=0, g=0, b=0, c=0;

	for(int i=-1; i<=1; i++)
	    for(int j=-1; j<=1; j++) {

		// if this is a neighbouring cell and it is alive
		// then add to the total

		if((i != 0 || j != 0) &&
		   cellAlive(x + i, y + j)) {

		    r += board[x + i][y + j].getRed();
		    g += board[x + i][y + j].getGreen();
		    b += board[x + i][y + j].getBlue();
		    c++;
		}
	    }

	// return averaged colour

	return new Color(r/c, g/c, b/c);
    }

    // generate the next pattern

    public void generateNext() {

	// use the existing board to generate this new board:

	Color[][] newboard = new Color[xcells][ycells];

	for(int x=0; x<xcells; x++)
	    for(int y=0; y<ycells; y++) {
		int neighbours = cellNeighbours(x, y);

		if(cellAlive(x, y)) {

		    // cell only continues to live on the new board if
		    // there are 2 or 3 neighbours
		    // otherwise it dies

		    if(neighbours < 2 || neighbours > 3)
			newboard[x][y] = null;
		    else
			newboard[x][y] = board[x][y];

		} else {

		    // if we have 3 neighbours (parents) then bring this
		    // cell to life on the new board

		    if(neighbours == 3)
			newboard[x][y] = mixColour(x, y);
		    else
			newboard[x][y] = null;
		}
	    }

	// update board

	board = newboard;

	//repaint();
    }
}

//
// the Applet
// 

public class LifeApplet extends Applet {
    LifeBoard board;
    List presets = new List();

    public void init() {

	// create board
	// infact board is derived from Canvas and so is a component
	// we add it as we would add a button or other component

	board = new LifeBoard(40, 25);
	board.randomize();
	
	add(board);

	// put all the buttons on a panel

	Panel controls = new Panel();

   	add(controls, BorderLayout.SOUTH);

	// create next button

	Button nextButton = new Button("next");

	controls.add(nextButton, BorderLayout.SOUTH);

	// this is much less messy than having to mess about using
	// the applet class as the actionlistener and then trying
	// to figure out which button has been pressed

	nextButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent event) {
		    board.generateNext();
		    board.refresh();
		}
	    });

	// 'run' button
	// forward 10 moves

	Button runButton = new Button("run");

	controls.add(runButton, BorderLayout.SOUTH);

	// this is much less messy than having to mess about using
	// the applet class as the actionlistener and then trying
	// to figure out which button has been pressed

	runButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent event) {
		    for(int i=0; i<10; i++) {

			// advance and redraw

			board.generateNext();
			board.refresh();

			// delay before next frame

			try {
			    Thread.sleep(50);
			} catch(Exception e) {
			}
		    }
		}
	    });

	// add clear button

	Button clearButton = new Button("clear");
	
	controls.add(clearButton);

	clearButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent event) {
		    board.clear();
		}
	    });

	// add randomize button

	Button randomButton = new Button("random");
	
	controls.add(randomButton);

	randomButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent event) {
		    board.randomize();
		}
	    });

	// preset list 

	presets.add("pulsar.txt");
	presets.add("virus.txt");
	presets.add("glider.txt");
	presets.add("oscillator1.txt");
	presets.add("honeyfarm.txt");

	controls.add(presets);

	// button to load preset

	Button loadButton = new Button("load");
	
	controls.add(loadButton);

	loadButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent event) {
		    try {
			URL url = new URL(getDocumentBase(), 
					  presets.getSelectedItem());
			board.loadBoard(url);
		    } catch(MalformedURLException e) {
		    }
		}
	    });

    }

    // there is no need for any paint method

    // public void paint(Graphics g) {}
}
