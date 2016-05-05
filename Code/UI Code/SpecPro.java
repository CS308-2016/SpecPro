/**
* Team Id: SPEC PRO
* Author List: Ramprakash K
* Filename: SpecPro.java
* Description: Graphic User Interface for the Project
*/

import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.*;

import javax.imageio.ImageIO;
import javax.swing.*;

class Data {
	public int number;		//! number	: Number of the entry 
	public String time;		//! time	: TimeStamp of the speeding event
	public String vNo;		//! vno		: Vehicle number
	public String speed;	//! speed	: Vehicle speed
	public Image img;		//! img		: Original image of the vehicle
	public Image img1;		//! img1	: Image of the Number plate alone
	public Image img2;		//! img2	: Binarized Image of the Number plate
	
	/**
	* Function Name: Data()
	* Logic: Initialize Data object with details of the Speeding Vehicle
	*/
	public Data(int n,String a,String b,String c,Image d,Image d1,Image d2) {
		number = n;
		time = a;
		vNo = b;
		speed = c;
		img = d;
		img1 = d1;
		img2 = d2;
	}
}

public class SpecPro {
	
	private static JFrame frame;		//! frame	 : Stores the frame of the window
	private static BufferedReader br;	//! br		 : For reading from the file
	private static int dataCount;		//! dataCount: The number of vehicles' data read
	
	private static JPanel panel;			//! panel	: Panel for the heading of the table
	private static GridBagLayout layout;	//! layout	: Layout(grid) for the table of entries
	private static JPanel pane;				//! pane	: Panel for the table entries
	private static Box table;				//! table	: Box containing heading panel and the table panel
	
	private static JPanel panel1;	//! panel1	: Panel for Individual vehicle details
	private static JLabel vno;		//! vno		: Label for Vehicle Number
	private static JLabel spd;		//! spd		: Label for Vehicle Speed
	private static JLabel tim;		//! tim		: Label for TimeStamp of the speeding event
	private static JLabel img;		//! img		: Label for the image of the vehicle
	private static JLabel img1;		//! img1	: Label for the image of the number plate alone
	private static JLabel img2;		//! img2	: Label for the binarized image of the number plate
	
	/**
	* Function Name: main
	* Logic: Window is created, Data is read from file "data.txt" and fed to the window frame
	*/
	public static void main(String[] args){
		dataCount = 0;
		//! Initialize Frame
		frame = new JFrame("SpecPro GUI");
		frame.setBounds(100, 100, 508, 400);
		frame.setResizable(false);
		initialize(frame.getContentPane());
		frame.addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent event) {
				try {
					br.close();
				} catch (IOException e) {}
				try {
					Runtime.getRuntime().exec("make clean");
				} catch (Exception e) {}
				System.exit(0);
			}
		});
		frame.pack();
		frame.setVisible(true);
		
		//! Read from file and add to table
		String line = null;
		while(frame.isVisible()) {
			try {
				br = new BufferedReader(new FileReader("img/data.txt"));
			} catch (FileNotFoundException e1) {
				System.out.println("data.txt not found in img folder");
				System.exit(0);
			}
			for (int i = 0; i < dataCount; i++) {
				try {
					line = br.readLine();
				} catch (IOException e) {
					System.exit(0);
				}
			}
			try {
				line = br.readLine();
			} catch (IOException e) {
				System.exit(0);
			}
			while (line != null) {
				System.out.println(line);
				String info[] = line.split("[|]");
				File f = new File("img/"+info[3]);
				File f1 = new File("img/"+info[4]);
				File f2 = new File("img/"+info[5]);
				Data d;
				Image sc = null,sc1 = null,sc2 = null;
				try {
					BufferedImage im = ImageIO.read(f);
					sc = im.getScaledInstance(180, 150, Image.SCALE_SMOOTH);
				} catch(IOException e) {}
				try {
					BufferedImage im1 = ImageIO.read(f1);
					sc1 = im1.getScaledInstance(90, 50, Image.SCALE_SMOOTH);
				} catch(IOException e) {}
				try {
					BufferedImage im2 = ImageIO.read(f2);
					sc2 = im2.getScaledInstance(90, 50, Image.SCALE_SMOOTH);
				} catch(IOException e) {}
				d = new Data(dataCount+1,info[0],info[1],info[2],sc,sc1,sc2);
				addData(d);
				try {
					line = br.readLine();
				} catch (IOException e) {
					System.exit(0);
				}
			}
			try {
				br.close();
			} catch (IOException e) {}
		}
	}

	/**
	 * Function name : initialize
	 * Input : Container of the window's frame
	 * Logic : Initialize the contents of the frame.
	 */
	private static void initialize(final Container container){
		ScrollPane scrollPane = new ScrollPane(ScrollPane.SCROLLBARS_AS_NEEDED);
		
		//! Initialize heading panel
		panel = new JPanel();
		panel.setLayout(new FlowLayout(FlowLayout.LEFT,0,0));
		
		JLabel label0 = new JLabel("No.", SwingConstants.CENTER);
		label0.setBorder(BorderFactory.createRaisedBevelBorder());
		label0.setPreferredSize(new Dimension(50,30));
		panel.add(label0);
		
		JLabel label1 = new JLabel("Time Stamp", SwingConstants.CENTER);
		label1.setBorder(BorderFactory.createRaisedBevelBorder());
		label1.setPreferredSize(new Dimension(150,30));
		panel.add(label1);
		
		JLabel label2 = new JLabel("Vehicle Number", SwingConstants.CENTER);
		label2.setBorder(BorderFactory.createRaisedBevelBorder());
		label2.setPreferredSize(new Dimension(150,30));
		panel.add(label2);
		
		JLabel label3 = new JLabel("Speed", SwingConstants.CENTER);
		label3.setBorder(BorderFactory.createRaisedBevelBorder());
		label3.setPreferredSize(new Dimension(150,30));
		panel.add(label3);
		
		//! Initialize table panel
		pane = new JPanel();
		layout = new GridBagLayout();
		pane.setLayout(layout);
		scrollPane.add(pane);
		scrollPane.setPreferredSize(new Dimension(350,350));
		
		table = Box.createVerticalBox();
		table.add(panel);
		table.add(scrollPane);
		
		//! Initialize Vehicle Details panel
		panel1 = new JPanel();
		panel1.setBounds(0, 0, 500, 370);
		panel1.setLayout(null);

		JLabel title = new JLabel("Vehicle Details");
		title.setHorizontalAlignment(SwingConstants.CENTER);
		title.setFont(new Font("Verdana", Font.BOLD, 15));
		title.setBounds(162, 27, 162, 34);
		panel1.add(title);
		
		JLabel vnol = new JLabel("Vehicle No :",SwingConstants.RIGHT);
		vnol.setBounds(27, 103, 84, 29);
		panel1.add(vnol);
		
		JLabel spdl = new JLabel("Speed :",SwingConstants.RIGHT);
		spdl.setBounds(27, 156, 84, 29);
		panel1.add(spdl);
		
		JLabel timl = new JLabel("Time :",SwingConstants.RIGHT);
		timl.setBounds(27, 212, 84, 29);
		panel1.add(timl);
		
		vno = new JLabel("Number", SwingConstants.LEFT);
		vno.setBounds(121, 103, 99, 29);
		panel1.add(vno);
		
		spd = new JLabel("Speed", SwingConstants.LEFT);
		spd.setBounds(121, 156, 99, 29);
		panel1.add(spd);
		
		tim = new JLabel("Time", SwingConstants.LEFT);
		tim.setBounds(121,212,99,29);
		panel1.add(tim);
		
		img = new JLabel();
		img.setBounds(259, 72, 180, 150);
		panel1.add(img);
		
		img1 = new JLabel();
		img1.setBounds(259, 222, 90, 50);
		panel1.add(img1);
		
		img2 = new JLabel();
		img2.setBounds(349, 222, 90, 50);
		panel1.add(img2);
		
		JButton btnBack = new JButton("Back");
		btnBack.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				panel1.setVisible(false);
				container.remove(panel1);
				table.setVisible(true);
				container.add(table, BorderLayout.CENTER);
			}
		});
		btnBack.setBounds(192, 297, 117, 34);
		panel1.add(btnBack);
		
		container.add(table, BorderLayout.CENTER);
	}
	
	/**
	 * Function Name : getAdapter
	 * Input : Data object with Vehicle Details
	 * Output : Action to switch view mode on clicking data in table
	 * Logic : Feed data to the global labels and switch the panels in the container
	 */
	private static MouseAdapter getAdapter(final Data d) {
		return new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent me) {
				Container container = frame.getContentPane();
				table.setVisible(false);
				container.remove(table);
				vno.setText(d.vNo);
				tim.setText(d.time);
				spd.setText(d.speed);
				if (d.img != null) {
					img.setText("");
					img.setIcon(new ImageIcon(d.img));
				} else {
					img.setIcon(null);
					img.setText("Image Unavailable");
				}
				if (d.img1 != null) {
					img1.setText("");
					img1.setIcon(new ImageIcon(d.img1));
				} else {
					img1.setIcon(null);
					img1.setText("No Image");
				}
				if (d.img2 != null) {
					img2.setText("");
					img2.setIcon(new ImageIcon(d.img2));
				} else {
					img2.setIcon(null);
					img2.setText("No Image");
				}
				panel1.updateUI();
				panel1.setVisible(true);
				container.add(panel1, BorderLayout.CENTER);
			}
		};
	}
	
	/**
	 * Function Name : addData
	 * Input : Data object d
	 * Logic : Add data to the table after proper formatting
	 * 		   and add the MouseAdapter to it from getAdapter()
	 */
	private static void addData(Data d) {
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.fill = GridBagConstraints.HORIZONTAL;
		gbc.anchor = GridBagConstraints.NORTHWEST;
		
		dataCount++;
		MouseAdapter adapter = getAdapter(d);
		
		JTextArea label0 = new JTextArea(Integer.toString(d.number));
		label0.setEditable(false);
		label0.setBorder(BorderFactory.createRaisedSoftBevelBorder());
		label0.addMouseListener(adapter);
		label0.setLineWrap(true);
		label0.setWrapStyleWord(true);
		label0.setPreferredSize(new Dimension(46,25));
		gbc.gridx = 0;gbc.gridy = d.number - 1;gbc.weighty = 1;
		pane.add(label0, gbc);
		
		JTextArea label1 = new JTextArea(d.time);
		label1.setEditable(false);
		label1.setBorder(BorderFactory.createRaisedSoftBevelBorder());
		label1.addMouseListener(adapter);
		label1.setLineWrap(true);
		label1.setWrapStyleWord(true);
		label1.setPreferredSize(new Dimension(150,25));
		gbc.gridx = 1;gbc.gridy = d.number - 1;gbc.weighty = 1;
		pane.add(label1, gbc);
		
		JTextArea label2 = new JTextArea(d.vNo);
		label2.setEditable(false);
		label2.setBorder(BorderFactory.createRaisedSoftBevelBorder());
		label2.addMouseListener(adapter);
		label2.setLineWrap(true);
		label2.setWrapStyleWord(true);
		label2.setPreferredSize(new Dimension(150,25));
		gbc.gridx = 2;gbc.gridy = d.number - 1;gbc.weighty = 1;
		pane.add(label2, gbc);
		
		JTextArea label3 = new JTextArea(d.speed);
		label3.setEditable(false);
		label3.setBorder(BorderFactory.createRaisedSoftBevelBorder());
		label3.addMouseListener(adapter);
		label3.setLineWrap(true);
		label3.setWrapStyleWord(true);
		label3.setPreferredSize(new Dimension(150,25));
		gbc.gridx = 3;gbc.gridy = d.number - 1;gbc.weighty = 1;
		pane.add(label3, gbc);
		
		if (dataCount > 1) {
			updateWeights();
		}
		pane.updateUI();
	}
	
	/**
	 * Function Name : updateWeights
	 * Logic : Set so that the table shows data from top to bottom
	 */
	private static void updateWeights() {
		for (int i = 0; i < 4; i++) {
			JTextArea c = (JTextArea) pane.getComponent(4*dataCount - 8 + i);
			GridBagConstraints gbc = layout.getConstraints(c);
			gbc.weighty = 0;
			pane.remove(4*dataCount - 8 + i);
			pane.add(c, gbc, 4*dataCount - 8 + i);
		}
	}
}
